#ifndef TYPES_H
#define TYPES_H

/* A declared type: a basic type, or an array of one. */
typedef struct Type {
	int base; /* INTEGER or REAL; 0 for procedures, which have no type */
	int isArray;
	int arrayStart;
	int arrayEnd;
} Type;

/* A numeric literal in a const declaration. */
typedef struct Constant {
	int type; /* INTEGER or REAL */
	double value;
} Constant;

/* The result of an expression during code generation: either a named C
 * variable or a numbered temporary (_t<id>). */
typedef struct Value {
	int type;   /* INTEGER or REAL */
	int isName; /* 1: the C variable for identifier `id`; 0: temporary _t<id> */
	int id;
	int isRef;      /* a pointer to a single value; dereference to read it */
	int isArray;    /* a whole array, i.e. a pointer to its first element */
	int arrayStart; /* for arrays: the Pascal index of the first element */
} Value;

#endif
