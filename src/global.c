#include "_.h"

Value at_True = NULL;
Value at_Number = NULL;
Value at_String = NULL;
Value at_Function = NULL;
Value at_Func = NULL;
Value at_Accessor = NULL;
Value at_Array = NULL;

Value at_TV_minIndex = NULL;
Value at_TV_maxIndex = NULL;
Value at_TV_startIndex = NULL;
Value at_TV_endIndex = NULL;

extern void at_True_init ();
extern void at_Number_init ();
extern void at_String_init ();
extern void at_Function_init ();
extern void at_Array_init ();

void AT_init () {
	at_True = Value_new(NULL, NULL);
	at_Number = Value_new(at_True, NULL);
	at_String = Value_new(at_True, NULL);
	at_Function = Value_new(at_True, NULL);
	at_Func = Value_new(at_Function, NULL);
	at_Accessor = Value_new(at_True, NULL);
	at_Array = Value_new(at_Func, NULL);

	at_TV_minIndex = Value_new(at_True, NULL);
	at_TV_maxIndex = Value_new(at_True, NULL);
	at_TV_startIndex = Value_new(at_True, NULL);
	at_TV_endIndex = Value_new(at_True, NULL);
	
	at_True_init();
	at_Number_init();
	at_String_init();
	at_Function_init();
	at_Array_init();
}

// ================================================================================================
// Array
// ================================================================================================

static void _getRange (Value self, int *startIndex, int *endIndex, int *step) {
	Value start = Value_get(self, at_TV_startIndex);
	Value end = Value_get(self, at_TV_endIndex);
	*startIndex = start ? start->u.number : 0;
	*endIndex = end ? end->u.number : 0;
	*step = *endIndex < *startIndex ? -1 : 1;
}
static Value at_Array_startIndex (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	if (arg && arg->u.value) {
		if (arg->u.value->u.func == at_Array_startIndex) {
			return Value_get(at, at_TV_startIndex);
		}
	}
	return NULL;
}
static Value at_Array_endIndex (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	if (arg && arg->u.value) {
		if (arg->u.value->u.func == at_Array_endIndex) {
			return Value_get(at, at_TV_endIndex);
		}
	}
	return NULL;
}
static Value at_Array_push (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	int startIndex, endIndex, step;
	_getRange(at, &startIndex, &endIndex, &step);
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		Value_set(at, Value_number(endIndex), Procedure_eval(proc, argE));
		endIndex += step;
	}
	Value_set(at, at_TV_endIndex, Value_number(endIndex));
	return at;
}
static Value at_Array_pop (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Value result = NULL, endValue = NULL;
	int startIndex, endIndex, step;
	_getRange(at, &startIndex, &endIndex, &step);
	if (startIndex == startIndex) return NULL;
	endValue = Value_number(endIndex - step);
	result = Value_remove(at, endValue);
	Value_set(at, at_TV_endIndex, endValue);
	return result;
}
static Value at_Array_unshift (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	Value startValue = NULL;
	int startIndex, endIndex, step;
	_getRange(at, &startIndex, &endIndex, &step);
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		startIndex -= step;
		startValue = Value_number(startIndex);
		Value_set(at, startValue, Procedure_eval(proc, argE));
	}
	Value_set(at, at_TV_startIndex, startValue);
	return at;
}
static Value at_Array_shift (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Value result = NULL;
	int startIndex, endIndex, step;
	_getRange(at, &startIndex, &endIndex, &step);
	if (startIndex == startIndex) return NULL;
	result = Value_remove(at, Value_number(startIndex));
	Value_set(at, at_TV_startIndex, Value_number(startIndex + step));
	return result;
}
static Value _join (Value at, Value sperator) {
	if (!at) return NULL;
	int startIndex, endIndex, step;
	_getRange(at, &startIndex, &endIndex, &step);

	Chars sum = NULL, temp = NULL;
	Value value = NULL;
	sperator = Value_toStringValue(sperator);
	if (!sperator) sperator = Value_string(",");
	for (; startIndex != endIndex; startIndex += step) {
		value = Value_toStringValue(Value_get(at, Value_number(startIndex)));
		if (sum) {
			temp = sum;
			sum = String_format("%s%s%s", sum, sperator->u.string, value->u.string);
			free(temp);
		} else {
			sum = String_getCopy(value->u.string);
		}
	}
	value = Value_string(sum);
	free(sum);
	return value;
}
static Value at_Array_join (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
	}
	return _join(at, argV);
}
static Value at_Array_new (Procedure proc, Value at, Exp arg) {
	at = Value_new(at_Array, NULL);
	return at_Array_push(proc, at, arg);
}
void at_Array_init () {
	at_Array->u.pointer = at_Array_new;
	Value_set(at_Array, at_TV_startIndex, Value_number(0));
	Value_set(at_Array, at_TV_endIndex, Value_number(0));
	Value_set(at_Array, Value_string("startIndex"), Value_funcAccessor(at_Array_startIndex));
	Value_set(at_Array, Value_string("endIndex"), Value_funcAccessor(at_Array_endIndex));
	Value_set(at_Array, Value_string("push"), Value_func(at_Array_push));
	Value_set(at_Array, Value_string("pop"), Value_func(at_Array_pop));
	Value_set(at_Array, Value_string("unshift"), Value_func(at_Array_unshift));
	Value_set(at_Array, Value_string("shift"), Value_func(at_Array_shift));
	Value_set(at_Array, Value_string("join"), Value_func(at_Array_join));
}
/*
Value Array_get (Value self, Value key) {
	if (!(self && self->table && Value_is(self, at_Array))) return NULL;
	Value result = NULL;
	int start = self->u.array->start;
	int end = self->u.array->end;
	int index = 0;
	if (Value_is(key, at_Number)) {
		index = key->u.number + start - 1;
		if (index < start || index >= end) return NULL;
		result = Table_get(self->table, Value_number(index));
		return result ? result : Value_get(self->meta, key);
	} else if (key == Value_string("length")) {
		return Value_number(end - start);
	}
	result = Table_get(self->table, key);
	return result ? result : Value_get(self->meta, key);
}
Value Array_set (Value self, Value key, Value value) {
	if (!(self && self->table && Value_is(self, at_Array))) return NULL;
	int start = self->u.array->start;
	int end = self->u.array->end;
	int index = 0;
	Value result = NULL;
	if (Value_is(key, at_Number)) {
		index = key->u.number + start - 1;
		if (index >= end) self->u.array->end = index + 1;
		if (index < start) self->u.array->start = index;
		return Table_set(self->table, Value_number(index), value);
	} else if (key == Value_string("length") && Value_is(value, at_Number) && value->u.number >= 0) {
		if (value->u.number == end - start) return value;
		if (value->u.number < end - start) {
			for (index = value->u.number + start; index < end; ++index) {
				Table_set(self->table, Value_number(index), NULL);
			}
		}
		self->u.array->end = value->u.number + start;
		return Value_number(end - start);
	}
	return result;
}
*/

// ================================================================================================
// Function
// ================================================================================================
static Value at_Function_call (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Function)) return NULL;
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		return Procedure_evalFunction(proc, argV, at, arg);
	}
	return NULL;
}
void at_Function_init () {
	Value_set(at_Function, Value_string("call"), Value_func(at_Function_call));
}

// ================================================================================================
// Number
// ================================================================================================
// ================================================================================================
// Number
// ================================================================================================
#define NUMBER_FUNC_ACCESSOR(FUNC_NAME, CALLED_FUNC_NAME) \
	static Value at_Number_##FUNC_NAME (Procedure proc, Value at, Exp arg) {\
		if (!Value_is(at, at_Number)) return NULL;\
		if (arg && arg->u.value && arg->u.value->u.func == at_Number_##FUNC_NAME) {\
			return Value_number(CALLED_FUNC_NAME(at->u.number));\
		}\
		return NULL;\
	}
NUMBER_FUNC_ACCESSOR(abs, fabs)
NUMBER_FUNC_ACCESSOR(ceil, ceil)
NUMBER_FUNC_ACCESSOR(floor, floor)
NUMBER_FUNC_ACCESSOR(round, round)
NUMBER_FUNC_ACCESSOR(sign, math_sign)
NUMBER_FUNC_ACCESSOR(sqrt, sqrt)

static Value at_Number_sqr (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Number)) return NULL;
	if (arg && arg->u.value && arg->u.value->u.func == at_Number_sqr) {
		return Value_number(at->u.number * at->u.number);
	}
	return NULL;
}
static Value at_Number_toString (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Number)) return NULL;
	Chars str = r2s(at->u.number);
	Value result = Value_string(str);
	free(str);
	return result;
}
static Value at_Number_toNumber (Procedure proc, Value at, Exp arg) {
	return Value_is(at, at_Number) ? at : NULL;
}
void at_Number_init () {
	Value_set(at_Number, Value_string("abs"), Value_funcAccessor(at_Number_abs));
	Value_set(at_Number, Value_string("ceil"), Value_funcAccessor(at_Number_ceil));
	Value_set(at_Number, Value_string("floor"), Value_funcAccessor(at_Number_floor));
	Value_set(at_Number, Value_string("round"), Value_funcAccessor(at_Number_round));
	Value_set(at_Number, Value_string("sign"), Value_funcAccessor(at_Number_sign));
	Value_set(at_Number, Value_string("sqrt"), Value_funcAccessor(at_Number_sqrt));
	Value_set(at_Number, Value_string("sqr"), Value_funcAccessor(at_Number_sqr));
	Value_set(at_Number, Value_string("toString"), Value_func(at_Number_toString));
	Value_set(at_Number, Value_string("toNumber"), Value_func(at_Number_toNumber));
}

// ================================================================================================
// String
// ================================================================================================
extern Value AT_VALUES;
#define STRING_ALLOC_SIZE (256)

static Chars __stringBuffer = NULL;
static word __stringBufferSize = 0;
static word __stringBufferAllocSize = 0;

void StringBuffer_add (int letter) {
	if (__stringBufferSize == __stringBufferAllocSize) {
		__stringBufferAllocSize += STRING_ALLOC_SIZE;
		__stringBuffer = (Chars)realloc(__stringBuffer, __stringBufferAllocSize);
	}
	__stringBuffer[__stringBufferSize++] = letter;
}
void StringBuffer_cat (Chars str) {
	if (!str) return;
	int size = __stringBufferSize + strlen(str) + 1;
	if (size > __stringBufferAllocSize) {
		__stringBufferAllocSize = (size + STRING_ALLOC_SIZE - 1) / STRING_ALLOC_SIZE * STRING_ALLOC_SIZE;
		__stringBuffer = (Chars)realloc(__stringBuffer, __stringBufferAllocSize);
	}
	int i = 0;
	for (; i < strlen(str); ++i) {
		__stringBuffer[__stringBufferSize++] = str[i];
	}
}
Value StringBuffer_close (void) {
	StringBuffer_add('\0');
	Value result = Value_string(__stringBuffer);
	free(__stringBuffer);
	__stringBuffer = NULL;
	__stringBufferSize = 0;
	__stringBufferAllocSize = 0;
	return result;
}
Chars r2s (real value) {
	char buff[128];
	Chars cur, end;
	sprintf(buff, "%.6f", value);
	for (cur = buff + strlen(buff) - 1, end = cur - 7;
		(cur > end) && (*cur == '0' || *cur == '.');
		*(cur--) = '\0'
	);
	return String_getCopy(buff);
}
word String_hash (Chars key) {
	register word hash = 0;
	while (*key) {
		hash = hash * 31 + (*key++);
	}
	return hash;
}
Chars String_getCopy (Chars from) {
	Chars to = (Chars)malloc(sizeof(char) * (strlen(from) + 1));
	strcpy(to, from);
	return to;
}
Chars String_cat (Chars a, Chars b) {
	Chars result = (Chars)malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
	strcpy(result, a);
	strcat(result, b);
	return result;
}
Chars String_format (Chars format, ...) {
	char buff[256];
	va_list arg;
	va_start(arg, format);
	vsprintf(buff, format, arg);
	va_end(arg);
	return String_getCopy(buff);
}
Chars String_charAt (Chars self, int index) {
	char buffer[2];
	int num = index - 1;
	if (!self) return NULL;
	if (num >= 0 && num < strlen(self)) {
		buffer[0] = self[num];
		buffer[1] = '\0';
		return String_getCopy(buffer);
	}
	return NULL;
}
Chars String_upperCase (Chars from) {
	if (!from) return NULL;
	Chars result = String_getCopy(from);
	Chars i = result;
	for (; *i != '\0'; ++i) {
		if (*i >= 'a' && *i <= 'z') {
			*i -= 32;
		}
	}
	return result;
}
Chars String_lowerCase (Chars from) {
	if (!from) return NULL;
	Chars result = String_getCopy(from);
	Chars i = result;
	for (; *i != '\0'; ++i) {
		if (*i >= 'A' && *i <= 'Z') {
			*i += 32;
		}
	}
	return result;
}
Chars String_getDir (Chars filePath) {
	char dir[256];
	strcpy(dir, filePath);
	int i = strlen(dir);
	for (; i > 0; --i) {
		if (dir[i] == '\\' || dir[i] == '/') {
			dir[i] = '\0';
			break;
		}
	}
	return String_getCopy(dir);
}
Chars String_getAbsolutePath (Chars currentDir, Chars filePath) {
	if (!(currentDir && filePath)) return NULL;
	if (strchr(filePath, ':')) {
		return filePath;
	}
	Chars dir, file;
	Chars result;
	if (filePath[0] == '.') {
		if (filePath[1] == '.' && (filePath[2] == '/' || filePath[2] == '\\')) {
			dir = String_getDir(currentDir);
			file = &filePath[3];
		} else if (filePath[1] == '/' || filePath[1] == '\\')  {
			dir = currentDir;
			file = &filePath[2];
		}
	} else {
		dir = currentDir;
		file = filePath;
	}
	result = String_format("%s\\%s", dir, file);
	if (dir != currentDir) free(dir);
	return result;
}

static Value at_String_length (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_String)) return NULL;
	if (arg && arg->u.value && arg->u.value->u.func == at_String_length) {
		return Value_number(strlen(at->u.string));
	}
	return NULL;
}
static Value at_String_charAt (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_String)) return NULL;
	Exp argE = NULL;
	Value argV = NULL;
	char buffer[2];
	int index = 0;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (Value_is(argV, at_Number)) {
			index = argV->u.number;
			if (index >= 0 && index < strlen(at->u.string)) {
				buffer[0] = at->u.string[index];
				buffer[1] = '\0';
				return Value_string(buffer);
			}
		}
	}
	return NULL;
}
static Value at_String_compare (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_String)) return NULL;
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (argV && Value_is(argV, at_String)) {
			return Value_number(strcmp(at->u.string, argV->u.string));
		}
	}
	return NULL;
}
static Value at_String_toString (Procedure proc, Value at, Exp arg) {
	return Value_is(at, at_String) ? at : NULL;
}
static Value at_String_toNumber (Procedure proc, Value at, Exp arg) {
	return Value_is(at, at_String) ? Value_number(atof(at->u.string)) : NULL;
}
void at_String_init () {
	Value_set(at_String, Value_string("length"), Value_funcAccessor(at_String_length));
	Value_set(at_String, Value_string("charAt"), Value_func(at_String_charAt));
	Value_set(at_String, Value_string("compare"), Value_func(at_String_compare));
	Value_set(at_String, Value_string("toString"), Value_func(at_String_toString));
	Value_set(at_String, Value_string("toNumber"), Value_func(at_String_toNumber));
}

// ================================================================================================
// True
// ================================================================================================
static Value at_True_memoryAddress (Procedure proc, Value at, Exp arg) {
	if (at && arg && arg->u.value && arg->u.value->u.func == at_True_memoryAddress) {
		return Value_number((word)at);
	}
	return NULL;
}
static Value at_True_maxIndex (Procedure proc, Value at, Exp arg) {
	if (at && arg && arg->u.value && arg->u.value->u.func == at_True_maxIndex) {
		return Value_maxIndex(at);
	}
	return NULL;
}
static Value at_True_minIndex (Procedure proc, Value at, Exp arg) {
	if (at && arg && arg->u.value && arg->u.value->u.func == at_True_minIndex) {
		return Value_minIndex(at);
	}
	return NULL;
}
static Value at_True_meta (Procedure proc, Value at, Exp arg) {
	if (at && arg && arg->u.value && arg->u.value->u.func == at_True_meta) {
		return at->meta;
	}
	return NULL;
}
static Value at_True_is (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		return Value_is(at, Procedure_eval(proc, argE));
	}
	return NULL;
}
static Value at_True_relationOf (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		return Value_relationOf(at, Procedure_eval(proc, argE));
	}
	return NULL;
}
static Value at_True_hasProperty (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		return Value_has(at, argV) ? at_True : NULL;
	}
	return NULL;
}
static Value at_True_hasOwnProperty (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		return Value_hasOwn(at, argV) ? at_True : NULL;
	}
	return NULL;
}
static Value at_True_toString (Procedure proc, Value at, Exp arg) {
	return at ? Value_string("True") : NULL;
}
static Value at_True_extends (Procedure proc, Value at, Exp arg) {
	if (!at) return NULL;
	Exp argE = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		Value_extends(at, Procedure_eval(proc, argE));
	}
	return at;
}
static Value at_True_new (Procedure proc, Value at, Exp arg) {
	return at ? Value_new(at, NULL) : NULL;
}
void at_True_init () {
	Value_set(at_True, Value_string("memoryAddress"), Value_funcAccessor(at_True_memoryAddress));
	Value_set(at_True, Value_string("maxIndex"), Value_funcAccessor(at_True_maxIndex));
	Value_set(at_True, Value_string("minIndex"), Value_funcAccessor(at_True_minIndex));
	Value_set(at_True, Value_string("meta"), Value_funcAccessor(at_True_meta));
	Value_set(at_True, Value_string("is"), Value_func(at_True_is));
	Value_set(at_True, Value_string("relationOf"), Value_func(at_True_relationOf));
	Value_set(at_True, Value_string("hasProperty"), Value_func(at_True_hasProperty));
	Value_set(at_True, Value_string("hasOwnProperty"), Value_func(at_True_hasOwnProperty));
	Value_set(at_True, Value_string("toString"), Value_func(at_True_toString));
	Value_set(at_True, Value_string("extends"), Value_func(at_True_extends));
	Value_set(at_True, Value_string("new"), Value_func(at_True_new));
}




