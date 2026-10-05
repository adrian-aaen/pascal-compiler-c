CC     = gcc
CFLAGS = -O2 -I. -I$(OBJ)
LIBS   = -lfl -lm

B    = build
OBJ  = $(B)/obj
PROG = $(B)/program

OBJS = $(OBJ)/parser.o $(OBJ)/lexer.o $(OBJ)/strtab.o $(OBJ)/symtab.o \
       $(OBJ)/semantic.o $(OBJ)/codegen.o

.PHONY: all run compile parser lexer strtab symtab semantic codegen clean

# ---- Main targets ----------------------------------------------------

all: $(B)/compiler

# Usage: make compile SRC=valid-gcd.pas
SRC ?= test.pas

compile: $(B)/compiler
	rm -rf $(PROG)
	mkdir -p $(PROG)
	cp $(SRC) $(PROG)/input.pas
	./$(B)/compiler $(SRC) $(PROG)/output.c

run: $(PROG)/output.c
	$(CC) -O2 -pedantic -Wall -o $(PROG)/output $<
	./$(PROG)/output

clean:
	rm -rf $(B) *~

# Shortcuts for building one piece at a time
parser: $(OBJ)/parser.o
lexer:  $(OBJ)/lexer.o
strtab: $(OBJ)/strtab.o
symtab: $(OBJ)/symtab.o
semantic: $(OBJ)/semantic.o
codegen: $(OBJ)/codegen.o

# ---- Build rules -----------------------------------------------------

$(B)/compiler: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Bison produces both files in one run
$(OBJ)/parser.c $(OBJ)/parser.tab.h &: parser.y | $(OBJ)
	bison -d -o $(OBJ)/parser.c --defines=$(OBJ)/parser.tab.h $<

$(OBJ)/lexer.c: lexer.fl | $(OBJ)
	flex -o $@ $<

# Compile generated sources (in build/obj/) and hand-written ones (here)
$(OBJ)/%.o: $(OBJ)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: %.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $@

# ---- Header dependencies ---------------------------------------------

$(OBJ)/parser.o:   strtab.h types.h symtab.h semantic.h codegen.h
$(OBJ)/lexer.o:    strtab.h types.h $(OBJ)/parser.tab.h
$(OBJ)/strtab.o:   strtab.h
$(OBJ)/symtab.o:   symtab.h types.h
$(OBJ)/semantic.o: semantic.h symtab.h types.h $(OBJ)/parser.tab.h
$(OBJ)/codegen.o:  codegen.h symtab.h types.h strtab.h $(OBJ)/parser.tab.h
