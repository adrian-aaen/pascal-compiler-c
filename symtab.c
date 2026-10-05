#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"

SymbolTable table;
int scope = 0;

// --- Symbol ---

Symbol newSymbol() {
	Symbol symbol;
	symbol.identifier = -1;
	symbol.symbolType = -1;
	symbol.type.base = -1;
	symbol.type.isArray = 0;
	symbol.type.arrayStart = -1;
	symbol.type.arrayEnd = -1;
	symbol.isRef = 0;
	symbol.paramCount = -1;
	symbol.params = NULL;
	symbol.scope = -1;
	return symbol;
}

// --- SymbolStack ---

SymbolStack newSymbolStack() {
	SymbolStack stack;
	stack.array = malloc(sizeof(Symbol));
	assert(stack.array != NULL);
	stack.top = 0;
	stack.size = 1;
	return stack;
}

void doubleSymbolStackSize(SymbolStack *stp) {
	int newSize = 2 * stp->size;
	stp->array = realloc(stp->array, newSize * sizeof(Symbol));
	assert(stp->array != NULL);
	stp->size = newSize;
}

int isEmptySymbolStack(SymbolStack stack) { return (stack.top == 0); }

void stackEmptyError() {
	printf("stack empty\n");
	abort();
}

void push(Symbol symbol, SymbolStack *stp) {
	if (stp->top == stp->size) {
		doubleSymbolStackSize(stp);
	}
	stp->array[stp->top] = symbol;
	stp->top++;
}

Symbol peek(SymbolStack *stp) {
	if (isEmptySymbolStack(*stp)) {
		stackEmptyError();
	}
	return stp->array[stp->top - 1];
}

Symbol pop(SymbolStack *stp) {
	Symbol result = peek(stp);
	stp->top--;
	return result;
}

void freeSymbolStack(SymbolStack stack) { free(stack.array); }

// --- SymbolTable ---

void initSymbolTable() {
	table.size = 1;
	table.array = malloc(table.size * sizeof(SymbolStack));
	assert(table.array != NULL);
	table.array[0] = newSymbolStack();
	table.idx = 0;
}

void doubleTableSize() {
	int newSize = table.size * 2;
	table.array = realloc(table.array, newSize * sizeof(SymbolStack));
	assert(table.array != NULL);
	for (int i = table.size; i < newSize; i++) {
		table.array[i] = newSymbolStack();
	}
	table.size = newSize;
}

void symbolTableUnintializedError() {
	printf("Symbol Table Uninitialized\n");
	abort();
}

SymbolTable *getTable() { return &table; }

Symbol getSymbolByIdentifier(int identifier) {
	for (int i = 0; i < table.idx; i++) {
		SymbolStack *stp = &table.array[i];
		Symbol symbol = peek(stp);
		if (symbol.identifier == identifier) {
			return symbol;
		}
	}
	return newSymbol();
}

Symbol getSymbolByIndex(int index) {
	if (index >= table.idx || index < 0) {
		return newSymbol();
	}
	return peek(&table.array[index]);
}

static int getSymbolIndex(Symbol symbol) {
	for (int i = 0; i < table.idx; i++) {
		SymbolStack *stp = &table.array[i];
		Symbol symbol_i = peek(stp);
		if (symbol_i.identifier == symbol.identifier) {
			return i;
		}
	}
	return -1;
}

int addSymbol(Symbol symbol) {
	/* returnval:
	 * 1 =	added to existing slot
	 * 2 =	added to new slot
	 * 3 =	added to existing slot, but with duplicate scope
	 * 		(error should be thrown)
	 */
	int result = 1;
	symbol.scope = scope;
	int index = getSymbolIndex(symbol);
	if (index == -1) {
		result = 2;
		if (table.idx == table.size) {
			doubleTableSize();
		}
		index = table.idx++;
	} else if (peek(&table.array[index]).scope == scope) {
		result = 3;
	}
	push(symbol, &table.array[index]);
	return result;
}

void fillEmptySlots() {
	for (int i = 0; i < table.idx; i++) {
		SymbolStack *stp = &table.array[i];
		if (!isEmptySymbolStack(*stp)) {
			continue;
		}
		// stp points to empty slot
		// Get next non-empty slot
		int j;
		for (j = i + 1; j < table.idx; j++) {
			if (!isEmptySymbolStack(table.array[j])) {
				break;
			}
		}
		// Swap stp slot with non-empty slot
		if (j < table.idx) {
			SymbolStack *candidateStp = &table.array[j];
			while (!isEmptySymbolStack(*candidateStp)) {
				push(pop(candidateStp), stp);
			}
		}
	}
	// Reset table.idx to earliest empty slot
	for (int i = 0; i < table.idx; i++) {
		if (isEmptySymbolStack(table.array[i])) {
			table.idx = i;
			break;
		}
	}
}

void removeSymbolsOutOfScope() {
	for (int i = 0; i < table.idx; i++) {
		SymbolStack *stp = &table.array[i];
		while (!isEmptySymbolStack(*stp)) {
			if (peek(stp).scope <= scope) {
				break;
			}
			pop(stp);
		}
	}
	fillEmptySlots();
}

void enterScope() { scope++; }

void exitScope() {
	scope--;
	removeSymbolsOutOfScope();
}

void freeTable() {
	for (int i = 0; i < table.size; i++) {
		SymbolStack *stp = &table.array[i];
		while (!isEmptySymbolStack(*stp)) {
			free(pop(stp).params);
		}
		freeSymbolStack(*stp);
	}
	free(table.array);
}
