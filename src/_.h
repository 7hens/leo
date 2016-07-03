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

#define LEO_DEBUG
#define USE_WINDOWS

#define LEO_VERSION_MAJOR 	"0"
#define LEO_VERSION_MINOR 	"1"
#define LEO_VERSION_RELEASE "0"
#define LEO_VERSION_NUM 	3
#define LEO_VERSION 	"Leo " LEO_VERSION_MAJOR "." LEO_VERSION_MINOR
#define LEO_RELEASE 	LEO_VERSION "." LEO_VERSION_RELEASE
#define LEO_COPYRIGHT	LEO_RELEASE "	Copyright (C) "
#define LEO_AUTHORS		"7hens"

/*********************************************/
#if defined(USE_WINDOWS)
#define DIR_SEPERATOR '\\'
#else
#define DIR_SEPERATOR '/'
#endif
/*********************************************/

/*********************************************/
#if LONG_MAX >> 31 > 0
#define WORD_BYTES 8     /* 64-bit */
#elif INT_MAX >> 15 > 0
#define WORD_BYTES 4     /* 32-bit */
#else
#define WORD_BYTES 2     /* 16-bit */
#endif
/*********************************************/

/*********************************************/
#if WORD_BYTES == 8

typedef signed long intx;
typedef unsigned long uintx;
typedef signed int int32;
typedef unsigned int uint32;

#elif WORD_BYTES == 4

typedef signed int intx;
typedef unsigned int uintx;
typedef signed int int32;
typedef unsigned int uint32;

#else

typedef signed int intx;
typedef unsigned int uintx;
typedef signed long int32;
typedef unsigned long uint32;

#endif /* WORD_BYTES */
/*********************************************/

/*********************************************/
#define REAL_TYPE_FLOAT         1
#define REAL_TYPE_DOUBLE        2
#define REAL_TYPE_LONGDOUBLE    3
/*********************************************/

/*********************************************/
#ifndef REAL_TYPE

#if WORD_BYTES == 8
#define REAL_BYTES  	8
#define REAL_TYPE 		REAL_TYPE_DOUBLE
#else
#define REAL_BYTES      4
#define REAL_TYPE     	REAL_TYPE_FLOAT
#endif /* WORD_BYTES */

#endif /* REAL_TYPE */
/*********************************************/

/*********************************************/
#if REAL_TYPE == REAL_TYPE_FLOAT

typedef float real;
#define REAL_FORMAT     "%.7g"
#define real_limit(n)	FLT_##n
#define math_op(op)		op##f
#define math_s2r(s,p)	(strtof((s),(p)))

#elif REAL_TYPE == REAL_TYPE_DOUBLE

typedef double real;
#define REAL_FORMAT		"%.14g"
#define real_limit(n)   DBL_##n
#define math_op(op)     op##d
#define math_s2r(s,p)   (strtod((s),(p)))

#elif REAL_TYPE == REAL_TYPE_LONGDOUBLE

#define REAL_FORMAT     "%.19g"
#define real_limit(n)   LDBL_##n
#define math_op(op)     op##l
#define math_s2r(s,p)   (strtold((s),(p)))

#else

#error "numeric float type not defined"

#endif
/*********************************************/

/*********************************************/
typedef signed short int16;
typedef unsigned short uint16;
typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef unsigned char boolean;
typedef void *Pointer;
typedef char *Chars;
typedef FILE *File;
/*********************************************/

/*********************************************/
#define bitsof(t) (sizeof(t)<<3)
#define cast_type(t, x) ((t)(x))
#define cast_intx(x) ((intx)(x))
#define cast_uintx(x) ((uintx)(x))
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
#define math_abs(x) ((x) < 0 ? -(x) : (x))
#define math_sign(x) ((x) < 0 ? -1 : ((x) ? 1 : 0))
#define math_r2i(r, i) ((i = (r)) == (r))
#define math_pow2(n) (1 << (n))
/*********************************************/

/*********************************************/
#ifdef LEO_DEBUG
#define debug_assert(c) ((c)||printf("debug>> assertion failed: %s, #%d, "#c"\n",__FILE__,__LINE__))
#define debug_log(format, arg...)	(printf(format, arg))
#else
#define debug_assert(c) 			((void)0)
#define debug_log(format, arg...) 	((void)0)
#endif
/*********************************************/

/*********************************************/
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
/*********************************************/

/*********************************************/
#include "exp.h"
#include "value.h"
#include "global.h"
#include "eval.h"
#include "lib.h"
#include "debug.h"
/*********************************************/

#endif
