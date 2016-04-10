#include "_.h"

extern Interpreter at_currentInterpreter;
extern Value AT_VALUES;
extern Value at_TV_minIndex, at_TV_maxIndex;
// ================================================================================================
// Assoc
// ================================================================================================
void Assoc_free (Assoc self) {
	Assoc i = NULL;
	if (!self) return;
	while (i = self->next) {
		self->next = i->next;
		free(i);
	}
	free(self);
}
Assoc Assoc_new () {
	Assoc node = struct_new(Assoc);
	node->key = NULL;
	node->value = NULL;
	node->next = NULL;
	return node;
}
Assoc Assoc_add (Assoc self, Value key, Value value) {
	if (!(self && key)) return NULL;
	Assoc assoc = struct_new(Assoc);
	assoc->key = key;
	assoc->value = value;
	assoc->next = self->next;
	self->next = assoc;
	return assoc;
}
Assoc Assoc_has (Assoc self, Value key) {
	if (!(self && key)) return NULL;
	Assoc i = self->next, j = self;
	for (; i && i->key != key; i = (j = i)->next);
	if (i && j != self) {
		j->next = i->next;
		i->next = self->next;
		self->next = i;
	}
	return i;
}
Value Assoc_get (Assoc self, Value key) {
	Assoc node = Assoc_has(self, key);
	return node ? node->value : NULL;
}
Value Assoc_set (Assoc self, Value key, Value value) {
	if (!(self && key)) return NULL;
	Assoc node = Assoc_has(self, key);
	Value result = NULL;
	if (node) {
		result = node->value;
		node->value = value;
	} else {
		Assoc_add(self, key, value);
	}
	return result;
}
Value Assoc_remove (Assoc self, Value key) {
	if (!(self && key)) return NULL;
	Assoc i = self->next, j = self;
	Value result = NULL;
	for (; i && i->key != key; i = (j = i)->next);
	if (i) {
		result = i->value;
		j->next = i->next;
		free(i);
	}
	return result;
}

/* the sizes of table
	53,         97,             193,        389,        769,  
    1543,       3079,           6151,       12289,      24593,  
    49157,      98317,          196613,     393241,     786443,  
    1572869,    3145739,        6291469,    12582917,   25165842,  
    50331553,   100663319,      201326611,  402653189,  805306457,  
    1610612741, 3221225473ul,   4294967291ul  
*/
// ================================================================================================
// Table
// ================================================================================================
#define __DEFAULT_TABLE_SIZE (53)
#define __indexOfKey(t, k) (cast_word(k) % (t)->size)
#define __assoc(t, i) ((t)->list[i])

void Table_free (Table self) {
	word i = 0;
	if (self) {
		for (i = 0; i < self->size; ++i) {
			Assoc_free(self->list[i]);
		}
		free(self->list);
		free(self);
	}
}
Table Table_new (word size) {
	Table node = struct_new(Table);
	node->size = size ? size : __DEFAULT_TABLE_SIZE;
	node->list = NULL;
	return node;
}
Assoc Table_has (Table self, Value key) {
	if (!(self && self->list && key)) return NULL;
	return Assoc_has(self->list[__indexOfKey(self, key)], key);
}
Value Table_get (Table self, Value key) {
	Assoc node = Table_has(self, key);
	return node ? node->value : NULL;
}
Value Table_set (Table self, Value key, Value value) {
	if (!(self && key)) return NULL;
	self->list || (self->list = (Assoc *)calloc(self->size, sizeof_word));
	word index = __indexOfKey(self, key);
	Assoc assoc = NULL;
	(assoc = self->list[index]) || (self->list[index] = (assoc = Assoc_new()));
	return Assoc_set(assoc, key, value);
}
Value Table_remove (Table self, Value key) {
	if (!(self && self->list && key)) return NULL;
	word index = __indexOfKey(self, key);
	return Assoc_remove(self->list[index], key);
}

// ================================================================================================
// Value
// ================================================================================================
#define STRING_TABLE_SIZE (1543)
#define NUMBER_TABLE_SIZE (1543)
#define VALUE_TABLE_SIZE (53)

static Table __stringTable = NULL;
static Table __numberTable = NULL;
static union { real number; word index; } __converter;

#define __r2p(r) ((__converter.number = (r)), __converter.index)

void Value_init () {
	if (__stringTable) return;

	__stringTable = Table_new(STRING_TABLE_SIZE);
	__numberTable = Table_new(NUMBER_TABLE_SIZE);
	__stringTable->list = (Assoc *)calloc(STRING_TABLE_SIZE, sizeof_word);
	__numberTable->list = (Assoc *)calloc(NUMBER_TABLE_SIZE, sizeof_word);
	debug_assert(__stringTable->list && __numberTable->list);
}
void Value_free (Value self) {
	if (self && --self->used == 0) {
		Table_free(self->table);
		Value_free(self->meta);
		free(self);
	}
}
Assoc Value_has (Value self, Value key) {
	if (!(self && key)) return NULL;
	Assoc assocNode = self->table ? Table_has(self->table, key) : NULL;
	return assocNode ? assocNode : Value_has(self->meta, key);
}
Assoc Value_hasOwn (Value self, Value key) {
	if (!(self && key)) return NULL;
	return Table_has(self->table, key);
}
Value Value_get (Value self, Value key) {
	if (!(self && key)) return NULL;
	Assoc assocNode = Value_has(self, key);
	Value result = assocNode ? assocNode->value : NULL;
	return result && Value_is(result, at_Accessor) ? Procedure_evalAccessor(self, result, result->u.value) : result;
}
static void _setMIndex (Value self, Value key) {
	Value minIndex = Value_get(self, at_TV_minIndex);
	Value maxIndex = Value_get(self, at_TV_maxIndex);
	(!minIndex || minIndex->u.number > key->u.number) && (Value_set(self, at_TV_minIndex, key));
	(!maxIndex || maxIndex->u.number < key->u.number) && (Value_set(self, at_TV_maxIndex, key));
}
Value Value_set (Value self, Value key, Value value) {
	if (!(self && key)) return NULL;
	Assoc assocNode = Value_has(self, key);
	Value me = NULL;
	//if (Value_is(key, at_Number)) _setMIndex(self, key);
	if (assocNode && (me = assocNode->value) && Value_is(me, at_Accessor)) {
		Procedure_evalAccessor(self, me, value);
	} else if (value == Value_get(self->meta, key)) {
		Table_remove(self->table, key);
	} else {
		self->table || (self->table = Table_new(VALUE_TABLE_SIZE));
		Table_set(self->table, key, value);
	}
	return me;
}
Value Value_remove (Value self, Value key) {
	return self && self->table && key ? Table_remove(self->table, key) : NULL;
}
static void _copyToAssoc (Value at, Value from, Value to, Assoc result, int flag) {
	if (!(from && result && from != to)) return;
	_copyToAssoc(at, from->meta, to, result, flag);
	if (!(from->table && from->table->list)) return;
	word i = 0;
	Assoc assoc = NULL;
	Value value = NULL;
	for (; i < from->table->size; ++i) {
		if (assoc = from->table->list[i]) {
			while (assoc = assoc->next) {
				if (flag && Value_is(assoc->key, at_String) || Value_is(assoc->key, at_Number)) {
					value = assoc->value;
					Assoc_set(result, assoc->key, Value_is(value, at_Accessor)
						? Procedure_evalAccessor(at, value, value->u.value)
						: value
					);
				}
			}
		}
	}
}
Assoc Value_pairs (Value self, int flag) {
	if (!self) return NULL;
	Assoc result = Assoc_new();
	_copyToAssoc(self, self, at_True, result, flag);
	return result;
}
Value Value_maxIndex (Value self) {
	if (!self) return NULL;
	real result = 0;
	int isFirst = 1;
	word i = 0;
	Assoc assoc = NULL;
	if (self->table && self->table->list) {
		for (; i < self->table->size; ++i) {
			if (assoc = self->table->list[i]) {
				while (assoc = assoc->next) {
					if (Value_is(assoc->key, at_Number) && (isFirst || result < assoc->key->u.number)) {
						result = assoc->key->u.number;
						isFirst = 0;
					}
				}
			}
		}
	}
	return isFirst ? NULL : Value_number(result);
}
Value Value_minIndex (Value self) {
	if (!self) return NULL;
	real result = 0;
	int isFirst = 1;
	word i = 0;
	Assoc assoc = NULL;
	if (self->table && self->table->list) {
		for (; i < self->table->size; ++i) {
			if (assoc = self->table->list[i]) {
				while (assoc = assoc->next) {
					if (Value_is(assoc->key, at_Number) && (isFirst || result > assoc->key->u.number)) {
						result = assoc->key->u.number;
						isFirst = 0;
					}
				}
			}
		}
	}
	return isFirst ? NULL : Value_number(result);
}
Value Value_is (Value a, Value b) {
	if (!(a && b)) return NULL;
	return a == b ? at_True : Value_is(a->meta, b);
}
Value Value_relationOf (Value a, Value b) {
	if (!(a && b)) return NULL;
	return Value_is(a, b) ? b : Value_relationOf(a, b->meta);
}
Value Value_extends (Value self, Value that) {
	if (!(self && that && !Value_is(self, that))) return;
	word i = 0;
	Assoc assoc = NULL;
	if (that->table && that->table->list) {
		for (; i < that->table->size; ++i) {
			if (assoc = that->table->list[i]) {
				while (assoc = assoc->next) {
					if (!Value_has(self, assoc->key)) {
						self->table || (self->table = Table_new(VALUE_TABLE_SIZE));
						Table_set(self->table, assoc->key, assoc->value);
					}
				}
			}
		}
	}
	Value_extends(self, that->meta);
	return self;
}
Value Value_new (Value meta, void *pointer) {
	Value node = struct_new(Value);
	node->meta = meta;
	node->u.pointer = pointer;
	node->table = NULL;
	return node;
}
Value Value_number (real value) {
	word index = __r2p(value) % NUMBER_TABLE_SIZE;
	Assoc head, i, j;
	(head = __numberTable->list[index]) || (__numberTable->list[index] = (head = Assoc_new()));
	for (j = head, i = j->next; i && i->value->u.number != value; i = (j = i)->next);
	if (!i) {
		i = struct_new(Assoc);
		i->key = NULL;
		i->value = Value_new(at_Number, NULL);
		i->value->u.number = value;
		i->next = head->next;
		head->next = i;
	} else {
		j->next = i->next;
		i->next = head->next;
		head->next = i;
	}
	return i->value;
}
Value Value_string (Chars value) {
	word index = String_hash(value) % STRING_TABLE_SIZE;
	Assoc head, i, j;
	if (!value) return NULL;
	if (!(head = __stringTable->list[index])) {
		head = Assoc_new();
		__stringTable->list[index] = head;
	}
	for (j = head, i = j->next; i && strcmp(i->value->u.string, value); i = (j = i)->next);
	if (!i) {
		i = struct_new(Assoc);
		i->key = NULL;
		i->value = Value_new(at_String, String_getCopy(value));
		i->next = head->next;
		head->next = i;
	} else {
		j->next = i->next;
		i->next = head->next;
		head->next = i;
	}
	return i->value;
}
Value Value_toNumberValue (Value self) {
	if (!self) return NULL;
	Value toNumber = Value_get(self, Value_string("toNumber"));
	return Procedure_evalFunction(at_currentInterpreter->proc, self, toNumber, NULL);
}
Value Value_toStringValue (Value self) {
	if (!self) return NULL;
	Value toString = Value_get(self, Value_string("toString"));
	return Procedure_evalFunction(at_currentInterpreter->proc, self, toString, NULL);
}



