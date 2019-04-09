CC = gcc
OBJS = main.o main_aux.o game.o solver.o parser.o struct_functions.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@
all: $(OBJS)
	$(CC) $(COMP_FLAG) $(OBJS) $(GUROBI_LIB) -o $(EXEC)
main.o: main.c main_aux.h structs.h parser.h struct_functions.h
	$(CC) $(COMP_FLAG) -c $*.c
main_aux.o: main_aux.c main_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
solver.o: solver.c solver.h structs.h game.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
game.o: game.c game.h structs.h solver.h struct_functions.h
	$(CC) $(COMP_FLAG) -c $*.c
parser.o: parser.c parser.h main_aux.h structs.h game.h solver.h 
	$(CC) $(COMP_FLAG) -c $*.c
struct_functions.o: struct_functions.c struct_functions.h structs.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
