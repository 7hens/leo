#include "_.h"

extern Interpreter at_currentInterpreter;
#define STAT_LEVEL 1
#define EXP_LEVEL 2
#define BASIC_LEVEL 3 
#define BINARY_LEVEL 4
#define UNARY_LEVEL 5
#define PRIMARY_LEVEL 6
#define setExpTable(type, a, b) __expTable[type].format = a; __expTable[type].level = b

static struct {
	Chars format;
	int level;
} __expTable[128];

void ExpD_init () {
	setExpTable(OBJECT_EXP, "{ %s }", EXP_LEVEL);
	setExpTable(MINUS_EXP, "-%s", UNARY_LEVEL);
	setExpTable(ABS_EXP, "+%s", UNARY_LEVEL);
	setExpTable(NOT_EXP, "!%s", UNARY_LEVEL);
	setExpTable(RETURN_EXP, "//%s", STAT_LEVEL);
	setExpTable(RETURN_IF_EXP, "//?%s", STAT_LEVEL);
	setExpTable(RETURN_UNLESS_EXP, "//?:%s", STAT_LEVEL);
	setExpTable(BREAK_EXP, "--%s", STAT_LEVEL);
	setExpTable(BREAK_IF_EXP, "--?%s", STAT_LEVEL);
	setExpTable(BREAK_UNLESS_EXP, "--?:%s", STAT_LEVEL);
	setExpTable(CONTINUE_EXP, "++%s", STAT_LEVEL);
	setExpTable(CONTINUE_IF_EXP, "++?%s", STAT_LEVEL);
	setExpTable(CONTINUE_UNLESS_EXP, "++?:%s", STAT_LEVEL);
	setExpTable(AND_EXP, "%s & %s", BINARY_LEVEL);
	setExpTable(OR_EXP, "%s | %s", BINARY_LEVEL);
	setExpTable(EQ_EXP, "%s == %s", BINARY_LEVEL);
	setExpTable(NE_EXP, "%s != %s", BINARY_LEVEL);
	setExpTable(LT_EXP, "%s << %s", BINARY_LEVEL);
	setExpTable(LE_EXP, "%s <= %s", BINARY_LEVEL);
	setExpTable(GT_EXP, "%s >> %s", BINARY_LEVEL);
	setExpTable(GE_EXP, "%s >= %s", BINARY_LEVEL);
	setExpTable(CAT_EXP, "%s ~ %s", BINARY_LEVEL);
	setExpTable(ADD_EXP, "%s + %s", BINARY_LEVEL);
	setExpTable(SUB_EXP, "%s - %s", BINARY_LEVEL);
	setExpTable(MUL_EXP, "%s * %s", BINARY_LEVEL);
	setExpTable(DIV_EXP, "%s / %s", BINARY_LEVEL);
	setExpTable(ASSIGN_EXP, "%s = %s", BASIC_LEVEL);
	setExpTable(AND_ASSIGN_EXP, "%s &= %s", BASIC_LEVEL);
	setExpTable(OR_ASSIGN_EXP, "%s |= %s", BASIC_LEVEL);
	setExpTable(EQ_ASSIGN_EXP, "%s === %s", BASIC_LEVEL);
	setExpTable(NE_ASSIGN_EXP, "%s !== %s", BASIC_LEVEL);
	setExpTable(LT_ASSIGN_EXP, "%s <<= %s", BASIC_LEVEL);
	setExpTable(LE_ASSIGN_EXP, "%s <== %s", BASIC_LEVEL);
	setExpTable(GT_ASSIGN_EXP, "%s >>= %s", BASIC_LEVEL);
	setExpTable(GE_ASSIGN_EXP, "%s >== %s", BASIC_LEVEL);
	setExpTable(CAT_ASSIGN_EXP, "%s ~= %s", BASIC_LEVEL);
	setExpTable(ADD_ASSIGN_EXP, "%s += %s", BASIC_LEVEL);
	setExpTable(SUB_ASSIGN_EXP, "%s -= %s", BASIC_LEVEL);
	setExpTable(MUL_ASSIGN_EXP, "%s *= %s", BASIC_LEVEL);
	setExpTable(DIV_ASSIGN_EXP, "%s /= %s", BASIC_LEVEL);
	setExpTable(REQUIRE_EXP, "%s@", PRIMARY_LEVEL);
	setExpTable(AT_EXP, "@%s", PRIMARY_LEVEL);
	setExpTable(MEMBER_EXP, "%s.%s", PRIMARY_LEVEL);
	setExpTable(ME_EXP, "@.%s", PRIMARY_LEVEL);
	setExpTable(CALL_EXP, "%s(%s)", PRIMARY_LEVEL);
	setExpTable(COLON_EXP, "%s: %s", STAT_LEVEL);
	setExpTable(ACCESSOR_EXP, "%s := %s", BASIC_LEVEL);
	setExpTable(COMMA_EXP, "%s, %s", STAT_LEVEL);
	setExpTable(CASE_EXP, ":>%s {%s}", STAT_LEVEL);
	setExpTable(FUNCTION_EXP, "$ %s { %s }", EXP_LEVEL);
	setExpTable(WHILE_EXP, "%% %s { %s }", EXP_LEVEL);
	setExpTable(UNTIL_EXP, "%%: %s { %s }", EXP_LEVEL);
	setExpTable(IN_EXP, "%% %s =>> %s { %s }", BASIC_LEVEL);
	setExpTable(IN_OWN_EXP, "%% %s => %s { %s }", BASIC_LEVEL);
	setExpTable(IF_EXP, "? %s { %s :: %s }", EXP_LEVEL);
	setExpTable(SWITCH_EXP, "?> %s { %s :: %s }", EXP_LEVEL);
	setExpTable(TO_EXP, "%s +> %s ~~ %s", BASIC_LEVEL);
	setExpTable(LESS_TO_EXP, "%s -> %s ~~ %s", BASIC_LEVEL);
}
Chars ExpD_toString (Exp self) {
	ExpType type;
	if (!self) return NULL;
	type = self->type;
	if (type == VALUE_EXP || type == IDENTIFIER_EXP) {
		return Value_toStringValue(self->u.value)->u.string;
	} else if (ExpType_isUnary(type)) {
		return String_format(__expTable[type].format, ExpD_toString(self->u.unary));
	} else if (ExpType_isBinary(type)) {
		return String_format(__expTable[type].format,
			ExpD_toString(self->u.binary->a),
			ExpD_toString(self->u.binary->b)
		);
	} else if (ExpType_isTernary(type)) {
		return String_format(__expTable[type].format,
			ExpD_toString(self->u.ternary->a),
			ExpD_toString(self->u.ternary->b),
			ExpD_toString(self->u.ternary->c)
		);
	}
	return NULL;
}

void yyerror (Chars msg, ...) {
	va_list arg;
	va_start(arg, msg);
	fprintf(stderr, "error> %s ", at_currentInterpreter->filePath ? at_currentInterpreter->filePath->u.string : "");
	Exp currentExp = at_currentInterpreter->currentExp;
	if (currentExp) {
		fprintf(stderr, "(#%d, %s) ", currentExp->lineno, ExpD_toString(currentExp));
	}
	vfprintf(stderr, msg, arg);
	va_end(arg);
	fprintf(stderr, "\n");
}




