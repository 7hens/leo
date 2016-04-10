#ifndef _H_INCLUDED
#define _H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <direct.h>
#include <sys/time.h>

#define DEBUG
#define USE_WINDOWS

#if defined(USE_WINDOWS)
#define DIR_SEPERATOR '\\'
#else
#define DIR_SEPERATOR '/'
#endif

#if LONG_MAX >> 31 > 0
#define WORD_BYTES 8 /* 64-bit */
#elif INT_MAX >> 15 > 0
#define WORD_BYTES 4 /* 32-bit */
#else
#define WORD_BYTES 2 /* 16-bit */
#endif

#if WORD_BYTES == 8
typedef signed int int32;
typedef unsigned int uint32;
#define REAL_BYTES 8
#elif WORD_BYTES == 4
typedef signed int int32;
typedef unsigned int uint32;
#define REAL_BYTES 8
#else
typedef signed long int32;
typedef unsigned long uint32;
#define REAL_BYTES 4
#endif /* WORD_BYTES */

typedef size_t word;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef unsigned char boolean;
typedef void *Pointer;
typedef char *Chars;
typedef FILE *File;

#if REAL_BYTES == 8
typedef double real;
#else
typedef float real;
#endif /* REAL_BYTES */

#define bitsof(t) (sizeof(t)<<3)
#define sizeof_type(t) (sizeof(struct t))
#define sizeof_word (sizeof(word))
#define cast_type(t, x) ((t)(x))
#define cast_word(x) ((word)(x))
#define cast_int32(x) ((int32)(x))
#define cast_uint32(x) ((uint32)(x))
#define cast_int16(x) ((int16)(x))
#define cast_uint16(x) ((uint16)(x))
#define cast_int8(x) ((int8)(x))
#define cast_uint8(x) ((uint8)(x))
#define cast_byte(x) ((byte)(x))
#define cast_boolean(x) ((boolean)(!!(x)))
#define cast_void(x) ((void)(x))
#define cast_Chars(x) ((Chars)(x))
#define cast_Pointer(x) ((Pointer)(x))
#define struct_new(t) ((t)calloc(1, sizeof_type(t)))
#define struct_alloc(t) ((t)malloc(sizeof_type(t)))
#define array_size(a) (sizeof(a)/sizeof(*(a)))
#define math_max(x, y) ((x) > (y) ? (x) : (y))
#define math_min(x, y) ((x) < (y) ? (x) : (y))
#define math_swap(x, y) ((x) ^= (y) ^= (x) ^= (y))
#define math_abs(x) ((x) > 0 ? (x) : -(x))
#define math_sign(x) ((x) > 0 ? 1 : ((x) ? -1 : 0))
#define math_r2i(r, i) ((i = (r)) == (r))
#define math_pow2(n) (1 << (n))



typedef enum ExpType ExpType;
typedef struct Exp *Exp;
typedef struct BinaryExp *BinaryExp;
typedef struct TernaryExp *TernaryExp;
typedef struct String *String;
typedef struct StringList *StringList;
typedef struct ObjectTable *ObjectTable;
typedef struct NumberTable *NumberTable;
typedef struct Object *Object;
typedef struct Value *Value;
typedef struct Assoc *Assoc;
typedef struct Table *Table;
typedef struct Procedure *Procedure;
typedef struct Interpreter *Interpreter;
typedef Value (*Func) (Procedure proc, Value at, Exp arg);
typedef Value (*InitFunc) ();

#include "exp.h"
#include "value.h"
#include "global.h"
#include "eval.h"
#include "lib.h"
#include "debug.h"

#endif
