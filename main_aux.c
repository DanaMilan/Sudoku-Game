#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void print_invalid() {
	printf("ERROR: invalid command\n");
}

void print_no_moves_undo() {
	printf("Error: no moves to undo\n");
}

void print_no_moves_redo() {
	printf("Error: no moves to redo\n");
}

/**
 * print_invalid_range an error message of invalid range according to the function if was called from.
 * @param
 * N - the upper range
 * func - the name of calling function
 */
void print_invalid_range (int N, char* func) {
	if (strcmp(func, "hint") == 0)
		printf("Error: value not in range %d-%d\n",1, N);
	else if (strcmp(func, "set") == 0 || strcmp(func, "gen") == 0)
		printf("Error: value not in range %d-%d\n",0, N);
	else if (strcmp(func, "mark_errors") == 0)
		printf("Error: the value should be 0 or 1\n");
}

void print_contains_error () {
	printf("Error: board contains erroneous values\n");
}

void print_validation_passed() {
	printf("Validation passed: board is solvable\n");
}

void print_validation_failed() {
	printf("Validation failed: board is unsolvable\n");
}

/*
 * print_set_cell - prints that cell on column x and row y was set to z
 * @param
 * x - column
 * y - row
 * z - value of cell
 */
void print_set_cell (int x, int y, int z) {
	printf("Cell <%d,%d> set to %d\n", x+1, y+1, z);
}

void print_fixed () {
	printf("Error: cell is fixed\n");
}

void print_contains_val() {
	printf("Error: cell already contains a value\n");
}

/*
 * print_hint - prints a hint with value dig.
 * @param
 * dig - the value of cell the user requested hint to
 */
void print_hint(int dig) {
	printf("Hint: set cell to %d\n", dig);
}

void print_reset() {
	printf("Board reset\n");
}

void print_err_validation() {
	printf("Error: board validation failed\n");
}

void print_file_err_save() {
	printf ("Error: File cannot be created or modified\n");
}
void print_file_err_solve() {
	printf ("Error: File doesn’t exist or cannot be opened\n");
}
void print_file_err_edit() {
	printf("Error: File cannot be opened\n");
}
/*
 * print_num_sols - prints number of solutions of the board.
 * @param
 * num - number of solutions
 */
void print_num_sols(int num) {
	printf("Number of solutions: %d\n", num);
}

void print_good_board() {
	printf("This is a good board!\n");
}

void print_multi_sols() {
	printf("The puzzle has more than one solution, try to edit it further\n");
}

void print_gen_failed() {
	printf("Error: puzzle generator failed\n");
}
