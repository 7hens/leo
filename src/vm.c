#include "_.h"

extern Value AT_VALUES;

#define _BITS_OF_OP 5
#define _BITS_OF_REG 9
#define _BITS_OF_POOL 18
#define _POOL_ALLOC_SIZE 256
#define _POOL_MAX_SIZE (math_pow2(_BITS_OF_POOL))

static uint32 _poolSize = 0;
static Value *_pool = NULL;

static Value *_KRegister_Values = [
	NULL,
	System_True,
	System_Number,
	System_String,
	System_Function,
	System_Buildin,
	System_Accessor
];

typedef Value (*_OpcodeFunc) (uint32 code);
static _OpcodeFunc *_Opcode_Handlers = [
	_Opcode_load,	/* RA = pool[BC] */
	_Opcode_jump,	/* ?: RA {pc += sBC} */
	_Opcode_move,	/* (A:) RB = RC */
	_Opcode_top, 	/* RA = top(B) [C] */
	_Opcode_add, 	/* RA = RB + RC */
	_Opcode_sub,	/* RA = RB - RC */
	_Opcode_mul, 	/* RA = RB * RC */
	_Opcode_div,	/* RA = RB / RC */
	_Opcode_mod,	/* RA = RB \ RC */
	_Opcode_cat,	/* RA = RB ~ RC */
	_Opcode_eq,		/* RA = RB == RC */
	_Opcode_ne,		/* RA = RB != RC */
	_Opcode_lt,		/* RA = RB << RC */
	_Opcode_le,		/* RA = RB <= RC */
	_Opcode_gt,		/* RA = RB >> RC */
	_Opcode_ge,		/* RA = RB >= RC */
	_Opcode_and,	/* RA = RB & RC */
	_Opcode_or,		/* RA = RB | RC */
	_Opcode_xor,	/* RA = RB ^ RC */
	_Opcode_new,	/* RA = {meta:RB, size:C} */
	_Opcode_get,	/* RA = RB.RC */
	_Opcode_set,	/* RA.RB = RC */
	_Opcode_plus	/* extra */
];

