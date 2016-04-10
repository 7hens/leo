#ifndef RUNTIME_H_INCLUDED
#define RUNTIME_H_INCLUDED

#define TRAVERSEL_COMMA_EXP(next, cur) if (next->type == COMMA_EXP) {\
		cur = next->u.binary->a;\
		next = next->u.binary->b;\
	} else {\
		cur = next;\
		next = NULL;\
	}
	
// ================================================================================================
// Procedure
// ================================================================================================
struct Procedure {
	Procedure meta;
	Assoc assoc; // 变量链表
	Value at, me;
	Exp exp;
	int active;
};
void Procedure_free (Procedure self);
Procedure Procedure_new (Procedure meta, Assoc assoc, Value at, Value me, Exp exp);
Procedure Procedure_newFile (Exp exp);
Value Procedure_evalFunction (Procedure self, Value at, Value me, Exp arg);
Value Procedure_evalAccessor (Value at, Value me, Value arg);
Value Procedure_eval (Procedure self, Exp exp);
// ================================================================================================
// Interpreter
// ================================================================================================
struct Interpreter {
	Value filePath;
	Procedure proc;
	Exp currentExp;
	Value result;
	Interpreter next;
};
Interpreter Interpreter_init ();
void Interpreter_free ();
Value Interpreter_eval (Value filePath);
Value Interpreter_getFilePath (Interpreter current, Value self);

#endif /* RUNTIME_H_INCLUDED */
