#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "parser.h"
#include "struct_functions.h"
#include <time.h>
#include <string.h>
#include <assert.h>
int main () {
	int command_res = 1;
	int count_hid;
	int rows;
	int cols;
	MODE mode = INIT;
	MoveList* curr_move = create_move_list (NULL,NULL);
	int mark_errors = 1;
	Num*** board = NULL;
	srand(time(NULL));
	
	printf("Sudoku\n------\n");
	command_res = get_command(&board,&rows,&cols, &count_hid, &mode, &mark_errors, &curr_move);
	while (command_res > 1) {
		command_res = get_command(&board,&rows, &cols, &count_hid, &mode, &mark_errors, &curr_move);
	}
	return 0;
}

