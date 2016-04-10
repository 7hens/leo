#include "_.h"

extern Interpreter at_currentInterpreter;
extern Value at_TV_startIndex, at_TV_endIndex;
extern FILE *yyin;
extern int yylineno;
extern Value AT_VALUES;

Interpreter at_currentInterpreter = NULL;
Procedure at_rootProcedure = NULL;
// ================================================================================================
// Interpreter
// ================================================================================================
static Interpreter __interpreterTable = NULL;
Interpreter Interpreter_init () {
	if (__interpreterTable) return __interpreterTable;
	Interpreter self = struct_new(Interpreter);
	Assoc assoc = Assoc_new();
	Assoc_set(assoc, Value_string("True"), at_True);
	Assoc_set(assoc, Value_string("Number"), at_Number);
	Assoc_set(assoc, Value_string("String"), at_String);
	Assoc_set(assoc, Value_string("Array"), at_Array);
	Assoc_set(assoc, Value_string("Function"), at_Function);
	self->proc = (at_rootProcedure = Procedure_new(NULL, assoc, NULL, NULL, NULL));
	__interpreterTable = self;
	return self;
}
void Interpreter_free () {
	Interpreter i = __interpreterTable;
	if (!i) return;
	while (i = i->next) {
		__interpreterTable->next = i->next;
		Value_free(i->filePath);
		Procedure_free(i->proc);
		Value_free(i->result);
		free(i);
	}
	free(__interpreterTable);
}
static Interpreter __Interpreter_new (Value filePath) {
	Interpreter node = struct_new(Interpreter);
	InitFunc func = NULL;
	node->filePath = filePath;
	at_currentInterpreter = node;
	yylineno = 1;
	if (filePath) {
		if (func = LIB_getInitFunc(filePath->u.string)) {
			node->result = func();
			return node;
		}
		yyin = fopen(filePath->u.string, "rt");
		if (!yyin) {
			perror(filePath->u.string);
			return NULL;
		}
		yyrestart(yyin);
		yyparse();
		fclose(yyin);
	} else {
		//yyin = stdin;
		printf("Interpreter._new: yyparse\n");
		//yyrestart(yyin);
		yyparse();
		printf("Interpreter._new: yyparse end\n");
	}
	node->result = Procedure_eval(node->proc, node->proc->exp);
	return node;
}
Value Interpreter_eval (Value filePath) {
	debug_assert(__interpreterTable);
	Interpreter i = NULL, j = __interpreterTable;
	for (i = j->next; i && i->filePath != filePath; i = (j = i)->next);
	if (!i) {
		if (!(i = __Interpreter_new(filePath))) return NULL;
	} else {
		j->next = i->next;
	}
	i->next = __interpreterTable->next;
	__interpreterTable->next = i;
	return i->result;
}
Value Interpreter_getFilePath (Interpreter current, Value self) {
	Value result = NULL;
	char currentDir[256];
	Chars dir = current->filePath ? String_getDir(current->filePath->u.string) : getcwd(currentDir, 256);
	Chars absolutePath = String_getAbsolutePath(dir, self->u.string);
	result = Value_string(absolutePath);
	//printf("_getFilePathValue: %s\n", absolutePath);
	free(absolutePath);
	free(dir);
	return result;
}



#define _EVAL(exp) (Procedure_eval(self, exp))
#define _evalFunction Procedure_evalFunction
#define _evalAccessor Procedure_evalAccessor

// ================================================================================================
// Procedure
// ================================================================================================
void Procedure_free (Procedure self) {
	if (!self) return;
	Assoc_free(self->assoc);
	free(self);
}
Procedure Procedure_new (Procedure meta, Assoc assoc, Value at, Value me, Exp exp) {
	Procedure node = struct_new(Procedure);
	node->meta = meta;
	node->assoc = assoc;
	node->at = at;
	node->me = me;
	node->exp = exp;
	node->active = 1;
	return node;
}
static Assoc _has (Procedure self, Value key) {
	if (!(self && key)) return NULL;
	Assoc assocNode = self->assoc ? Assoc_has(self->assoc, key) : NULL;
	return assocNode ? assocNode : _has(self->meta, key);
}
static Value _get (Procedure self, Value key) {
	if (!(self && key)) return NULL;
	Assoc assocNode = _has(self, key);
	Value result = assocNode ? assocNode->value : NULL;
	return result && Value_is(result, at_Accessor) ? _evalAccessor(NULL, result, result->u.value) : result;
}
static Value _set (Procedure self, Value key, Value value) {
	if (!(self && key)) return NULL;
	Assoc assocNode = _has(self, key);
	Value me = NULL;
	if (assocNode && (me = assocNode->value) && Value_is(me, at_Accessor)) {
		Procedure_evalAccessor(NULL, me, value);
	} else {
		if (!self->assoc) {
			self->assoc = Assoc_new();
		}
		Assoc_set(self->assoc, key, value);
	}
	return me;
}
static Value _remove (Procedure self, Value key) {
	return self && key && self->assoc ? Assoc_remove(self->assoc, key) : NULL;
}


// ================================================================================================
// _evalFunction, _evalAccessor, _evalMember, _evalCall
// ================================================================================================
Value Procedure_evalFunction (Procedure self, Value at, Value me, Exp arg) {
	Value result = NULL;
	Procedure proc = NULL;
	Exp para = NULL, paraE = NULL, argE = NULL;
	if (!(Value_is(me, at_Function) && me->u.proc)) {
		yyerror("wrong call expression.");
		return NULL;
	}
	if (Value_is(me, at_Func)) {
		return me->u.func(self, at, arg);
	}
	proc = Procedure_new(me->u.proc, Assoc_new(), at, me, me->u.proc->exp);
	para = proc->exp->u.binary->a;
	while (para && arg) {
		TRAVERSEL_COMMA_EXP(para, paraE);
		TRAVERSEL_COMMA_EXP(arg, argE);
		_set(proc, paraE->u.value, _EVAL(argE));
	}
	result = Procedure_eval(proc, proc->exp->u.binary->b);
	//Assoc_free(proc->assoc);
	//free(proc);
	return result;
}
Value Procedure_evalAccessor (Value at, Value me, Value arg) {
	if (!(me && Value_is(me, at_Accessor) && me->u.value)) {
		yyerror("wrong accessor call expression.");
		return NULL;
	}
	Value func = me->u.value;
	Exp tempExp = Exp_value(arg);
	Value result = Procedure_evalFunction(func->u.proc, at, func, tempExp);
	free(tempExp);
	return arg != func ? arg : result;
}
static Value _evalRequire (Value name) {
	if (!(Value_is(name, at_String))) return NULL;
	Interpreter currentInterpreter = at_currentInterpreter;
	if (!LIB_getInitFunc(name->u.string)) {
		name = Interpreter_getFilePath(at_currentInterpreter, name);
	}
	Value result = Interpreter_eval(name);
	at_currentInterpreter = currentInterpreter;
	return result;
}
static Value _evalCall (Procedure self, Exp a, Exp b) {
	Value at = NULL, me = NULL;
	if (a->type == MEMBER_EXP) {
		at = _EVAL(a->u.binary->a);
		me = Value_get(at, _EVAL(a->u.binary->b));
	} else if ((a->type == AT_EXP || a->type == ME_EXP) && a->u.unary) {
		at = a->type == AT_EXP ? self->at : self->me;
		me = Value_get(at, _EVAL(a->u.unary));
	} else {
		me = _EVAL(a);
	}
	return _evalFunction(self, at, me, b);
}


// ================================================================================================
// _evalAbs, _evalMath, _evalAssign, _evalTo, _evalComma
// ================================================================================================
static Value _evalAbs (Procedure self, ExpType type, Exp exp) {
	Value value = _EVAL(exp);
	real number = 0;
	if (!(Value_is(value, at_Number))) {
		// 产生一个临时变量
		value = Value_toNumberValue(value);
		if (!value) {
			yyerror("can't convert to number.");
			return NULL;
		}
	}
	number = value->u.number;
	return (type == ABS_EXP && number < 0) || (type == MINUS_EXP && number != 0) ? Value_number(-number) : value;
}
static Value _evalMath (Procedure self, ExpType type, Exp a, Exp b) {
	Value va = NULL, vb = NULL;
	switch (type) {
		case AND_EXP:case OR_EXP:
			va = _EVAL(a);
			return (type == AND_EXP && va) || (type == OR_EXP && !va) ? _EVAL(b) : va;
		case EQ_EXP:case NE_EXP:case GT_EXP:case GE_EXP:case LT_EXP:case LE_EXP:
			va = _EVAL(a);
			if (type == NE_EXP) {
				vb = _EVAL(b);
				if (!va) return vb;
				if (!vb) return va;
				return va != vb ? vb : NULL;
			} else if (type == EQ_EXP) {
				vb = _EVAL(b);
				if (va == at_True) return vb;
				if (vb == at_True) return va;
				if (va == vb) return va ? vb : at_True;
				return NULL;
			}
			if (!va) return NULL;
			vb = _EVAL(b);
			if (!vb) return NULL;
			if (type == EQ_EXP) {
				return va == vb ? vb : NULL;
			} else if (Value_is(va, at_Number) && (Value_is(vb, at_Number) || Value_is(vb, at_String))) {
				vb = Value_toNumberValue(vb);
				return (type == GT_EXP && va->u.number > vb->u.number)
					|| (type == GE_EXP && va->u.number >= vb->u.number)
					|| (type == LT_EXP && va->u.number < vb->u.number)
					|| (type == LE_EXP && va->u.number <- vb->u.number)
					? vb : NULL;
			} else if (Value_is(va, at_String) && (Value_is(vb, at_Number) || Value_is(vb, at_String))) {
				vb = Value_toStringValue(vb);
				return (type == GT_EXP && strcmp(va->u.string, vb->u.string) > 0)
					|| (type == GE_EXP && strcmp(va->u.string, vb->u.string) >= 0)
					|| (type == LT_EXP && strcmp(va->u.string, vb->u.string) < 0)
					|| (type == LE_EXP && strcmp(va->u.string, vb->u.string) <= 0)
					? vb : NULL;
			}
			return NULL;
		case CAT_EXP:
			va = Value_toStringValue(_EVAL(a));
			vb = Value_toStringValue(_EVAL(b));
			if (!va) return vb;
			if (!vb) return va;
			return Value_string(String_cat(va->u.string, vb->u.string));
		default:
			va = Value_toNumberValue(_EVAL(a));
			vb = Value_toNumberValue(_EVAL(b));
			if (!(va && vb)) return NULL;
			if (type == ADD_EXP) return Value_number(va->u.number + vb->u.number);
			if (type == SUB_EXP) return Value_number(va->u.number - vb->u.number);
			if (type == MUL_EXP) return Value_number(va->u.number * vb->u.number);
			if (type == DIV_EXP) return Value_number(va->u.number / vb->u.number);
			return NULL;
	}
}
static Value _evalAssign (Procedure self, Exp leftExp, Value rightValue) {
	Value memberValue = NULL;
	Value at = NULL;
	if (!leftExp) {
		yyerror("null left value in assign expression.");
		return NULL;
	};
	if (leftExp->type == MEMBER_EXP || leftExp->type == AT_EXP || leftExp->type == ME_EXP) {
		if (leftExp->type == MEMBER_EXP) {
			at = _EVAL(leftExp->u.binary->a);
			memberValue = _EVAL(leftExp->u.binary->b);
		} else {
			at = leftExp->type == AT_EXP ? self->at : self->me;
			memberValue = _EVAL(leftExp->u.unary);
		}
		Value_set(at, memberValue, rightValue);
	} else if (leftExp->type == IDENTIFIER_EXP) {
		_set(self, leftExp->u.value, rightValue);
	} else {
		yyerror("wrong left value in assign expression.");
	}
	return rightValue;
}
static Value _evalTo (Procedure self, ExpType type, Exp a, Exp b, Exp c) {
	Value va = _EVAL(a), vb = _EVAL(b), vc = _EVAL(c);
	if (!vc) { vc = Value_number(1); }
	if (!(Value_is(va, at_Number) && Value_is(vb, at_Number) && Value_is(vc, at_Number))) {
		yyerror("wrong to expression with NAN.");
		return NULL;
	}
	Value result =  Value_new(at_Array, NULL);
	int endIndex = 0;
	real i = va->u.number;
	for (;i < vb->u.number || i == vb->u.number && type == TO_EXP; i += vc->u.number) {
		Value_set(result, Value_number(endIndex++), Value_number(i));
	}
	Value_set(result, at_TV_endIndex, Value_number(endIndex));
	return result;
}
static Value _evalComma (Procedure self, Exp a, Exp b) {
	Value va = _EVAL(a);
	return b && self->active == 1 ? _EVAL(b) : va;
}


// ================================================================================================
// _evalTrue, _evalWhile, _evalForeach, _evalBreak, _evalSwitch
// ================================================================================================
static Value _evalTrue (Procedure self, Exp exp) {
	Value result = NULL, key = NULL, value = NULL;
	Exp iColon = NULL;
	real endIndex = 0;
	result = Value_new(at_True, NULL);
	while (exp) {
		TRAVERSEL_COMMA_EXP(exp, iColon);
		if (!iColon) continue;
		if (iColon->type == ASSIGN_EXP) {
			result->meta = at_Function;
			result->u.proc = Procedure_new(self, NULL, NULL, result, iColon->u.binary->b);
		} else {
			if (iColon->type != COLON_EXP && iColon->type != ACCESSOR_EXP) {
				key = Value_number(endIndex++);
				value = _EVAL(iColon);
			} else {
				key = _EVAL(iColon->u.binary->a);
				value = (iColon->type == ACCESSOR_EXP
					? Value_functionAccessor(self, iColon->u.binary->b)
					: _EVAL(iColon->u.binary->b)
				);
				if (Value_is(key, at_Number)) {
					endIndex = key->u.number + 1;
				}
			}
			Value_set(result, key, value);
		}
	}
	return result;
}
static Value _evalWhile (Procedure self) {
	Exp exp = self->exp;
	Value cond = _EVAL(exp->u.binary->a);
	Value result = Value_new(at_Array, NULL);
	int endIndex = 0;
	while (self->active && (exp->type == WHILE_EXP && cond || exp->type == UNTIL_EXP && !cond)) {
		self->active = 1;
		Value_set(result, Value_number(endIndex++), _EVAL(exp->u.binary->b));
		cond = _EVAL(exp->u.binary->a);
	}
	free(self);
	Value_set(result, at_TV_endIndex, Value_number(endIndex));
	return result;
}
static Value _evalForeach (Procedure self) {
	Exp exp = self->exp;
	Exp pairs = exp->u.ternary->a;
	Value collection = _EVAL(exp->u.ternary->b);
	if (!collection) return NULL;

	Value key = NULL, value = NULL;
	if (pairs->type == COMMA_EXP) {
		key = pairs->u.binary->a->u.value;
		value = pairs->u.binary->b->u.value;
	} else {
		value = pairs->u.value;
	}

	Assoc assoc = Value_pairs(collection, exp->type == IN_EXP);
	Assoc assocNode = assoc;
	Value result = Value_new(at_Array, NULL);
	int endIndex = 0;
	while (self->active && (assocNode = assocNode->next)) {
		self->active = 1;
		_set(self, key, assocNode->key);
		_set(self, value, assocNode->value);
		Value_set(result, Value_number(endIndex++),  _EVAL(exp->u.ternary->c));
	}
	Assoc_free(assoc);
	free(self);
	Value_set(result, at_TV_endIndex, Value_number(endIndex));
	return result;
}
// RETURN, BREAK, CONTINUE
static Value _evalBreak (Procedure self, ExpType type, Exp exp) {
	Value value = _EVAL(exp);
	Procedure i = self;
	if (type == RETURN_EXP || (type == RETURN_IF_EXP && value) || (type == RETURN_UNLESS_EXP && !value)) {
		for (; i && i->exp && i->exp->type != FUNCTION_EXP; i->active = 0, i = i->meta);
		if (i) i->active = 0;
	} else if (type == BREAK_EXP || (type == BREAK_IF_EXP && value) || (type == BREAK_UNLESS_EXP && !value)) {
		for (; i && i->exp && i->exp->type == FUNCTION_EXP; i->active = 0, i = i->meta);
		if (i) i->active = 0;
	} else if (type == CONTINUE_EXP || (type == CONTINUE_IF_EXP && value) || (type == CONTINUE_UNLESS_EXP && !value)) {
		for (; i && i->exp && i->exp->type == FUNCTION_EXP; i->active = 0, i = i->meta);
		if (i) i->active = 2;
	}
	return type == RETURN_EXP || type == BREAK_EXP || type == CONTINUE_EXP ? value : NULL;
}
static Value _evalSwitch (Procedure self, Exp a, Exp b, Exp c) {
	Value value = _EVAL(a);
	Exp iCaseList = b, iCase = NULL, iEnum = NULL, cur = NULL;
	while (iCaseList) {
		TRAVERSEL_COMMA_EXP(iCaseList, iCase);
		if (iCase->type != CASE_EXP) {
			yyerror("syntax error in switch block.");
			return _EVAL(c);
		}
		iEnum = iCase->u.binary->a;
		while (iEnum) {
			TRAVERSEL_COMMA_EXP(iEnum, cur);
			if (_EVAL(cur) == value) {
				return _EVAL(iCase->u.binary->b);
			}
		}
	}
	return _EVAL(c);
}


// ================================================================================================
// Procedure_eval
// ================================================================================================
Value Procedure_eval (Procedure self, Exp exp) {
	if (!(self && exp)) return NULL;
	/*
	if (exp->type != COMMA_EXP) {
		printf("%s: %d, %s\n", at_currentInterpreter->filePath->u.string, exp->lineno, ExpD_toString(exp));
	}
	*/
	at_currentInterpreter->currentExp = exp;
	switch (exp->type) {
		case VALUE_EXP:
			return exp->u.value;
		case IDENTIFIER_EXP:
			return _get(self, exp->u.value);
		case OBJECT_EXP:
			return _evalTrue(self, exp->u.unary);
		case ABS_EXP:case MINUS_EXP:
			return _evalAbs(self, exp->type, exp->u.unary);
		case NOT_EXP:
			return _EVAL(exp->u.unary) ? NULL : at_True;
		case RETURN_EXP:case RETURN_IF_EXP:case RETURN_UNLESS_EXP:
		case BREAK_EXP:case BREAK_IF_EXP:case BREAK_UNLESS_EXP:
		case CONTINUE_EXP:case CONTINUE_IF_EXP:case CONTINUE_UNLESS_EXP: 
			return _evalBreak(self, exp->type, exp->u.unary);
		case AND_EXP:case OR_EXP:
		case EQ_EXP:case NE_EXP:case GT_EXP:case GE_EXP:case LT_EXP:case LE_EXP:
		case CAT_EXP:case ADD_EXP:case SUB_EXP:case MUL_EXP:case DIV_EXP:
			return _evalMath(self, exp->type, exp->u.binary->a, exp->u.binary->b);
		case ASSIGN_EXP:
			//printf("debug: %s\n", ExpD_toString(exp));
			return _evalAssign(self, exp->u.binary->a, _EVAL(exp->u.binary->b));
		case AND_ASSIGN_EXP:case OR_ASSIGN_EXP:
		case EQ_ASSIGN_EXP:case NE_ASSIGN_EXP:
		case GT_ASSIGN_EXP:case GE_ASSIGN_EXP:case LT_ASSIGN_EXP:case LE_ASSIGN_EXP:
		case CAT_ASSIGN_EXP:case ADD_ASSIGN_EXP:case SUB_ASSIGN_EXP:case MUL_ASSIGN_EXP:case DIV_ASSIGN_EXP:
			//printf("debug: %s\n", ExpD_toString(exp));
			return _evalAssign(self, exp->u.binary->a,
				_evalMath(self, ExpType_assign2(exp->type), exp->u.binary->a, exp->u.binary->b));
		case REQUIRE_EXP:
			return _evalRequire(_EVAL(exp->u.unary));
		case AT_EXP:
			if (!exp->u.unary) return self->at;
			return Value_get(self->at, _EVAL(exp->u.unary));
		case ME_EXP:
			if (!exp->u.unary) return self->me;
			return Value_get(self->me, _EVAL(exp->u.unary));
		case MEMBER_EXP:
			return Value_get(_EVAL(exp->u.binary->a), _EVAL(exp->u.binary->b));
		case CALL_EXP:
			return _evalCall(self, exp->u.binary->a, exp->u.binary->b);
		case COMMA_EXP:
			return _evalComma(self, exp->u.binary->a, exp->u.binary->b);
		case CASE_EXP:
			yyerror("syntax error: unexpected case operator.");
			return NULL;
		case COLON_EXP:
			yyerror("syntax error: unexpected colon operator.");
			return NULL;
		case TO_EXP:case LESS_TO_EXP:
			return _evalTo(self, exp->type, exp->u.ternary->a, exp->u.ternary->b, exp->u.ternary->c);
		case ACCESSOR_EXP:
			//printf("debug: %s\n", ExpD_toString(exp));
			return _evalAssign(self, exp->u.binary->a, Value_functionAccessor(self, exp->u.binary->b));
		case FUNCTION_EXP:
			return Value_function(self, exp);
		case WHILE_EXP:case UNTIL_EXP:
			return _evalWhile(Procedure_new(self, self->assoc, self->at, self->me, exp));
		case IN_EXP:case IN_OWN_EXP:
			return _evalForeach(Procedure_new(self, self->assoc, self->at, self->me, exp));
		case IF_EXP:
			return _EVAL(_EVAL(exp->u.ternary->a) ? exp->u.ternary->b : exp->u.ternary->c);
		case SWITCH_EXP:
			return _evalSwitch(self, exp->u.ternary->a, exp->u.ternary->b, exp->u.ternary->c);
		default:
			yyerror("syntax error: unknown expression");
			return NULL;
	}
}

