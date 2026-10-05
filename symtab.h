#ifndef SYMTAB_H
#define SYMTAB_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

typedef struct Sym {
	int identifier;
	int symbolType; /* CONST, VAR, ARG, FUNCTION or PROCEDURE */
	Type type;      /* for functions: the return type */
	int isRef;      /* VAR parameter: passed as a pointer */
	int paramCount; /* functions and procedures only, -1 otherwise */
	struct Sym
		*params; /* functions and procedures: their parameters, in order */
	int scope;
} Symbol;

typedef struct SymStk {
	Symbol *array;
	int top;
	int size;
} SymbolStack;

typedef struct SymTab {
	SymbolStack *array;
	int idx;
	int size;
} SymbolTable;

Symbol newSymbol(void);

void initSymbolTable(void);
SymbolTable *getTable(void);
Symbol getSymbolByIdentifier(int identifier);
Symbol getSymbolByIndex(int index);
int addSymbol(Symbol symbol);
void freeTable(void);

void enterScope(void);
void exitScope(void);

#endif
