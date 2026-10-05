%code requires {
	#include "types.h"
}

%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "strtab.h"
	#include "symtab.h"
	#include "semantic.h"
	#include "codegen.h"
	#include "parser.tab.h"

	void yyerror(char *msg);
	extern int yylex(void);
	extern char *yytext;
	extern void showErrorLine();
	extern void initLexer(FILE *f);
	extern void finalizeLexer();

	int identifiers[100];
	int identifierCount = 0;

	Symbol params[100];
	int paramCount = 0;

	Value values[100];
	int valueCount = 0;

	void pushValue(Value v) {
		values[valueCount++] = v;
	}

	Value *popValues(int count) {
		valueCount -= count;
		return &values[valueCount];
	}

	void addParams(Type type, int isRef) {
		for (int i = 0; i < identifierCount; i++) {
			params[paramCount++] = paramSymbol(identifiers[i], type, isRef);
		}
		identifierCount = 0;
	}

	Type basicType(int base) {
		Type t = { .base = base };
		return t;
	}

	Type arrayType(int start, int end, int base) {
		Type t = { .base = base, .isArray = 1, .arrayStart = start, .arrayEnd = end };
		return t;
	}
%}

%token	PROGRAM CONST IDENTIFIER VAR ARG ARRAY RANGE INTNUMBER REALNUMBER OF 
		FUNCTION PROCEDURE BEGINTOK ENDTOK ASSIGN IF THEN ELSE WHILE DO
		RELOPLT RELOPLEQ RELOPEQ RELOPNEQ RELOPGEQ RELOPGT INTEGER REAL
		AND OR NOT DIV MOD SKIP READLN WRITELN

%left '+' '-'
%left '*' '/' DIV MOD
%left OR
%left AND
%left NOT

%union {
	int ival;
	double dval;
	Type type;
	Constant constant;
	Value value;
}

%type <ival>		Identifier BasicType RangeStart RangeEnd Relop ArithExprList LhsList ConstDecl
%type <type>		TypeSpec
%type <constant>	NumericValue
%type <value>		Lhs ArithExpr BoolAtom Guard

%%

program				: PROGRAM Identifier ';' 	{ initSymbolTable(); genProgramStart(); }
					ConstDecl					{ if ($5) genBlankLine(); }
					VarDecl						{ genBlankLine(); }
					FuncProcDecl				{ genMainStart(); }
					CompoundStatement			{ genMainEnd(); }
					'.'
					;

ConstDecl			: ConstDecl CONST Identifier RELOPEQ NumericValue ';'
					{ declareConst($3, $5); genConst($3, $5); $$ = 1; }
					| %empty { $$ = 0; }
					;

NumericValue		: INTNUMBER		{ $$ = (Constant){ INTEGER, yylval.ival }; }
					| REALNUMBER	{ $$ = (Constant){ REAL, yylval.dval }; }
					;

VarDecl				: VarDecl VAR IdentifierList ':' TypeSpec ';'
					{ declareVars(identifiers, identifierCount, $5); genVars(identifiers, identifierCount, $5); identifierCount = 0; }
					| %empty
					;

IdentifierList		: Identifier					{ identifiers[identifierCount++] = $1; }
					| IdentifierList ',' Identifier	{ identifiers[identifierCount++] = $3; }
					;

TypeSpec			: BasicType { $$ = basicType($1); }
					| ARRAY '[' RangeStart RANGE RangeEnd ']' OF BasicType
					{ checkArrayRange($3, $5); $$ = arrayType($3, $5, $8); }
					;

RangeStart			: INTNUMBER { $$ = yylval.ival; }
					;

RangeEnd			: INTNUMBER { $$ = yylval.ival; }
					;

BasicType			: INTEGER	{ $$ = INTEGER; }
					| REAL		{ $$ = REAL; }
					;

FuncProcDecl		: FuncProcDecl SubProgDecl ';'
					| %empty
					;

SubProgDecl			: SubProgHeading VarDecl { genBlankLine(); }
					  CompoundStatement { genFuncEnd(); endSubprogram(); }
					;

SubProgHeading		: FUNCTION Identifier Parameters ':' BasicType ';'
					{ genFuncHead(declareSubprogram($2, $5, params, paramCount)); paramCount = 0; }
					| PROCEDURE Identifier PossibleParameters ';'
					{ genFuncHead(declareSubprogram($2, 0, params, paramCount)); paramCount = 0; }
					;

PossibleParameters	: Parameters
					| %empty
					;

Parameters			: '(' ParameterList ')'
					;

ParameterList		: ParamList
					| ParameterList ';' ParamList
					;

ParamList			: VAR IdentifierList ':' TypeSpec	{ addParams($4, 1); }
					| IdentifierList ':' TypeSpec		{ addParams($3, 0); }
					;

CompoundStatement	: BEGINTOK OptionalStatements ENDTOK
					;

OptionalStatements	: StatementList
					| %empty
					;

StatementList		: Statement
					| StatementList ';' Statement
					;

Statement			: Lhs ASSIGN ArithExpr	{ genAssign($1, $3); }
					| SKIP
					| ProcedureCall
					| CompoundStatement
					| IF Guard THEN { $<ival>$ = newLabels(2); genCondJump($2, $<ival>$); }
					  Statement { genJump($<ival>4 + 1); genLabel($<ival>4); }
					  ELSE Statement { genLabel($<ival>4 + 1); }
					| WHILE { $<ival>$ = newLabels(2); genLabel($<ival>$); }
					  Guard { genCondJump($3, $<ival>2 + 1); }
					  DO Statement { genJump($<ival>2); genLabel($<ival>2 + 1); }
					;

LhsList				: Lhs				{ pushValue($1); $$ = 1; }
					| LhsList ',' Lhs	{ pushValue($3); $$ = $1 + 1; }

Lhs					: Identifier					{ checkLhs($1); $$ = identifierValue($1); }
					| Identifier '[' ArithExpr ']'
					{ checkLhs($1); checkIndexed($1); $$ = genArrayIndexOp(identifierValue($1), $3); }
					;

ProcedureCall		: Identifier
					{ checkProcCall($1, 0); genVoidFuncCall(getSymbolByIdentifier($1), NULL, 0); }
					| Identifier '(' ArithExprList ')'
					{ checkProcCall($1, $3); genVoidFuncCall(getSymbolByIdentifier($1), popValues($3), $3); }
					| READLN '(' LhsList ')'			{ genStdinRead(popValues($3), $3); }
					| WRITELN '(' ArithExprList ')'		{ genStdoutWrite(popValues($3), $3); }
					;

Guard				: BoolAtom			{ $$ = $1; }
					| NOT Guard			{ $$ = genUnaryOpAssign("!", $2); }
					| Guard OR Guard	{ $$ = genOpAssign($1, "||", $3); }
					| Guard AND Guard	{ $$ = genOpAssign($1, "&&", $3); }
					| '(' Guard ')'		{ $$ = $2; }
					;

BoolAtom			: ArithExpr Relop ArithExpr	{ $$ = genBoolAtom($1, $2, $3); }
					;

Relop				: RELOPLT	{ $$ = RELOPLT;		}
					| RELOPLEQ	{ $$ = RELOPLEQ;	}
					| RELOPEQ	{ $$ = RELOPEQ;		}
					| RELOPNEQ	{ $$ = RELOPNEQ;	}
					| RELOPGEQ	{ $$ = RELOPGEQ;	}
					| RELOPGT	{ $$ = RELOPGT;		}
					;

ArithExprList		: ArithExpr						{ pushValue($1); $$ = 1; }
					| ArithExprList ',' ArithExpr	{ pushValue($3); $$ = $1 + 1; }
					;

ArithExpr			: Identifier { checkValue($1); $$ = identifierValue($1); }
					| Identifier '[' ArithExpr ']'
					{ checkIndexed($1); $$ = genUnaryOpAssign("*", genArrayIndexOp(identifierValue($1), $3)); }
					| Identifier '[' ArithExpr RANGE ArithExpr ']'
					{ checkIndexed($1); $$ = genArraySlice(identifierValue($1), $3); }
					| Identifier '(' ArithExprList ')'
					{ checkFuncCall($1, $3); $$ = genRhsFuncCall(getSymbolByIdentifier($1), popValues($3), $3); }
					| INTNUMBER					{ $$ = genLoad(INTEGER, yylval.ival);	}
					| REALNUMBER				{ $$ = genLoad(REAL, yylval.dval);		}
					| ArithExpr '+' ArithExpr	{ $$ = genOpAssign($1, "+", $3);		}
					| ArithExpr '-' ArithExpr	{ $$ = genOpAssign($1, "-", $3);		}
					| ArithExpr '*' ArithExpr	{ $$ = genOpAssign($1, "*", $3);		}
					| ArithExpr '/' ArithExpr	{ $$ = genOpAssign($1, "/", $3);		}
					| ArithExpr DIV ArithExpr	{ checkDivMod($1, $3); $$ = genOpAssign($1, "DIV", $3); }
					| ArithExpr MOD ArithExpr	{ checkDivMod($1, $3); $$ = genOpAssign($1, "%", $3); }
					| '-' ArithExpr				{ $$ = genUnaryOpAssign("-", $2);		}
					| '(' ArithExpr ')'			{ $$ = $2;								}
					;

Identifier			: IDENTIFIER { $$ = yylval.ival; }
					;

%%


void printToken(int token, FILE *f) {
	/* single character tokens */
	if (token < 256) {
		if (token < 33) {
			/* non-printable character */
			fprintf(f, "chr(%d)", token);
		} else {
			fprintf(f, "'%c'", token);
		}
		return;
	}
	/* standard tokens (>255) */
	switch (token) {
		case PROGRAM	: fprintf(f, "PROGRAM"); break;
		case CONST		: fprintf(f, "CONST"); break;
		case IDENTIFIER	: fprintf(f, "identifier<%s>", yytext); break;
		case VAR		: fprintf(f, "VAR"); break;
		case ARRAY		: fprintf(f, "ARRAY"); break;
		case RANGE		: fprintf(f, ".."); break;
		case INTNUMBER	: fprintf(f, "Integer<%d>", yylval.ival); break;
		case REALNUMBER	: fprintf(f, "Real<%lf>", yylval.dval); break;
		case OF			: fprintf(f, "OF"); break;
		case INTEGER	: fprintf(f, "INTEGER"); break;
		case REAL		: fprintf(f, "REAL"); break;
		case FUNCTION	: fprintf(f, "FUNCTION"); break;
		case PROCEDURE	: fprintf(f, "PROCEDURE"); break;
		case BEGINTOK	: fprintf(f, "BEGIN"); break;
		case ENDTOK		: fprintf(f, "END"); break;
		case ASSIGN		: fprintf(f, ":="); break;
		case IF			: fprintf(f, "IF"); break;
		case THEN		: fprintf(f, "THEN"); break;
		case ELSE		: fprintf(f, "ELSE"); break;
		case WHILE		: fprintf(f, "WHILE"); break;
		case DO			: fprintf(f, "DO"); break;
		case SKIP		: fprintf(f, "SKIP"); break;
		case READLN		: fprintf(f, "READLN"); break;
		case WRITELN	: fprintf(f, "WRITELN"); break;
	}
}

void yyerror (char *msg) {
	showErrorLine();
	fprintf(stderr, "%s (detected at token=", msg);
	printToken(yychar, stderr);
	fprintf(stderr, ").\n");

	printf("ERRORS: 1\nWARNINGS: 0\nREJECTED\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [pasfile] [outfile]\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *input = (strcmp(argv[1], "-") == 0) ? stdin : fopen(argv[1], "r");
	if(input == NULL) {
		fprintf(stderr, "Failed to open input file!\n");
		exit(EXIT_FAILURE);
	}
	FILE *output = (strcmp(argv[2], "-") == 0) ? stdout : fopen(argv[2], "w");
	if(output == NULL) {
		fprintf(stderr, "Failed to initialize output file!\n");
		exit(EXIT_FAILURE);
	}

	initCodegen(output);
	initLexer(input);
	int result = yyparse();
	finalizeLexer();

	//showStringTable();

	fclose(input);
	fclose(output);

	freeTable();
	return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
