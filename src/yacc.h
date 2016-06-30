/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SRC_YACC_H_INCLUDED
# define YY_YY_SRC_YACC_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _num = 258,
     _str = 259,
     _estr = 260,
     _ident = 261,
     OR = 262,
     AND = 263,
     OR_ASSIGN = 264,
     AND_ASSIGN = 265,
     ASSIGN = 266,
     LT = 267,
     LE = 268,
     EQ = 269,
     GE = 270,
     GT = 271,
     NE = 272,
     LT_ASSIGN = 273,
     LE_ASSIGN = 274,
     EQ_ASSIGN = 275,
     GE_ASSIGN = 276,
     GT_ASSIGN = 277,
     NE_ASSIGN = 278,
     ADD = 279,
     SUB = 280,
     MUL = 281,
     DIV = 282,
     CAT = 283,
     ADD_ASSIGN = 284,
     SUB_ASSIGN = 285,
     MUL_ASSIGN = 286,
     DIV_ASSIGN = 287,
     CAT_ASSIGN = 288,
     NOT = 289,
     AT = 290,
     ME = 291,
     DOT = 292,
     COLON = 293,
     ACCESSOR = 294,
     SEMICOLON = 295,
     COMMA = 296,
     LA = 297,
     RA = 298,
     LP = 299,
     RP = 300,
     LB = 301,
     RB = 302,
     LC = 303,
     RC = 304,
     ECAT = 305,
     TO = 306,
     LESS_TO = 307,
     BY = 308,
     FUNCTION = 309,
     RETURN = 310,
     RETURN_IF = 311,
     RETURN_UNLESS = 312,
     IF = 313,
     UNLESS = 314,
     ELSE = 315,
     SWITCH = 316,
     CASE = 317,
     WHILE = 318,
     UNTIL = 319,
     FOREACH = 320,
     IN = 321,
     IN_OWN = 322,
     BREAK = 323,
     BREAK_IF = 324,
     BREAK_UNLESS = 325,
     CONTINUE = 326,
     CONTINUE_IF = 327,
     CONTINUE_UNLESS = 328,
     NO_ELSE = 329
   };
#endif
/* Tokens.  */
#define _num 258
#define _str 259
#define _estr 260
#define _ident 261
#define OR 262
#define AND 263
#define OR_ASSIGN 264
#define AND_ASSIGN 265
#define ASSIGN 266
#define LT 267
#define LE 268
#define EQ 269
#define GE 270
#define GT 271
#define NE 272
#define LT_ASSIGN 273
#define LE_ASSIGN 274
#define EQ_ASSIGN 275
#define GE_ASSIGN 276
#define GT_ASSIGN 277
#define NE_ASSIGN 278
#define ADD 279
#define SUB 280
#define MUL 281
#define DIV 282
#define CAT 283
#define ADD_ASSIGN 284
#define SUB_ASSIGN 285
#define MUL_ASSIGN 286
#define DIV_ASSIGN 287
#define CAT_ASSIGN 288
#define NOT 289
#define AT 290
#define ME 291
#define DOT 292
#define COLON 293
#define ACCESSOR 294
#define SEMICOLON 295
#define COMMA 296
#define LA 297
#define RA 298
#define LP 299
#define RP 300
#define LB 301
#define RB 302
#define LC 303
#define RC 304
#define ECAT 305
#define TO 306
#define LESS_TO 307
#define BY 308
#define FUNCTION 309
#define RETURN 310
#define RETURN_IF 311
#define RETURN_UNLESS 312
#define IF 313
#define UNLESS 314
#define ELSE 315
#define SWITCH 316
#define CASE 317
#define WHILE 318
#define UNTIL 319
#define FOREACH 320
#define IN 321
#define IN_OWN 322
#define BREAK 323
#define BREAK_IF 324
#define BREAK_UNLESS 325
#define CONTINUE 326
#define CONTINUE_IF 327
#define CONTINUE_UNLESS 328
#define NO_ELSE 329



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 7 "..\\..\\src\\_.y"

	Exp exp;
	ExpType expType;


/* Line 2058 of yacc.c  */
#line 211 "..\\..\\src\\yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_SRC_YACC_H_INCLUDED  */
