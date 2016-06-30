%{
#include "_.h"
extern FILE *yyin;
extern Interpreter at_currentInterpreter;
extern Procedure at_rootProcedure;
%}
%union {
	Exp exp;
	ExpType expType;
}
%token <exp> _num _str _estr _ident
%token
	OR AND 						OR_ASSIGN AND_ASSIGN ASSIGN 
	LT LE EQ GE GT NE 			LT_ASSIGN LE_ASSIGN EQ_ASSIGN GE_ASSIGN GT_ASSIGN NE_ASSIGN
	ADD SUB MUL DIV CAT			ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN CAT_ASSIGN
	NOT AT ME DOT COLON ACCESSOR SEMICOLON COMMA LA RA LP RP LB RB LC RC
	ECAT
	TO LESS_TO BY
	FUNCTION RETURN RETURN_IF RETURN_UNLESS
	IF UNLESS ELSE SWITCH CASE
	WHILE UNTIL FOREACH IN IN_OWN BREAK BREAK_IF BREAK_UNLESS CONTINUE CONTINUE_IF CONTINUE_UNLESS
%type <exp>
	block stat expList caseList func parameter argument accessor
	exp string estr member primaryExp unaryExp binaryExp ternaryExp basicExp blockExp objectExp basicExp_N
%type <expType> breakOpr breakIfOpr inOpr assignOpr atOpr

%right CASE
%right SEMICOLON
%right COMMA
%right WHILE UNTIL
%nonassoc NO_ELSE
%nonassoc ELSE
%right IF UNLESS
%left OR
%left AND
%left EQ NE
%left LT LE GE GT
%left CAT
%left ADD SUB
%left MUL DIV
%left DOT
%left ECAT
%%
main
	: block { at_currentInterpreter->proc = Procedure_new(at_rootProcedure, Assoc_new(), NULL, NULL, $1); }
;
block
	: stat
	| { $$ = NULL; }
	| block SEMICOLON block { $$ = Exp_binary(COMMA_EXP, $1, $3); }
	| block COMMA block { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
stat
	: exp
	| breakOpr { $$ = Exp_unary($1, NULL); }
	| breakOpr exp { $$ = Exp_unary($1, $2); }
	| breakIfOpr basicExp { $$ = Exp_unary($1, $2); }
;
caseList
	: expList LC block RC { $$ = Exp_binary(CASE_EXP, $1, $3); }
	| caseList CASE caseList { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
expList
	: exp
	| expList COMMA expList { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
argument
	: exp
	| { $$ = NULL; }
	| argument COMMA argument { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
parameter
	: _ident
	| parameter COMMA parameter { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
func
	: parameter LC block RC { $$ = Exp_binary(FUNCTION_EXP, $1, $3); }
;
accessor
	: _ident LC block RC { $$ = Exp_binary(FUNCTION_EXP, $1, $3); }
;
objectExp
	: blockExp | ternaryExp
	| { $$ = NULL; }
	| ME COLON func { $$ = Exp_binary(ASSIGN_EXP, NULL, $3); }
	| member COLON exp { $$ = Exp_binary(COLON_EXP, $1, $3); }
	| member FUNCTION func { $$ = Exp_binary(COLON_EXP, $1, $3); }
	| member ACCESSOR accessor { $$ = Exp_binary(ACCESSOR_EXP, $1, $3); }
	| objectExp COMMA objectExp { $$ = Exp_binary(COMMA_EXP, $1, $3); }
;
exp
	: basicExp | blockExp
	| LC objectExp RC { $$ = Exp_unary(OBJECT_EXP, $2); }
;
blockExp
	: FUNCTION func { $$ = $2; }
	| IF basicExp LC block ELSE block RC { $$ = Exp_ternary(IF_EXP, $2, $4, $6); }
	| IF basicExp LC block RC %prec NO_ELSE { $$ = Exp_ternary(IF_EXP, $2, $4, NULL); }
	| IF basicExp LC block RC ELSE LC block RC { $$ = Exp_ternary(IF_EXP, $2, $4, $8); }
	| IF basicExp LC block RC ELSE blockExp { $$ = Exp_ternary(IF_EXP, $2, $4, $7); }
	| UNLESS basicExp LC block ELSE block RC { $$ = Exp_ternary(IF_EXP, $2, $6, $4); }
	| UNLESS basicExp LC block RC %prec NO_ELSE { $$ = Exp_ternary(IF_EXP, $2, NULL, $4); }
	| UNLESS basicExp LC block RC ELSE LC block RC { $$ = Exp_ternary(IF_EXP, $2, $8, $4); }
	| UNLESS basicExp LC block RC ELSE blockExp { $$ = Exp_ternary(IF_EXP, $2, $7, $4); }
	| SWITCH basicExp LC CASE caseList ELSE block RC { $$ = Exp_ternary(SWITCH_EXP, $2, $5, $7); }
	| SWITCH basicExp LC CASE caseList RC %prec NO_ELSE { $$ = Exp_ternary(SWITCH_EXP, $2, $5, NULL); }
	| SWITCH basicExp LC CASE caseList RC ELSE LC block RC { $$ = Exp_ternary(SWITCH_EXP, $2, $5, $9); }
	| SWITCH basicExp LC CASE caseList RC ELSE blockExp { $$ = Exp_ternary(SWITCH_EXP, $2, $5, $8); }
	| WHILE basicExp LC block RC { $$ = Exp_binary(WHILE_EXP, $2, $4); }
	| WHILE basicExp_N SEMICOLON basicExp_N SEMICOLON basicExp_N LC block RC {
		$$ = Exp_binary(COMMA_EXP, $2, Exp_binary(WHILE_EXP, $4, Exp_binary(COMMA_EXP, $8, $6)));
	}
	| UNTIL basicExp_N LC block RC { $$ = Exp_binary(UNTIL_EXP, $2, $4); }
	| UNTIL basicExp_N SEMICOLON basicExp_N SEMICOLON basicExp_N LC block RC {
		$$ = Exp_binary(COMMA_EXP, $2, Exp_binary(UNTIL_EXP, $4, Exp_binary(COMMA_EXP, $8, $6)));
	}
	| FOREACH _ident inOpr basicExp LC block RC { $$ = Exp_ternary($3, $2, $4, $6); }
	| FOREACH _ident COMMA _ident inOpr basicExp LC block RC {
		$$ = Exp_ternary($5, Exp_binary(COMMA_EXP, $2, $4), $6, $8);
	}
;
basicExp_N
	: basicExp
	| { $$ = NULL; }
;
basicExp
	: ternaryExp
	| primaryExp assignOpr exp { $$ = Exp_binary($2, $1, $3); }
	| primaryExp FUNCTION func { $$ = Exp_binary(ASSIGN_EXP, $1, $3); }
	| primaryExp ACCESSOR accessor { $$ = Exp_binary(ACCESSOR_EXP, $1, $3); }
;
ternaryExp
	: binaryExp
	| binaryExp TO binaryExp { $$ = Exp_ternary(TO_EXP, $1, $3, NULL); }
	| binaryExp TO binaryExp BY binaryExp { $$ = Exp_ternary(TO_EXP, $1, $3, $5); }
	| binaryExp LESS_TO binaryExp { $$ = Exp_ternary(LESS_TO_EXP, $1, $3, NULL); }
	| binaryExp LESS_TO binaryExp BY binaryExp { $$ = Exp_ternary(LESS_TO_EXP, $1, $3, $5); }
	| primaryExp IF basicExp %prec NO_ELSE { $$ = Exp_ternary(IF_EXP, $3, $1, NULL); }
	| primaryExp IF basicExp ELSE primaryExp { $$ = Exp_ternary(IF_EXP, $3, $1, $5); }
	| primaryExp UNLESS basicExp %prec NO_ELSE { $$ = Exp_ternary(IF_EXP, $3, NULL, $1); }
	| primaryExp UNLESS basicExp ELSE primaryExp { $$ = Exp_ternary(IF_EXP, $3, $5, $1); }
	| primaryExp WHILE basicExp { $$ = Exp_binary(WHILE_EXP, $3, $1); }
	| primaryExp UNTIL basicExp_N { $$ = Exp_binary(UNTIL_EXP, $3, $1); }
	| primaryExp FOREACH _ident inOpr basicExp { $$ = Exp_ternary($4, $3, $5, $1); }
	| primaryExp FOREACH _ident COMMA _ident inOpr basicExp {
		$$ = Exp_ternary($6, Exp_binary(COMMA_EXP, $3, $5), $7, $1);
	}
;
binaryExp
	: unaryExp
	| binaryExp OR binaryExp { $$ = Exp_binary(OR_EXP, $1, $3); }
	| binaryExp AND binaryExp { $$ = Exp_binary(AND_EXP, $1, $3); }
	| binaryExp EQ binaryExp { $$ = Exp_binary(EQ_EXP, $1, $3); }
	| binaryExp NE binaryExp { $$ = Exp_binary(NE_EXP, $1, $3); }
	| binaryExp LT binaryExp { $$ = Exp_binary(LT_EXP, $1, $3); }
	| binaryExp LE binaryExp { $$ = Exp_binary(LE_EXP, $1, $3); }
	| binaryExp GE binaryExp { $$ = Exp_binary(GE_EXP, $1, $3); }
	| binaryExp GT binaryExp { $$ = Exp_binary(GT_EXP, $1, $3); }
	| binaryExp CAT binaryExp { $$ = Exp_binary(CAT_EXP, $1, $3); }
	| binaryExp ADD binaryExp { $$ = Exp_binary(ADD_EXP, $1, $3); }
	| binaryExp SUB binaryExp { $$ = Exp_binary(SUB_EXP, $1, $3); }
	| binaryExp MUL binaryExp { $$ = Exp_binary(MUL_EXP, $1, $3); }
	| binaryExp DIV binaryExp { $$ = Exp_binary(DIV_EXP, $1, $3); }
;
unaryExp
	: primaryExp
	| ADD unaryExp { $$ = Exp_unary(ABS_EXP, $2); }
	| SUB unaryExp { $$ = Exp_unary(MINUS_EXP, $2); }
	| NOT unaryExp { $$ = Exp_unary(NOT_EXP, $2); }
	| atOpr { $$ = Exp_unary($1, NULL); }
;
primaryExp
	: _num | string | _ident
	| LP exp RP { $$ = $2; }
	| primaryExp _num { $$ = Exp_binary(CALL_EXP, $1, $2); }
	| primaryExp string { $$ = Exp_binary(CALL_EXP, $1, $2); }
	| primaryExp _ident { $$ = Exp_binary(CALL_EXP, $1, $2); }
	| primaryExp LP argument RP { $$ = Exp_binary(CALL_EXP, $1, $3); }
	| primaryExp DOT member { $$ = Exp_binary(MEMBER_EXP, $1, $3); }
	| atOpr member { $$ = Exp_unary($1, $2); }
	| primaryExp AT { $$ = Exp_unary(REQUIRE_EXP, $1); }
;
member
	: _num | string
	| _ident { $1->type = VALUE_EXP; $$ = $1; }
	| LP exp RP { $$ = $2; }
;
string
	: _str
	| estr ECAT _str { $$ = Exp_binary(CAT_EXP, $1, $3); }
;
estr
	: _estr exp { $$ = Exp_binary(CAT_EXP, $1, $2); }
	| estr ECAT estr { $$ = Exp_binary(CAT_EXP, $1, $3); }
;
atOpr
	: AT { $$ = AT_EXP; }
	| ME { $$ = ME_EXP; }
;
breakOpr
	: RETURN { $$ = RETURN_EXP; }
	| BREAK { $$ = BREAK_EXP; }
	| CONTINUE { $$ = CONTINUE_EXP; }
;
breakIfOpr
	: RETURN_IF { $$ = RETURN_IF_EXP; }
	| RETURN_UNLESS { $$ = RETURN_UNLESS_EXP; }
	| BREAK_IF { $$ = BREAK_IF_EXP; }
	| BREAK_UNLESS { $$ = BREAK_UNLESS_EXP; }
	| CONTINUE_IF { $$ = CONTINUE_IF_EXP; }
	| CONTINUE_UNLESS { $$ = CONTINUE_UNLESS_EXP; }
;
inOpr
	: IN { $$ = IN_EXP; }
	| IN_OWN { $$ = IN_OWN_EXP; }
;
assignOpr
	: ASSIGN { $$ = ASSIGN_EXP; }
	| OR_ASSIGN { $$ = OR_ASSIGN_EXP; }
	| AND_ASSIGN { $$ = AND_ASSIGN_EXP; }
	| EQ_ASSIGN { $$ = EQ_ASSIGN_EXP; }
	| NE_ASSIGN { $$ = NE_ASSIGN_EXP; }
	| LT_ASSIGN { $$ = LT_ASSIGN_EXP; }
	| LE_ASSIGN { $$ = LE_ASSIGN_EXP; }
	| GT_ASSIGN { $$ = GT_ASSIGN_EXP; }
	| GE_ASSIGN { $$ = GE_ASSIGN_EXP; }
	| CAT_ASSIGN { $$ = CAT_ASSIGN_EXP; }
	| ADD_ASSIGN { $$ = ADD_ASSIGN_EXP; }
	| SUB_ASSIGN { $$ = SUB_ASSIGN_EXP; }
	| MUL_ASSIGN { $$ = MUL_ASSIGN_EXP; }
	| DIV_ASSIGN { $$ = DIV_ASSIGN_EXP; }
;
%%

