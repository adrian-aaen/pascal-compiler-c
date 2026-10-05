#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "parser.tab.h"
#include "strtab.h"

#define getIdentifier(id) getString(id)
#define elems(t) ((t.arrayEnd + 1) - t.arrayStart)

static FILE *output;
static int indent = 0;
static int labelIndex = 0;
static int tempVarIndex = 0;
static Symbol currentFunc = {.identifier = -1};
static int returnTemp = -1;

void initCodegen(FILE *out) { output = out; }

static void printIndent() {
	for (int i = 0; i < indent; i++)
		fprintf(output, "  ");
}

static void printType(int type) {
	fprintf(output, "%s ", type == INTEGER ? "int" : "double");
}

static void printOperand(Value v) {
	if (v.isName)
		fprintf(output, "%s", getIdentifier(v.id));
	else
		fprintf(output, "_t%d", v.id);
}

static int isFunction(Symbol s) {
	return s.type.base == INTEGER || s.type.base == REAL;
}

// ---- Program structure -----------------------------------------------

void genProgramStart() { fprintf(output, "#include <stdio.h>\n\n"); }

void genBlankLine() { fprintf(output, "\n"); }

void genMainStart() {
	fprintf(output, "int main() {\n");
	indent++;
}

void genMainEnd() {
	printIndent();
	fprintf(output, "return 0;\n}\n");
	indent--;
}

// ---- Declarations ----------------------------------------------------

void genConst(int identifier, Constant constant) {
	printIndent();
	printType(constant.type);
	fprintf(output, "%s = ", getIdentifier(identifier));
	if (constant.type == INTEGER)
		fprintf(output, "%d;\n", (int)constant.value);
	else
		fprintf(output, "%lf;\n", constant.value);
}

void genVars(int *identifiers, int count, Type type) {
	printIndent();
	printType(type.base);
	for (int i = 0; i < count; i++) {
		if (i)
			fprintf(output, ", ");
		fprintf(output, "%s", getIdentifier(identifiers[i]));
		if (type.isArray) {
			fprintf(output, "[%d]", elems(type));
		}
	}
	fprintf(output, ";\n");
}

static void genParams(Symbol func) {
	for (int i = 0; i < func.paramCount; i++) {
		Symbol p = func.params[i];
		if (i)
			fprintf(output, ", ");
		printType(p.type.base);
		if (p.isRef || p.type.isArray)
			fprintf(output, "*");
		fprintf(output, "%s", getIdentifier(p.identifier));
	}
}

static void genArrayParamCopies(Symbol func) {
	for (int i = 0; i < func.paramCount; i++) {
		Symbol p = func.params[i];
		if (!p.type.isArray || p.isRef)
			continue;
		char *name = getIdentifier(p.identifier);
		int size = elems(p.type);
		printIndent();
		printType(p.type.base);
		fprintf(output, "%s_in[%d];\n", name, size);
		printIndent();
		fprintf(output, "for (int _k = 0; _k < %d; _k++) %s_in[_k] = %s[_k];\n",
		        size, name, name);
		printIndent();
		fprintf(output, "%s = %s_in;\n", name, name);
	}
}

void genFuncHead(Symbol func) {
	currentFunc = func;

	if (func.type.base == INTEGER)
		fprintf(output, "int ");
	if (func.type.base == REAL)
		fprintf(output, "double ");
	if (!isFunction(func))
		fprintf(output, "void ");

	fprintf(output, "%s(", getIdentifier(func.identifier));
	genParams(func);
	fprintf(output, ") {\n");
	indent++;
	genArrayParamCopies(func);

	if (isFunction(func)) {
		returnTemp = genLoad(func.type.base, 0).id;
	}
}

void genFuncEnd() {
	if (isFunction(currentFunc)) {
		printIndent();
		fprintf(output, "return _t%d;\n", returnTemp);
	}
	indent--;
	fprintf(output, "}\n\n");
	currentFunc.identifier = -1;
}

// ---- Control flow ----------------------------------------------------
int newLabels(int count) {
	int first = labelIndex;
	labelIndex += count;
	return first;
}

void genLabel(int label) { fprintf(output, "lbl%d: ;\n", label); }

void genJump(int label) {
	printIndent();
	fprintf(output, "goto lbl%d;\n", label);
}

void genCondJump(Value cond, int label) {
	printIndent();
	fprintf(output, "if (!_t%d) goto lbl%d;\n", cond.id, label);
}

// ---- Statements ------------------------------------------------------

void genAssign(Value l, Value r) {
	if (r.isRef || r.isArray)
		r = genUnaryOpAssign("*", r);

	printIndent();
	if (l.isRef || l.isArray)
		fprintf(output, "*");
	printOperand(l);
	fprintf(output, " = ");
	printOperand(r);
	fprintf(output, ";\n");
}

static void genFuncCall(Symbol func, Value *args, int argCount) {
	fprintf(output, "%s(", getIdentifier(func.identifier));
	for (int i = 0; i < argCount; i++) {
		if (i)
			fprintf(output, ", ");
		Value a = args[i];
		Symbol p = func.params[i];

		int isRefArg = a.isRef || a.isArray;
		int isRefParam = p.isRef || p.type.isArray;
		if (isRefParam && !isRefArg)
			fprintf(output, "&");
		else if (!isRefParam && isRefArg)
			fprintf(output, "*");

		printOperand(a);
	}
	fprintf(output, ");\n");
}

void genVoidFuncCall(Symbol func, Value *args, int argCount) {
	printIndent();
	genFuncCall(func, args, argCount);
}

static void printFormat(Value v) {
	if (v.type == INTEGER)
		fprintf(output, "%s", "%d");
	else if (v.type == REAL)
		fprintf(output, "%s", "%lf");
}

void genStdoutWrite(Value *values, int count) {
	printIndent();
	fprintf(output, "printf(\"");
	for (int i = 0; i < count; i++) {
		if (i)
			fprintf(output, " ");
		printFormat(values[i]);
	}
	fprintf(output, "\\n\"");
	for (int i = 0; i < count; i++) {
		Value v = values[i];
		fprintf(output, ", ");
		if (v.isRef || v.isArray)
			fprintf(output, "*");
		printOperand(v);
	}
	fprintf(output, ");\n");
}

void genStdinRead(Value *values, int count) {
	printIndent();
	fprintf(output, "scanf(\"");
	for (int i = 0; i < count; i++) {
		if (i)
			fprintf(output, " ");
		if (values[i].type != INTEGER && values[i].type != REAL)
			fprintf(output, "???");
		printFormat(values[i]);
	}
	fprintf(output, "\"");
	for (int i = 0; i < count; i++) {
		Value v = values[i];
		fprintf(output, ", ");
		if (!v.isRef && !v.isArray)
			fprintf(output, "&");
		printOperand(v);
	}
	fprintf(output, ");\n");
}

// ---- Expressions -----------------------------------------------------

Value identifierValue(int identifier) {
	Symbol s = getSymbolByIdentifier(identifier);
	Value v = {0};
	v.type = s.type.base;
	if (s.symbolType == FUNCTION && s.identifier == currentFunc.identifier) {
		v.id = returnTemp;
		return v;
	}
	v.isName = 1;
	v.id = identifier;
	v.isRef = s.isRef;
	v.isArray = s.type.isArray;
	v.arrayStart = s.type.arrayStart;
	return v;
}

Value genLoad(int type, double value) {
	Value v = {0};
	v.type = type;
	v.id = tempVarIndex++;

	printIndent();
	printType(type);
	fprintf(output, "_t%d = ", v.id);
	if (type == INTEGER)
		fprintf(output, "%d;\n", (int)value);
	else
		fprintf(output, "%lf;\n", value);
	return v;
}

Value genRhsFuncCall(Symbol func, Value *args, int argCount) {
	Value v = {0};
	v.type = func.type.base;
	v.id = tempVarIndex++;
	printIndent();
	printType(v.type);
	fprintf(output, "_t%d = ", v.id);
	genFuncCall(func, args, argCount);
	return v;
}

Value genUnaryOpAssign(char *action, Value v) {
	int index = tempVarIndex++;
	printIndent();
	printType(v.type);
	fprintf(output, "_t%d = %s", index, action);
	printOperand(v);
	fprintf(output, ";\n");
	v.id = index;
	v.isName = 0;
	if (strcmp(action, "*") == 0)
		v.isRef = 0;
	return v;
}

Value genOpAssign(Value l, char *op, Value r) {
	int index = tempVarIndex++;
	int divPromotion = l.type == INTEGER && r.type == INTEGER;
	divPromotion = divPromotion && strcmp(op, "/") == 0;
	if (divPromotion)
		l.type = REAL;

	// Arithmetic with a real on either side yields a real
	int isArith = strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
	              strcmp(op, "*") == 0 || strcmp(op, "/") == 0;
	if (isArith && r.type == REAL)
		l.type = REAL;

	if (l.isRef && !l.isArray)
		l = genUnaryOpAssign("*", l);
	if (r.isRef && !r.isArray)
		r = genUnaryOpAssign("*", r);

	printIndent();
	printType(l.type);

	if (l.isRef || r.isRef || l.isArray || r.isArray) {
		l.isRef = 1;
		fprintf(output, "*");
	}
	fprintf(output, "_t%d = ", index);
	if (divPromotion)
		fprintf(output, "(double)");
	if (strcmp(op, "DIV") == 0)
		op = "/";
	printOperand(l);
	fprintf(output, " %s ", op);
	printOperand(r);
	fprintf(output, ";\n");

	l.id = index;
	l.isName = 0;
	return l;
}

Value genBoolAtom(Value l, int relop, Value r) {
	if (relop == RELOPLEQ) {
		Value s = genBoolAtom(l, RELOPLT, r);
		Value t = genBoolAtom(l, RELOPEQ, r);
		return genOpAssign(s, "||", t);
	}
	if (relop == RELOPGEQ) {
		Value s = genBoolAtom(l, RELOPGT, r);
		Value t = genBoolAtom(l, RELOPEQ, r);
		return genOpAssign(s, "||", t);
	}
	char *op = "??";
	if (relop == RELOPEQ)
		op = "==";
	if (relop == RELOPLT)
		op = "<";
	if (relop == RELOPGT)
		op = ">";
	if (relop == RELOPNEQ)
		op = "!=";

	return genOpAssign(l, op, r);
}

Value genArrayIndexOp(Value array, Value pasIndex) {
	Value index = array;
	index.isName = 0;
	index.id =
		genOpAssign(pasIndex, "-", genLoad(INTEGER, array.arrayStart)).id;
	Value v = genOpAssign(array, "+", index);
	v.isName = 0;
	v.isRef = 1;
	v.isArray = 0;
	return v;
}

Value genArraySlice(Value array, Value pasStart) {
	return genArrayIndexOp(array, pasStart);
}
