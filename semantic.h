#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symtab.h"
#include "types.h"

/* Semantic analysis. Declarations go into the symbol table (symtab.c), and
 * each check runs before the code for its construct is generated. The first
 * error rejects the program and exits. */

void semanticError(const char *msg);

/* Declarations */
Symbol paramSymbol(int identifier, Type type, int isRef);
void declareConst(int identifier, Constant constant);
void declareVars(int *identifiers, int count, Type type);
/* returnType is 0 for procedures. Enters the subprogram's scope. */
Symbol declareSubprogram(int identifier, int returnType, Symbol *params,
                         int paramCount);
void endSubprogram(void);

/* Checks */
void checkArrayRange(int start, int end);
void checkLhs(int identifier);
void checkValue(int identifier);
void checkIndexed(int identifier);
void checkProcCall(int identifier, int argCount);
void checkFuncCall(int identifier, int argCount);
void checkDivMod(Value l, Value r);

#endif
