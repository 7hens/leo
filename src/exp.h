#ifndef SYNTAX_H_INCLUDED
#define SYNTAX_H_INCLUDED

// ================================================================================================
// Exp
// ================================================================================================
enum ExpType {
	/* literal expression */
	IDENTIFIER_EXP, VALUE_EXP,
	/* unary expression */
	OBJECT_EXP, REQUIRE_EXP, AT_EXP, ME_EXP, ABS_EXP, MINUS_EXP, NOT_EXP,
	RETURN_EXP, RETURN_IF_EXP, RETURN_UNLESS_EXP,
	BREAK_EXP, BREAK_IF_EXP, BREAK_UNLESS_EXP,
	CONTINUE_EXP, CONTINUE_IF_EXP, CONTINUE_UNLESS_EXP,
	/* binary expression */
	AND_EXP, OR_EXP,
	EQ_EXP, NE_EXP, GT_EXP, GE_EXP, LT_EXP, LE_EXP,
	CAT_EXP, ADD_EXP, SUB_EXP, MUL_EXP, DIV_EXP,
	ASSIGN_EXP, AND_ASSIGN_EXP, OR_ASSIGN_EXP,
	EQ_ASSIGN_EXP, NE_ASSIGN_EXP, GT_ASSIGN_EXP, GE_ASSIGN_EXP, LT_ASSIGN_EXP, LE_ASSIGN_EXP,
	CAT_ASSIGN_EXP, ADD_ASSIGN_EXP, SUB_ASSIGN_EXP, MUL_ASSIGN_EXP, DIV_ASSIGN_EXP,
	MEMBER_EXP, CALL_EXP, COMMA_EXP, CASE_EXP, COLON_EXP, ACCESSOR_EXP,
	FUNCTION_EXP, WHILE_EXP, UNTIL_EXP,
	/* ternary expression */
	IF_EXP, SWITCH_EXP, IN_EXP, IN_OWN_EXP, TO_EXP, LESS_TO_EXP
};
#define ExpType_isLiteral(type) (type == VALUE_EXP || type == IDENTIFIER_EXP)
int ExpType_isUnary (ExpType self);
int ExpType_isBinary (ExpType self);
int ExpType_isTernary (ExpType self);
int ExpType_isMath (ExpType self);
int ExpType_isBreak (ExpType self);
ExpType ExpType_assign2 (ExpType self);
#define ExpType_isLoop(type) (type == WHILE_EXP || type == UNTIL_EXP || type == IN_EXP || type == IN_OWN_EXP)

struct Exp {
	ExpType type;
	int lineno;
	union {
		Value value;
		Exp unary;
		BinaryExp binary;
		TernaryExp ternary;
	} u;
};
void Exp_free (Exp self);
Exp Exp_value (Value value);
Exp Exp_identifier (Chars value);
Exp Exp_unary (ExpType type, Exp unary);
Exp Exp_binary (ExpType type, Exp a, Exp b);
Exp Exp_ternary (ExpType type, Exp a, Exp b, Exp c);

// ================================================================================================
// BinaryExp
// ================================================================================================
struct BinaryExp { Exp a, b; };
void BinaryExp_free (BinaryExp self);
BinaryExp BinaryExp_new (Exp a, Exp b);

// ================================================================================================
// TernaryExp
// ================================================================================================
struct TernaryExp { Exp a, b, c; };
void TernaryExp_free (TernaryExp self);
TernaryExp TernaryExp_new (Exp a, Exp b, Exp c);

#endif /* SYNTAX_H_INCLUDED */
