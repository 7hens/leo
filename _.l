%option noyywrap yylineno

%{
#include "_.h"
#include "yacc.h"

static int defaultTab = 0;
static int curTab = 0;
static int preCTab = 0;
static int preNTab = -1;
static int nocomma = 1;
%}

LINE_WRAP ([ \t\n]*)
%start COMMENT NORMAL STRING STRING_2 REGEX INT_MEMBER NEXT_FILE
%%
<INITIAL>##[^\n]*\n { preCTab = curTab; curTab = 0; BEGIN COMMENT; }
<INITIAL,COMMENT>[ \t]*\n { }
<INITIAL>\t+ { curTab = yyleng; }
<INITIAL>. {
	yyless(0);
	//printf("lex> %d, INITIAL: (preNTab, curTab) = (%d, %d)\n", yylineno, preNTab, curTab);
	if (preNTab < 0) {
		preNTab = curTab;
		defaultTab = curTab;
	} else if (curTab > preNTab) {
		++preNTab;
		if (curTab == preNTab) BEGIN NORMAL;
		//printf("lex> %d, INITIAL: '{'\n", yylineno);
		return LC;
	} else if (curTab < preNTab) {
		--preNTab;
		//printf("lex> %d, INITIAL: '}'\n", yylineno);
		return RC;
	}
	BEGIN NORMAL;
	if (nocomma) {
		--nocomma;
	} else {
		//printf("lex> %d, INITIAL: ','\n", yylineno);
		return COMMA;
	}
}
<COMMENT>\t+ {
	if (yyleng <= preCTab) {
		yyless(0);
		BEGIN INITIAL;
	}
}
<COMMENT>[^\t][^\n]*\n { }
<NORMAL>[ \t]+ {}
<NORMAL>\n\t*("::"|":>") {
	yyless(1);
	++nocomma;
	BEGIN INITIAL;
}
<NORMAL>\n { curTab = 0; BEGIN INITIAL; }
<NORMAL>"<" { return LA; }
<NORMAL>">" { return RA; }
<NORMAL>"("{LINE_WRAP}? { return LP; }
<NORMAL>"["{LINE_WRAP}? { return LB; }
<NORMAL>"{"{LINE_WRAP}? { return LC; }
<NORMAL>{LINE_WRAP}?")" { return RP; }
<NORMAL>{LINE_WRAP}?"]" { return RB; }
<NORMAL>{LINE_WRAP}?"}" { return RC; }
<NORMAL>"!" { return NOT; }
<NORMAL>"|"{LINE_WRAP}? { return OR; }
<NORMAL>"&"{LINE_WRAP}? { return AND; }
<NORMAL>"+"{LINE_WRAP}? { return ADD; }
<NORMAL>"-"{LINE_WRAP}? { return SUB; }
<NORMAL>"*"{LINE_WRAP}? { return MUL; }
<NORMAL>"/"{LINE_WRAP}? { return DIV; }
<NORMAL>"~"{LINE_WRAP}? { return CAT; }
<NORMAL>"<<"{LINE_WRAP}? { return LT; }
<NORMAL>"<="{LINE_WRAP}? { return LE; }
<NORMAL>"=="{LINE_WRAP}? { return EQ; }
<NORMAL>">="{LINE_WRAP}? { return GE; }
<NORMAL>">>"{LINE_WRAP}? { return GT; }
<NORMAL>"!="{LINE_WRAP}? { return NE; }
<NORMAL>"=" { return ASSIGN; }
<NORMAL>"|=" { return OR_ASSIGN; }
<NORMAL>"&=" { return AND_ASSIGN; }
<NORMAL>"+=" { return ADD_ASSIGN; }
<NORMAL>"-=" { return SUB_ASSIGN; }
<NORMAL>"*=" { return MUL_ASSIGN; }
<NORMAL>"/=" { return DIV_ASSIGN; }
<NORMAL>"~=" { return CAT_ASSIGN; }
<NORMAL>"<<=" { return LT_ASSIGN; }
<NORMAL>"<==" { return LE_ASSIGN; }
<NORMAL>"===" { return EQ_ASSIGN; }
<NORMAL>">==" { return GE_ASSIGN; }
<NORMAL>">>=" { return GT_ASSIGN; }
<NORMAL>"!==" { return NE_ASSIGN; }
<NORMAL>":=" { return ACCESSOR; }
<NORMAL>"+>" { return TO; }
<NORMAL>"->" { return LESS_TO; }
<NORMAL>"~~" { return BY; }
<NORMAL>{LINE_WRAP}?"."{LINE_WRAP}? { BEGIN INT_MEMBER; return DOT; }
<NORMAL>"@" { BEGIN INT_MEMBER; return AT; }
<INT_MEMBER>[\+\-]?(0|[1-9][0-9]*) {
	real number = 0;
	sscanf(yytext, "%f", &number);
	yylval.exp = Exp_value(Value_number(number));
	return _num;
}
<INT_MEMBER>(.|\n) { yyless(0); BEGIN NORMAL; }
<NORMAL>":" { return COLON; }
<NORMAL>";"{LINE_WRAP}? { return SEMICOLON; }
<NORMAL>","{LINE_WRAP}? { return COMMA; }
<NORMAL>"$" { return ME; }
<NORMAL>"$$" { return FUNCTION; }
<NORMAL>"//" { return RETURN; }
<NORMAL>"//?" { return RETURN_IF; }
<NORMAL>"//?:" { return RETURN_UNLESS; }
<NORMAL>"?" { return IF; }
<NORMAL>"?:" { return UNLESS; }
<NORMAL>"::" { return ELSE; }
<NORMAL>"?>" { return SWITCH; }
<NORMAL>":>" { return CASE; }
<NORMAL>"%" { return WHILE; }
<NORMAL>"%:" { return UNTIL; }
<NORMAL>"%>" { return FOREACH; }
<NORMAL>"=>" { return IN_OWN; }
<NORMAL>"=>>" { return IN; }
<NORMAL>"--" { return BREAK; }
<NORMAL>"--?" { return BREAK_IF; }
<NORMAL>"--?:" { return BREAK_UNLESS; }
<NORMAL>"++" { return CONTINUE; }
<NORMAL>"++?" { return CONTINUE_IF; }
<NORMAL>"++?:" { return CONTINUE_UNLESS; }
<NORMAL>[\+\-]?(0|[1-9][0-9]*)(\.[0-9]+)? {
	real number = 0;
	sscanf(yytext, "%f", &number);
	yylval.exp = Exp_value(Value_number(number));
	return _num;
}
<NORMAL>[_A-Za-z][_0-9A-Za-z]* {
	yylval.exp = Exp_identifier(yytext);
	//printf("lex> %d, NORMAL: %s\n", yylineno, yytext);
	return _ident;
}
<NORMAL>{LINE_WRAP}?##[^\n]*\n { BEGIN INITIAL; }
<NORMAL>{LINE_WRAP}?# { BEGIN STRING; return ECAT; }
<NORMAL>\" { BEGIN STRING; }
<NORMAL>"'" { BEGIN STRING_2; }
<NORMAL>"@/" { BEGIN REGEX; }
<STRING>\" |
<STRING_2>"'" |
<REGEX>"/" {
	yylval.exp = Exp_value(StringBuffer_close());
	BEGIN NORMAL;
	//printf("lex> %d, STRING: \"%s\"\n", yylineno, yylval.exp->u.value->u.string);
	return _str;
}
<STRING>\\\" { StringBuffer_add('"'); }
<STRING_2>\\"'" { StringBuffer_add('\''); }
<REGEX>"\\/" { StringBuffer_add('/'); }
<STRING,STRING_2>\\t { StringBuffer_add('\t'); }
<STRING,STRING_2>\\n { StringBuffer_add('\n'); }
<STRING,STRING_2>\\\\ { StringBuffer_add('\\'); }
<STRING>\\# { StringBuffer_add('#'); }
<STRING>#{LINE_WRAP}# {}
<STRING>#{LINE_WRAP}? {
	yylval.exp = Exp_value(StringBuffer_close());
	BEGIN NORMAL;
	return _estr;
}
<STRING,STRING_2>\n { StringBuffer_add('\n'); }
<STRING,STRING_2,REGEX>. { StringBuffer_add(yytext[0]); }
<INITIAL,COMMENT,NORMAL><<EOF>> {
	//printf("lex> %d, EOF: (defaultTab, curTab) = (%d, %d)\n", yylineno, defaultTab, curTab);
	if (defaultTab < preNTab) {
		--preNTab;
		//printf("lex> %d, EOF: '}'\n", yylineno);
		return RC;
	} else {
		BEGIN NEXT_FILE;
		return COMMA;
	}
}
<NEXT_FILE>[^<<EOF>>] { yyless(0); BEGIN INITIAL; }
<NORMAL>. { yyerror("Mystery characher (#%d, \"%c\").\n", yylineno, *yytext); }

%%
