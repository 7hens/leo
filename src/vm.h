#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED
typedef enum Opcode Opcode;
typedef struct Stack *Stack;
//typedef struct Procedure *Procedure;
typedef struct CallFrame *CallFrame;

enum Opcode {
	OP_LOAD,	/* RA = pool[BC] */
};
struct Stack {
	Value value;
	Stack next;
};
struct _Procedure {
	Value *pool;
	uint32 poolCont;
	uint32 *code;
	uint32 *codeCount;
	uint8 regCount;
};
struct CallFrame {
	Value *reg;
	uint32 pc;
	CallFrame meta;
	Procedure me;	// R_me
	Value at;		// R_at
	Stack temp;		// R_temp
	//List gcList;
};

#endif

