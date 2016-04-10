#include "_.h"

extern Value AT_VALUES;

#define LIB_COUNT 4
static Value at_Math_init ();
static Value at_Console_init ();
static Value at_IO_init ();
static Value at_Date_init ();

static struct { Chars name; InitFunc func; } 
__lib[LIB_COUNT] = {
	{"Math", at_Math_init},
	{"IO", at_IO_init},
	{"Console", at_Console_init},
	{"Date", at_Date_init}
};
InitFunc LIB_getInitFunc (Chars name) {
	int i = 0;
	for (; i < LIB_COUNT; ++i) {
		if (!strcmp(name, __lib[i].name)) {
			return __lib[i].func;
		}
	}
	return NULL;
}


// ================================================================================================
// Math
// ================================================================================================

#define MATH_FUNC_1(FUNC_NAME, CALLED_FUNC_NAME) \
	static Value at_Math_##FUNC_NAME (Procedure proc, Value at, Exp arg) {\
		Exp argE = NULL;\
		Value argV = NULL;\
		Value result = NULL;\
		if (arg) {\
			TRAVERSEL_COMMA_EXP(arg, argE);\
			argV = Procedure_eval(proc, argE);\
			if (argV && Value_is(argV, at_Number)) {\
				result = Value_number(CALLED_FUNC_NAME(argV->u.number));\
			}\
		}\
		return result;\
	}
#define MATH_FUNC_2(FUNC_NAME, CALLED_FUNC_NAME) \
	static Value at_Math_##FUNC_NAME (Procedure proc, Value at, Exp arg) {\
		Exp argE = NULL;\
		Value argV1 = NULL, argV2 = NULL;\
		Value result = NULL;\
		if (arg) {\
			TRAVERSEL_COMMA_EXP(arg, argE);\
			argV1 = Procedure_eval(proc, argE);\
			if (arg) {\
				TRAVERSEL_COMMA_EXP(arg, argE);\
				argV2 = Procedure_eval(proc, argE);\
				if (argV1 && argV2 && Value_is(argV1, at_Number) && Value_is(argV2, at_Number)) {\
					result = Value_number(CALLED_FUNC_NAME(argV1->u.number, argV2->u.number));\
				}\
			}\
		}\
		return result;\
	}
#define VALUE_SET(FUNC_NAME) Value_set(self, Value_string(#FUNC_NAME), Value_func(at_Math_##FUNC_NAME))

MATH_FUNC_1(sin, sin)
MATH_FUNC_1(cos, cos)
MATH_FUNC_1(tan, tan)
MATH_FUNC_1(asin, asin)
MATH_FUNC_1(acos, acos)
MATH_FUNC_1(atan, atan)
MATH_FUNC_1(sinh, sinh)
MATH_FUNC_1(cosh, cosh)
MATH_FUNC_1(tanh, tanh)

MATH_FUNC_1(log, log)
MATH_FUNC_1(log10, log10)
MATH_FUNC_1(exp, exp)
MATH_FUNC_1(sqrt, sqrt)
MATH_FUNC_1(ceil, ceil)
MATH_FUNC_1(floor, floor)
MATH_FUNC_1(round, round)
MATH_FUNC_1(sign, math_sign)

MATH_FUNC_2(atan2, atan2)
MATH_FUNC_2(pow, pow)
MATH_FUNC_2(mod, fmod)
MATH_FUNC_2(hypot, hypot)

static Value at_Math_max (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	Value result = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (!result) {
			result = argV;
		} else if (argV && Value_is(result, at_Number) && Value_is(argV, at_Number)) {
			if (result->u.number < argV->u.number) {
				result = argV;
			}
		}
	}
	return result;
}
static Value at_Math_min (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	Value result = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (!result) {
			result = argV;
		} else if (argV && Value_is(result, at_Number) && Value_is(argV, at_Number)) {
			if (result->u.number > argV->u.number) {
				result = argV;
			}
		}
	}
	return result;
}
static Value at_Math_init () {
	Value self = Value_new(at_True, NULL);
	VALUE_SET(sin); VALUE_SET(cos); VALUE_SET(tan);
	VALUE_SET(asin); VALUE_SET(acos); VALUE_SET(atan);
	VALUE_SET(sinh); VALUE_SET(cosh); VALUE_SET(tanh);
	VALUE_SET(log); VALUE_SET(log10);
	VALUE_SET(exp); VALUE_SET(sqrt);
	VALUE_SET(ceil); VALUE_SET(floor); VALUE_SET(round); VALUE_SET(sign);
	VALUE_SET(atan2);
	VALUE_SET(pow); VALUE_SET(mod);
	VALUE_SET(hypot);
	VALUE_SET(max); VALUE_SET(min);
	Value_set(self, Value_string("random"), Value_number((double)rand() / RAND_MAX));
	Value_set(self, Value_string("pi"), Value_number(M_PI));
	Value_set(self, Value_string("e"), Value_number(M_E));
	Value_set(self, Value_string("log2e"), Value_number(M_LOG2E));
	Value_set(self, Value_string("log10e"), Value_number(M_LOG10E));
	Value_set(self, Value_string("sqrt2"), Value_number(M_SQRT2));
	Value_set(self, Value_string("ln2"), Value_number(M_LN2));
	Value_set(self, Value_string("ln10"), Value_number(M_LN10));
	return self;
}



// ================================================================================================
// Console
// ================================================================================================
static Value at_Console_log (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	int isFirst = 1;
	while (arg) {
		isFirst || printf(", "), isFirst = 0;
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (argV) {
			if (Value_is(argV, at_Number)) 			printf(r2s(argV->u.number));
			else if (Value_is(argV, at_String)) 	printf(argV->u.string);
			else if (Value_is(argV, at_Array)) 		printf("Array");
			else if (Value_is(argV, at_Function))	printf("Function");
			else if (Value_is(argV, at_Accessor)) 	printf("Accessor");
			else if (Value_is(argV, at_Func))  		printf("Func");
			else if (Value_is(argV, at_True)) 		printf("True");
			else 									printf("unknown value type");
		} else {
			printf("<null>");
		}
	}
	printf("\n");
	return NULL;
}
static Value at_Console_assert (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		argV = Procedure_eval(proc, argE);
		if (!argV) {
			at_Console_log(proc, at, arg);
		}
	}
	return NULL;
}
static Value at_Console_timerStart (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Console_timerEnd (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Console_new (Procedure proc, Value at, Exp arg) {
	Exp argE = NULL;
	Value argV = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		if (argV = Value_toStringValue(Procedure_eval(proc, argE))) {
			system(argV->u.string);
		}
	}
	return NULL;
}
static Value at_Console_init () {
	Value self = Value_new(at_Func, at_Console_new);
	//printf("Console_init\n");
	Value_set(self, Value_string("log"), Value_func(at_Console_log));
	Value_set(self, Value_string("assert"), Value_func(at_Console_assert));
	Value_set(self, Value_string("timerStart"), Value_func(at_Console_timerStart));
	Value_set(self, Value_string("timerEnd"), Value_func(at_Console_timerEnd));
	return self;
}
// ================================================================================================
// Date
// ================================================================================================

static Value at_Date = NULL;

static Value at_Date_date (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Date)) return NULL;
	return NULL;
}
static Value at_Date_day (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Date)) return NULL;
	struct tm *details;
	time_t timestamp;
	Value result = NULL;
	if (arg && arg->u.value) {
		timestamp = (at == at_Date ? time(NULL) : ((struct timeval *)at->u.pointer)->tv_sec);
		details = localtime(&timestamp);
		if (arg->u.value->u.func == at_Date_day) {
			result = Value_number(details->tm_mday);
		}
		free(details);
	}
	return result;
}
static Value at_Date_month (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_year (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_hours (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Date)) return NULL;
	struct tm *details;
	time_t timestamp;
	Value result = NULL;
	if (arg && arg->u.value) {
		timestamp = (at == at_Date ? time(NULL) : ((struct timeval *)at->u.pointer)->tv_sec);
		details = localtime(&timestamp);
		if (arg->u.value->u.func == at_Date_hours) {
			result = Value_number(details->tm_hour);
		}
		free(details);
	}
	return result;
}
static Value at_Date_minutes (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_seconds (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_miniseconds (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_Date)) return NULL;
	struct timeval *tv_ptr = NULL;
	struct timeval tv;
	if (arg && arg->u.value && arg->u.value->u.func == at_Date_miniseconds) {
		if (at == at_Date) {
			gettimeofday(&tv, NULL);
			return Value_number(tv.tv_usec / 1000.0);
		}
		tv_ptr = (struct timeval *)at->u.pointer;
		return Value_number(tv_ptr->tv_usec / 1000.0);
	}
	return NULL;
}
static Value at_Date_time (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_toString (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_Date_new (Procedure proc, Value at, Exp arg) {
	Value result = NULL;
	struct timeval *tv = (struct timeval *)malloc(sizeof(struct timeval));
	gettimeofday(tv, NULL);
	result = Value_new(at_Date, tv);
	return result; 
}

static Value at_Date_init () {
	Value self = Value_new(at_Func, at_Date_new);
	//printf("Console_init\n");
	Value_set(self, Value_string("date"), Value_funcAccessor(at_Date_date));
	Value_set(self, Value_string("day"), Value_funcAccessor(at_Date_day));
	Value_set(self, Value_string("month"), Value_funcAccessor(at_Date_month));
	Value_set(self, Value_string("year"), Value_funcAccessor(at_Date_year));
	Value_set(self, Value_string("hours"), Value_funcAccessor(at_Date_hours));
	Value_set(self, Value_string("minutes"), Value_funcAccessor(at_Date_minutes));
	Value_set(self, Value_string("seconds"), Value_funcAccessor(at_Date_seconds));
	Value_set(self, Value_string("miniseconds"), Value_funcAccessor(at_Date_miniseconds));
	Value_set(self, Value_string("time"), Value_funcAccessor(at_Date_time));
	Value_set(self, Value_string("toString"), Value_func(at_Date_toString));
	at_Date = self;
	return self;
}



// ================================================================================================
// IO
// ================================================================================================
extern Interpreter at_currentInterpreter;
static Value at_IO = NULL;

static Value at_IO_read (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_IO)) return NULL;
	File stream = (at == at_IO ? stdin : (File)at->u.pointer);
	char buffer[256];
	fscanf(stream, "%s", buffer);
	return Value_string(buffer);
}
static Value at_IO_readLine (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_IO)) return NULL;
	File stream = (at == at_IO ? stdin : (File)at->u.pointer);
	int letter = fgetc((stream));
	Value result = NULL;
	for (; letter != EOF && letter != '\n'; letter = fgetc(stream)) {
		StringBuffer_add(letter);
	}
	result = StringBuffer_close();
	return result;
}
static Value at_IO_write (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_IO)) return NULL;
	File stream = (at == at_IO ? stdout : (File)at->u.pointer);
	Exp argE = NULL;
	Value argV = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		if (argV = Value_toStringValue(Procedure_eval(proc, argE))) {
			fprintf(stream, argV->u.string);
		}
	}
	return NULL;
}
static Value at_IO_writeLine (Procedure proc, Value at, Exp arg) {
	if (!Value_is(at, at_IO)) return NULL;
	File stream = (at == at_IO ? stdout : (File)at->u.pointer);
	Exp argE = NULL;
	Value argV = NULL;
	while (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		if (argV = Value_toStringValue(Procedure_eval(proc, argE))) {
			fprintf(stream, "%s\n", argV->u.string);
		}
	}
	return NULL;
}
static Value at_IO_new (Procedure proc, Value at, Exp arg) {
	File stream = NULL;
	Exp argE = NULL;
	Value path = NULL, mode = NULL;
	Value result = NULL;
	if (arg) {
		TRAVERSEL_COMMA_EXP(arg, argE);
		path = Value_toStringValue(Procedure_eval(proc, argE));
		if (arg) {
			TRAVERSEL_COMMA_EXP(arg, argE);
			mode = Value_toStringValue(Procedure_eval(proc, argE));
		}
		path = Interpreter_getFilePath(at_currentInterpreter, path);
		if (!mode) mode = Value_string("r+");
		stream = fopen(path->u.string, mode->u.string);
		if (!stream) {
			printf("file can't open. (%s)\n", path->u.string);
			return NULL;
		}
		result = Value_new(at_IO, stream);
	}
	return result;
}
static Value at_IO_file_open (Procedure proc, Value at, Exp arg) {
	return NULL;
}
static Value at_IO_file_close (Procedure proc, Value at, Exp arg) {
	return NULL;
}

static Value at_IO_init () {
	Value self = Value_new(at_Func, at_IO_new);
	Value_set(self, Value_string("read"), Value_func(at_IO_read));
	Value_set(self, Value_string("readLine"), Value_func(at_IO_readLine));
	Value_set(self, Value_string("write"), Value_func(at_IO_write));
	Value_set(self, Value_string("writeLine"), Value_func(at_IO_writeLine));
	at_IO = self;
	return self;
}



