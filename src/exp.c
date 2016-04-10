#include "_.h"

extern int yylineno;

void BinaryExp_free (BinaryExp self) {
	if (self) {
		Exp_free(self->a);
		Exp_free(self->b);
		free(self);
	}
}
BinaryExp BinaryExp_new (Exp a, Exp b) {
	BinaryExp node = struct_new(BinaryExp);
	node->a = a;
	node->b = b;
	return node;
}
void TernaryExp_free (TernaryExp self) {
	if (self) {
		Exp_free(self->a);
		Exp_free(self->b);
		Exp_free(self->c);
		free(self);
	}
}
TernaryExp TernaryExp_new (Exp a, Exp b, Exp c) {
	TernaryExp node = struct_new(TernaryExp);
	node->a = a;
	node->b = b;
	node->c = c;
	return node;
}


int ExpType_isUnary (ExpType self) {
	switch (self) {
	case OBJECT_EXP:case REQUIRE_EXP:case AT_EXP: case ME_EXP:case ABS_EXP:case MINUS_EXP:case NOT_EXP:
	case RETURN_EXP:case RETURN_IF_EXP:case RETURN_UNLESS_EXP:
	case BREAK_EXP:case BREAK_IF_EXP:case BREAK_UNLESS_EXP:
	case CONTINUE_EXP:case CONTINUE_IF_EXP:case CONTINUE_UNLESS_EXP:
		return 1;
	default:
		return 0;
	}
}
int ExpType_isBinary (ExpType self) {
	switch (self) {
	case AND_EXP:case OR_EXP:
	case EQ_EXP:case NE_EXP:case GT_EXP:case GE_EXP:case LT_EXP:case LE_EXP:
	case CAT_EXP:case ADD_EXP:case SUB_EXP:case MUL_EXP:case DIV_EXP:
	case ASSIGN_EXP:case AND_ASSIGN_EXP:case OR_ASSIGN_EXP:
	case EQ_ASSIGN_EXP:case NE_ASSIGN_EXP:
	case GT_ASSIGN_EXP:case GE_ASSIGN_EXP:case LT_ASSIGN_EXP:case LE_ASSIGN_EXP:
	case CAT_ASSIGN_EXP:case ADD_ASSIGN_EXP:case SUB_ASSIGN_EXP:case MUL_ASSIGN_EXP:case DIV_ASSIGN_EXP:
	case MEMBER_EXP:case CALL_EXP:case COMMA_EXP:case CASE_EXP:case COLON_EXP:case ACCESSOR_EXP:
	case FUNCTION_EXP:case WHILE_EXP:case UNTIL_EXP:
		return 1;
	default:
		return 0;
	}
}
int ExpType_isTernary (ExpType self) {
	switch (self) {
	case IF_EXP:case SWITCH_EXP:
	case IN_EXP:case IN_OWN_EXP:case TO_EXP:case LESS_TO_EXP:
		return 1;
	default:
		return 0;
	}
}
int ExpType_isBreak (ExpType self) {
	switch (self) {
		case RETURN_EXP:case RETURN_IF_EXP:case RETURN_UNLESS_EXP:
		case BREAK_EXP:case BREAK_IF_EXP:case BREAK_UNLESS_EXP:
		case CONTINUE_EXP:case CONTINUE_IF_EXP:case CONTINUE_UNLESS_EXP:
			return 1;
		default:
			return 0;
	}
}
ExpType ExpType_assign2 (ExpType self) {
	switch (self) {
		case AND_ASSIGN_EXP: return AND_EXP;
		case OR_ASSIGN_EXP: return OR_EXP;
		case EQ_ASSIGN_EXP: return EQ_EXP;
		case NE_ASSIGN_EXP: return NE_EXP;
		case GT_ASSIGN_EXP: return GT_EXP;
		case GE_ASSIGN_EXP: return GE_EXP;
		case LT_ASSIGN_EXP: return LT_EXP;
		case LE_ASSIGN_EXP: return LE_EXP;
		case CAT_ASSIGN_EXP: return CAT_EXP;
		case ADD_ASSIGN_EXP: return ADD_EXP;
		case SUB_ASSIGN_EXP: return SUB_EXP;
		case MUL_ASSIGN_EXP: return MUL_EXP;
		case DIV_ASSIGN_EXP: return DIV_EXP;
		default: return self;
	}
}
// ================================================================================================
// Exp
// ================================================================================================
void Exp_free (Exp self) {
	ExpType type;
	if (!self) return;
	type = self->type;
	if (ExpType_isLiteral(type)) {
		Value_free(self->u.value);
	} else if (ExpType_isUnary(type)) {
		Exp_free(self->u.unary);
	} else if (ExpType_isBinary(type)) {
		BinaryExp_free(self->u.binary);
	} else if (ExpType_isTernary(type)) {
		TernaryExp_free(self->u.ternary);
	}
	free(self);
}
Exp Exp_value (Value value) {
	Exp node = struct_new(Exp);
	node->type = VALUE_EXP;
	node->lineno = yylineno;
	node->u.value = value;
	return node;
}
Exp Exp_identifier (Chars value) {
	Exp node = struct_new(Exp);
	node->type = IDENTIFIER_EXP;
	node->lineno = yylineno;
	node->u.value = Value_string(value);
	++node->u.value->used;
	return node;
}
Exp Exp_unary (ExpType type, Exp unary) {
	Exp node = struct_new(Exp);
	node->type = type;
	node->lineno = yylineno;
	node->u.unary = unary;
	return node;
}
Exp Exp_binary (ExpType type, Exp a, Exp b) {
	Exp node = struct_new(Exp);
	node->type = type;
	node->lineno = yylineno;
	node->u.binary = BinaryExp_new(a, b);
	return node;
}
Exp Exp_ternary (ExpType type, Exp a, Exp b, Exp c) {
	Exp node = struct_new(Exp);
	node->type = type;
	node->lineno = yylineno;
	node->u.ternary = TernaryExp_new(a, b, c);
	return node;
}

