#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

// ================================================================================================
// Assoc
// ================================================================================================
struct Assoc {
	Assoc next;
	Value key;
	Value value;
};
void Assoc_free (Assoc self);
Assoc Assoc_new ();
Assoc Assoc_add (Assoc self, Value key, Value value);
Assoc Assoc_has (Assoc self, Value key);
Value Assoc_get (Assoc self, Value key);
Value Assoc_set (Assoc self, Value key, Value value);
Value Assoc_remove (Assoc self, Value key);
// ================================================================================================
// Table
// ================================================================================================
struct Table {
	word size;
	Assoc *list;
};
void Table_free (Table self);
Table Table_new (word size);
Assoc Table_has (Table self, Value key);
Value Table_get (Table self, Value key);
Value Table_set (Table self, Value key, Value value);
Value Table_remove (Table self, Value key);
// ================================================================================================
// Value
// ================================================================================================
struct Value {
	union {
		real number;
		Chars string;
		Func func;
		Procedure proc;
		Value value;
		void *pointer;
	} u;
	unsigned int used;
	Table table; // 哈希表
	Value meta;
};
void Value_init ();
void Value_free (Value self);
Assoc Value_has (Value self, Value key);
Assoc Value_hasOwn (Value self, Value key);
Assoc Value_pairs (Value self, int flag);
Value Value_get (Value self, Value key);
Value Value_set (Value self, Value key, Value value);
Value Value_remove (Value self, Value key);
Value Value_maxIndex (Value self);
Value Value_minIndex (Value self);
Value Value_is (Value a, Value b);
Value Value_relationOf (Value a, Value b);
Value Value_extends (Value self, Value that);
Value Value_new (Value meta, void *pointer);
Value Value_number (real value);
Value Value_string (Chars value);
Value Value_toNumberValue (Value self);
Value Value_toStringValue (Value self);
#define Value_func(func) (Value_new(at_Func, func))
#define Value_function(proc, exp) (Value_new(at_Function, Procedure_new(proc, NULL, NULL, NULL, exp)))
#define Value_funcAccessor(func) (Value_new(at_Accessor, Value_new(at_Func, func)))
#define Value_functionAccessor(proc, exp) (Value_new(at_Accessor, \
	Value_new(at_Function, Procedure_new(proc, NULL, NULL, NULL, exp))))

#endif /* MEMORY_H_INCLUDED */


