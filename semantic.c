#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"
#include "semantic.h"

static int warnings = 0;

/* The function or procedure whose body is being parsed, -1 outside one */
static int currentSubprogram = -1;

void semanticError(const char *msg) {
	fprintf(stderr, "Error: %s\n\n", msg);
	printf("ERRORS: %d\n", 1);
	printf("WARNINGS: %d\n", warnings);
	puts("REJECTED");
	exit(EXIT_FAILURE);
}

static void checkAndAddSymbol(Symbol symbol) {
	if (addSymbol(symbol) == 3) {
		semanticError("Identifier already declared in the current scope");
	}
}

static Symbol lookup(int identifier) {
	return getSymbolByIdentifier(identifier);
}

static int isCurrentSubprogram(Symbol symbol) {
	return symbol.symbolType == FUNCTION &&
	       symbol.identifier == currentSubprogram;
}

// ---- Declarations ----------------------------------------------------

Symbol paramSymbol(int identifier, Type type, int isRef) {
	Symbol symbol = newSymbol();
	symbol.identifier = identifier;
	symbol.symbolType = isRef ? VAR : ARG;
	symbol.type = type;
	symbol.isRef = isRef;
	return symbol;
}

void declareConst(int identifier, Constant constant) {
	Symbol symbol = newSymbol();
	symbol.identifier = identifier;
	symbol.symbolType = CONST;
	symbol.type.base = constant.type;
	checkAndAddSymbol(symbol);
}

void declareVars(int *identifiers, int count, Type type) {
	for (int i = 0; i < count; i++) {
		Symbol symbol = newSymbol();
		symbol.identifier = identifiers[i];
		symbol.symbolType = VAR;
		symbol.type = type;
		checkAndAddSymbol(symbol);
	}
}

Symbol declareSubprogram(int identifier, int returnType, Symbol *params,
                         int paramCount) {
	Symbol symbol = newSymbol();
	symbol.identifier = identifier;
	symbol.symbolType = returnType ? FUNCTION : PROCEDURE;
	symbol.type.base = returnType;
	symbol.paramCount = paramCount;
	if (paramCount > 0) {
		// The subprogram keeps its own copy, since the parameters themselves
		// leave the symbol table at the end of its scope.
		symbol.params = malloc(paramCount * sizeof(Symbol));
		assert(symbol.params != NULL);
		memcpy(symbol.params, params, paramCount * sizeof(Symbol));
	}
	checkAndAddSymbol(symbol);

	enterScope();
	for (int i = 0; i < paramCount; i++) {
		checkAndAddSymbol(params[i]);
	}
	currentSubprogram = identifier;
	return symbol;
}

void endSubprogram() {
	exitScope();
	currentSubprogram = -1;
}

// ---- Checks ----------------------------------------------------------

void checkArrayRange(int start, int end) {
	if (start > end) {
		semanticError("Cannot initialize array with range smaller than 1");
	}
}

static void checkUndefined(int identifier) {
	if (lookup(identifier).identifier == -1) {
		semanticError("Variable or const undefined");
	}
}

/* Target of an assignment or readln */
void checkLhs(int identifier) {
	checkUndefined(identifier);
	Symbol symbol = lookup(identifier);
	if (symbol.symbolType == CONST) {
		semanticError("Assigning to const is not allowed");
	}
	if (symbol.symbolType == PROCEDURE) {
		semanticError("Assigning to procedure is not allowed");
	}
	if (symbol.symbolType == FUNCTION && !isCurrentSubprogram(symbol)) {
		semanticError("Assigning to function outside its body is not allowed");
	}
}

/* Identifier read in an expression */
void checkValue(int identifier) {
	checkUndefined(identifier);
	Symbol symbol = lookup(identifier);
	if (symbol.symbolType == PROCEDURE) {
		semanticError("Procedure used as a value");
	}
	if (symbol.symbolType == FUNCTION && !isCurrentSubprogram(symbol)) {
		semanticError("Function called without arguments");
	}
}

/* Identifier indexed with [i] or sliced with [i .. j] */
void checkIndexed(int identifier) {
	checkUndefined(identifier);
	if (!lookup(identifier).type.isArray) {
		semanticError("Cannot index non-array");
	}
}

static void checkUndefinedFuncProc(int identifier) {
	if (lookup(identifier).identifier == -1) {
		semanticError("Function or procedure undefined");
	}
}

static void checkParamCount(int identifier, int count) {
	if (lookup(identifier).paramCount != count) {
		semanticError("Invalid amount of arguments");
	}
}

void checkProcCall(int identifier, int argCount) {
	checkUndefinedFuncProc(identifier);
	checkParamCount(identifier, argCount);
	if (lookup(identifier).symbolType != PROCEDURE) {
		semanticError("Identifier used as procedure");
	}
}

void checkFuncCall(int identifier, int argCount) {
	checkUndefinedFuncProc(identifier);
	if (lookup(identifier).symbolType != FUNCTION) {
		semanticError("Identifier used as function");
	}
	checkParamCount(identifier, argCount);
}

void checkDivMod(Value l, Value r) {
	if (l.type == REAL || r.type == REAL) {
		semanticError("DIV or MOD used on non-integer");
	}
}
