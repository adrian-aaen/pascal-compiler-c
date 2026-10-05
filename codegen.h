#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "symtab.h"
#include "types.h"

/* Generates C as three-address code: every operation is assigned to its own
 * temporary (_t<n>), and control flow uses labels and gotos. Identifiers are
 * resolved through the symbol table, so they must be declared first. */

void initCodegen(FILE *out);

/* Program structure */
void genProgramStart(void);
void genBlankLine(void);
void genMainStart(void);
void genMainEnd(void);

/* Declarations */
void genConst(int identifier, Constant constant);
void genVars(int *identifiers, int count, Type type);
void genFuncHead(Symbol func);
void genFuncEnd(void);

/* Control flow */
int newLabels(int count);
void genLabel(int label);
void genJump(int label);
void genCondJump(Value cond, int label);

/* Statements */
void genAssign(Value l, Value r);
void genVoidFuncCall(Symbol func, Value *args, int argCount);
void genStdoutWrite(Value *values, int count);
void genStdinRead(Value *values, int count);

/* Expressions */
Value identifierValue(int identifier);
Value genLoad(int type, double value);
Value genRhsFuncCall(Symbol func, Value *args, int argCount);
Value genUnaryOpAssign(char *action, Value v);
Value genOpAssign(Value l, char *op, Value r);
Value genBoolAtom(Value l, int relop, Value r);
Value genArrayIndexOp(Value array, Value pasIndex);
Value genArraySlice(Value array, Value pasStart);

#endif
