#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "struct_functions.h"
#include "solver.h"
#include "main_aux.h"
#include "parser.h"
#define DEF_ROWS 3
#define DEF_COLS 3


/**
 * write_num writes the number in a required cell to a given file.
 * @param
 * fp - pointer to file
 * board - game's board
 * row - row of cell
 * col - column of cell
 * mode - game's mode
 */
void write_num (FILE* fp, Num*** board, int row, int col, MODE mode) {
	int dig = board[row][col]->num;
	if (mode == EDIT) {
		if ( dig != 0)
			fprintf(fp,"%d.",dig);
		else
			fprintf(fp, "0");
	}
	else if (mode == SOLVE) {
		if (board[row][col]->status == FIXED)
			fprintf(fp,"%d.",dig);
		else
			fprintf(fp,"%d",dig);
	}
}

/**
 * print_num prints the numbers according to their status (HIDDEN|SHOWN|FIXED|ERRONEOUS) and according to game's mode.
 *
 * @param
 * n - pointer to struct Num, which represents the number that appears in the cell.
 * mode - game's mode
 * mark_errors - indicates whether to mark erros
 */
void print_num (Num* n, MODE mode, int mark_errors) {
	STAT stat = n -> status;
	int number = n -> num;
	switch (stat) {
	case HIDDEN: /*value is 0*/
		printf("    ");
		break;
	case SHOWN:
		printf(" %2d ", number);
		break;
	case FIXED:
		if (mode == SOLVE)
			printf(" %2d.", number);
		else
			printf(" %2d ", number);
		break;
	case ERRONEOUS:
		if (mode == EDIT || mark_errors)
			printf(" %2d*", number);
		else
			printf(" %2d ", number);
	}
}


/**
* clear_board - function clears cells from the board by setting them to 0 and changing their status to HIDDEN.
* @params:
* board - the Sudoku board
* N - number of digits in one block
* to_clear - number of cells to clear
*/
void clear_board(Num*** board, int N, int to_clear){
	int row = 0;
	int col = 0;
	int i; int j;
	int count = 0;
	if (to_clear == N*N) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				board[i][j]-> num = 0;
				board[i][j]-> status = HIDDEN;
			}
		}
	}
	else {	
		while (count < to_clear) {
			col = rand() % N;
			row = rand() % N;
			if (board[row][col]->status != HIDDEN) {
				board[row][col]-> num = 0;
				board[row][col]->status = HIDDEN;
				count++;
			}
		}
	}
}


/**
 * print_dashes - prints 4N+m+1 dashes.
 *
 */
void print_dashes(int m, int n) {
	int i;
	for (i=0; i<4*n*m+m+1; i++) {
		printf("-");
	}
	printf("\n");
}

/**
 * print_board prints the Sudoku board by using print_dashes and print_num functions.
 *
 * @param 
 * m - number of rows in one block
 * n - number of columns in one block
 * board - the Sudoku board
 * mode - game's mode
 * mark_errors - indicates whether to mark_errors.
 * @return
 * 2 - if board printed sucessfully
 * 3 - otherwise
 * 
 */
int print_board(Num*** board, int m, int n, MODE mode, int mark_errors) {
	int row;
	int col;
	if(mode == INIT){
		print_invalid();
		return 3;
	}
	for (row = 0; row < n*m; row++){
		if (row % m == 0){
			print_dashes(m,n);
		}
		for (col = 0; col < n*m; col++) {
			if (col % n == 0)
				printf("%s","|");
			print_num(board[row][col], mode, mark_errors);
		}
		printf("%c\n",'|');
	}
	print_dashes(m,n);
	return 2;
}

/**
* fill_k_cells - fill k random cells with legal random values
* @param
* board - the Sudoku board
* k - number of cells to choose
* m - number of rows in one block
* n - number of columns in one block
* @return
* 1 - if k cells were filled with legal values successfully
* 0 - otherwise
*/
int fill_k_cells(Num*** board, int k, int m, int n){
	int row = 0; int col = 0; int count_filled = 0; int count_checked;
	int N = n*m;
	int dig;
	int* checked_nums;
	while (count_filled < k){
		col = rand() % N;
		row = rand() % N;
		if(board[row][col]-> num == 0){ /*if the chosen cell is empty */
			checked_nums = calloc(N,sizeof(int));
			count_checked = 0;
			dig = 1 + (rand() % N); /*choose a value*/
			while (count_checked < N) { /*while there are still values unchecked*/
				if (checked_nums[dig-1] == 0) { /*if we haven't checked this value*/
					if (validate_dig(dig,row,col,m,n,N,board,0,1)) {  /*if it's valid, fill it*/
						board[row][col]-> num = dig;
						board[row][col]-> status = SHOWN;
						count_filled++;
						break; /*cell filled successfully*/
					}
					else { /*value is not valid*/
						checked_nums[dig-1]++;
						count_checked++;
					}
				} /*choose a new value*/
				dig = 1 + (rand() % N);
			}
			if (count_checked == N){ /*found cell with no legal value*/
				free(checked_nums);
				return 0;
			}
			free(checked_nums);
		}
	}
	return 1;
}

/**
 * create_empty_board - creates a new empty board of the given size.
 * @param
 * m - number of rows in one block
 * n - number of columns in one block
 */
Num*** create_empty_board(int m, int n) {
	int N = n*m;
	int i; int j;
	Num*** board = calloc(N,sizeof(Num**));
	for (i=0; i<N; i++) {
		board[i] = calloc(N,sizeof(Num*));
		for (j = 0; j < N; j++) {
			board[i][j] = create_num(0);
		}
	}
	return board;
}

/**
 * parse_row goes through the given row and looks for a number dig.
 * if found, the function validates it. if it's valid, it changes its status to SHOWN.
 * @param
 * dig - the number to search
 * row - given row
 * N - number of rows on board
 * board - the game board
 * m - number of rows in a block
 * n - number of columns in a block
 */
void parse_row (int dig, int row, int N, Num*** board, int m, int n) {
	int col;
	for (col = 0; col < N; col++){
		if (board[row][col]->num == dig && board[row][col]->status != FIXED) {
			board[row][col]->num = 0;
			if (validate_dig(dig,row,col,n,m,n*m, board,0,1))
				board[row][col]->status = SHOWN;
			board[row][col]->num = dig;
		}
	}
}

/**
 * parse_col goes through the given column and looks for a number dig.
 * if found, the function validates it. if it's valid, it changes its status to SHOWN.
 * @param
 * dig - the number to search
 * col - given col
 * N - number of rows on board
 * board - the game board
 * m - number of rows in a block
 * n - number of columns in a block
 */
void parse_col(int dig, int col, int N, Num*** board, int m, int n) {
	int row;
	for (row = 0; row < N; row++){
		if (board[row][col]->num == dig && board[row][col]->status != FIXED) {
			board[row][col]->num = 0;
			if (validate_dig(dig,row,col,m,n,n*m, board,0,1))
				board[row][col]->status = SHOWN;
			board[row][col]->num = dig;
		}
	}
}

/**
 * parse_block goes through the block of the given cell coordinates and looks for a number dig.
 * if found, the function validates the it. if it's valid, it changes its status to SHOWN.
 * @param
 * dig - the number to search
 * row - given row
 * col - given col
 * m - number of rows in a block
 * n - number of columns in a block
 * board - the game board
 */
void parse_block(int dig, int row, int col, int m, int n, Num*** board) {
	int r_start = row - row % m;
	int c_start = col - col % n;
	int i; int j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			if (board[r_start+i][c_start+j]->num == dig && board[r_start+i][c_start+j]->status != FIXED) {
				board[r_start+i][c_start+j]->num = 0;
				if (validate_dig(dig,r_start+i,c_start+j,m,n,n*m, board,0,1)) {
					board[r_start+i][c_start+j]->status = SHOWN;
				}
				board[r_start+i][c_start+j]->num = dig;
			}
		}
	}
}

/**
 * parse_legitimate goes through the row, the column and the block of a given cell, and looks for a number dig.
 * if found, the function validates it. if it's valid, it changes its status to SHOWN.
 * @param
 * row - given row
 * col - given col
 * dig - the number to search
 * board - the game board
 * m - number of rows in a block
 * n - number of columns in a block
 */
void parse_legitimate (int col, int row, int dig, Num*** board, int m, int n) {
	if (dig != 0) {
		parse_row (dig,row,n*m,board, m, n);
		parse_col (dig,col,n*m,board,m,n);
		parse_block(dig,row,col,m,n,board);
	}
}


/**
* erroneous_board - checks if the board contains erroneous values
*
* @param
* board - the Sudoku board
* N - number of digits in one block
* @return
* 1 - if the board contains erroneous values
* 0 - otherwise
*
*/
int erroneous_board(Num*** board, int N) {
	int i; int j;
	for(i=0; i< N; i++){
		for(j=0; j< N; j++){
			if(board[i][j]->status == ERRONEOUS){
				return 1;
			}
		}
	}
	return 0;
}

/**
 * validate - validates that the current state of the board is solvable by calling ilp.
 * and prints massages according to the returned value.
 * @param
 * board - the Sudoku board
 * m - number of rows in one block
 * n - number of columns in one block
 */
int validate(Num*** board, int m, int n, MODE mode, int print_msg) {
	if(mode == INIT){
		print_invalid();
		return 3;
	}
	if(erroneous_board(board,n*m)){
		if (!print_msg)
			return 0;
		print_contains_error();
		return 3;
	}
	if(ilp(board, m, n, "valid", 0 , 0)){
		if (print_msg)
			print_validation_passed();
		return 2;
	}
	if (print_msg)
		print_validation_failed();
	return 3;
}

/**
 * switch_mode - switches the game's mode and makes the necessary adjustments,
 * such as emptying move list or changing relevant veriable's values.
 * @param
 * mode - game's mode
 * val - 1 for init, 2 for edit, 3 for solve
 * curr_move - pointer to current node of the list
 * mark_errors - pointer to short stating whether marking errors.
 */
void switch_mode (MODE* mode, int val, MoveList** curr_move) {
	switch (val) {
	case 1:
		*mode = INIT;
		break;
	case 2:
		*mode = EDIT;
		break;
	case 3:
		*mode = SOLVE;
		break;
	default:
		break;
	}
	empty_move_list(curr_move);
}

/**
 * set - sets a cell to the required number.
 * adds the move to the move list, changes relevant cell's status to erroneous and updates the number of hidden cells.
 * when board is full, prints message if it is solved or erroneous. changes mode to INIT if it's solved.
 * @param 
 * col - column in which the cell is located
 * row - row in which the cell is located
 * dig - the number to put into the cell
 * board - the Sudoku board
 * m - number of rows in one block
 * n - number of columns in one block
 * count_hid - pointer to the number of hidden cells in the Sudoku board
 * mode - pointer to game's mode
 * curr_move - pointer to the pointer of current move
 * mark_errors - indicates whether there's a need to mark the errorneous cells.
 * @return
 * 2 - the cell has been set.
 * 3 - an error occured.
 *
 */
int set (int col,int row,int dig, Num*** board, int m, int n, int* count_hid, MODE* mode, MoveList** curr_move, int mark_errors) {
	int prev_val; STAT prev_stat; SingleSet* move_step; Move* move = NULL;
	/*errors*/
	if (*mode == INIT){
		print_invalid();
		return 3;
	}
	if(col < 0 || row < 0 || dig < 0 || col >= n*m || row >= n*m || dig > n*m){ /*check for range*/
		print_invalid_range(n*m, "set");
		return 3;
	}
	if (board[row][col]->status == FIXED) {
		print_fixed();
		return 3;
	}
	/*set the cell*/
	prev_val = board[row][col]->num;
	prev_stat = board[row][col]->status;
	if (prev_val != dig) { /*if they are the same, board is not changing, and we don't consider it as a move*/
		if (prev_stat == ERRONEOUS) { /* change cells that are now valid to SHOWN */
			board[row][col]->num = 0;
			parse_legitimate(col,row,prev_val,board,m,n);
		}
		if (dig == 0) { /*emptying a cell*/
			board[row][col]->status = HIDDEN;
			*count_hid+=1;
		}
		else {  /*need to mark all new errors*/
			if (!validate_dig(dig,row,col,m, n,n*m, board, 1,1))
				board[row][col]->status = ERRONEOUS;
			else
				board[row][col]->status = SHOWN;
		}
		if (prev_val == 0)
			*count_hid-=1;
		board[row][col]->num = dig;
		/* clear moves beyond current move and add this move */
		move_step = create_single_set(prev_val,dig,col,row);
		move = create_move(move_step);
		empty_move_list_forward((*curr_move)->next);
		(*curr_move)->next = create_move_list(*curr_move,move);
		*curr_move = (*curr_move)->next;
	}
	print_board(board ,m, n, *mode, mark_errors);
	if (*count_hid == 0 && *mode == SOLVE) {
		if (validate(board,m,n,*mode,0) == 2) {
			printf("Puzzle solved successfully\n");
			switch_mode(mode,1,curr_move);
		}
		else
			printf("Puzzle solution erroneous\n");
	}
	return 2;
}


/* create_move_from_board - goes through the board and creates a move, according to the function it was called from.
 * call from generate -  steps are from empty cells to value filled by generate (move contains all cells on board that are not empty)
 * call from autofill - steps are from empty cells to values suggested by autofill saved in alt_num.
 * call from autofill - function moves the value from alt_num to num and clears alt_num
 * @param
 * board- game's board
 * N - number of cells in one block
 * calling_func - short name of calling function, either "gen" for generate or "auto" for autofill
 * @return
 * pointer to created move.
 */
Move* create_move_from_board(Num*** board, int N, char* calling_func) {
	int i; int j; int new_dig = 0;
	Move* head_move = create_move(NULL);
	Move* move = head_move;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (!strcmp(calling_func,"gen")) /*digit for move is num*/
				new_dig = board[i][j]->num;
			else if (!strcmp(calling_func,"auto")) { /*digit for move is alt_num, clear it and set num*/
				new_dig = board[i][j]->alt_num;
				board[i][j]->alt_num = 0;
				if (new_dig != 0)
					board[i][j]->num = new_dig;
			}
			if (new_dig != 0 ) { /*create move and add it*/
				SingleSet* step = create_single_set(0,new_dig,j,i);
				if (move->change == NULL) { /*first step of move*/
					move->change = step;
				}
				else { /*another node to steps of move*/
					move->next = create_move(step);
					move = move->next;
				}
			}
		}
	}
	return head_move;
}


/**
 * generate - generates a puzzle by randomly filling x cells with random legal values, running ILP to solve
 * the resulting board, and then clearing all but Y random cells.
 * @param 
 * board - the Sudoku board
 * m - number of rows in one block
 * n - number of columns in one block
 * x - number of random cells to be filled with random value
 * y - number of random cells to be set as fixed
 *
 * @return
 * 2 - solution was found
 * 3 - otherwise
 *
 */
int generate (Num*** board, int m, int n, int x, int y, MODE mode, int* count_hid, MoveList** curr_move) {
	int j = 0;
	int N = n*m;
	Move* head_move; MoveList* move;
	/*errors*/
	if(mode != EDIT){
		print_invalid();
		return 3;
	}
	if(x < 0 || x > N*N || y < 0 || y > N*N){
	print_invalid_range(N*N, "gen");
	return 3;
	}
	if(*count_hid < N*N){
		printf("Error: board is not empty\n");
		return 3;
	}
	if (y > 0) { /*otherwise, nothing is actually happening*/
		while(j<1000){
			/* X random cells filled with legal random value and solve board using ilp */
			if (fill_k_cells(board, x, m, n) && ilp(board, m, n, "gen", 0, 0)) { /*on success*/
				/* Randomly clear N*N-Y cells */
				clear_board(board, N, N*N-y);
				*count_hid = N*N-y;
				print_board(board,m,n,mode,1);
				/*create a move with all changes from empty to filled cells*/
				head_move = create_move_from_board(board,n*m, "gen");
				empty_move_list_forward((*curr_move)->next);
				move = create_move_list(*curr_move,head_move);
				(*curr_move)->next = move;
				*curr_move = (*curr_move)->next;
				return 2;
			}
			/* on failure */
			j++;
			clear_board(board, N, N*N);
		} /*didn't succeed after 1000 iterations*/
		print_gen_failed();
		return 3;
	}
	print_board(board,m,n,mode,1); /*if nothing happend print board anyway*/
	return 2;
}

/**
 * undo - undo the last move.
 * function undoes the move, prints the board, and then prints all the changes, when necessary.
 * @param
 * mode - game's mode
 * curr_move - pointer to pointer of current move
 * board - game's board
 * count_hid - pointer to counter of hidden cells on board
 * m - number of rows in one block
 * n - number of columns in one block
 * mark_errors - indicates whether to mark errors on board
 * print_msg - indicates whether to print messages of cells' changes
 * @return
 * 2 - if move undone successfully
 * 3 - otherwise
 */
int undo (MODE mode, MoveList** curr_move, Num*** board, int* count_hid, int m, int n, int mark_errors, int print_msg) {
	int row; int col; int prev_val; int next_val; Move* step;
	char prev[10]; char next[10];
	if (mode == INIT) {
		print_invalid();
		return 3;
	}
	if ((*curr_move)->head_move == NULL) {
		print_no_moves_undo();
		return 3;
	}
	step = (*curr_move)->head_move;
	while (step != NULL) { /*undo all steps in a move*/
		row = step->change->row;
		col = step->change->col;
		prev_val = step->change->prev_val;
		next_val = step->change->new_val;
		if (prev_val != next_val) { /*if they are the same, no point of checking and changing*/
			board[row][col]->num = 0;
			if (board[row][col]->status == ERRONEOUS) { /* change cells that are now valid to SHOWN */
				parse_legitimate(col,row,next_val,board,m,n);
			}
			/*update cell and change cells that are now wrong to ERRONEOUS*/
			if (validate_dig(prev_val,row,col,m,n,n*m,board,1,1))
				board[row][col]->status = SHOWN;
			else
				board[row][col]->status = ERRONEOUS;
			board[row][col]->num = prev_val;
			/*update num of hidden cells*/
			if (prev_val == 0) {
				*count_hid+=1;
				board[row][col]->status = HIDDEN;
			}
			else if (next_val == 0)
				*count_hid-=1;
		}
		step = step->next;
	}
	if (print_msg) {
	/*print board*/
	print_board(board,m,n,mode,mark_errors);
	/*print messages*/
		step = (*curr_move)->head_move;
		while (step != NULL) {
			row = step->change->row;
			col = step->change->col;
			prev_val = step->change->prev_val;
			next_val = step->change->new_val;
			prev_val == 0? sprintf(prev, "_") : sprintf(prev,"%d",prev_val);
			next_val == 0? sprintf(next, "_") : sprintf(next,"%d",next_val);
			printf("Undo %d,%d: from %s to %s\n",col+1,row+1,next,prev);
			step = step->next;
		}
	}
	/*update pointer to previous node*/
	*curr_move = (*curr_move)->prev;
	return 2;
}

/**
 * redo - redo the last move.
 * function redoes the move, prints the board, and then prints all the changes.
* @param
 * mode - game's mode
 * curr_move - pointer to pointer of current move
 * board - game's board
 * count_hid - pointer to counter of hidden cells on board
 * m - number of rows in one block
 * n - number of columns in one block
 * mark_errors - indicates whether to mark errors on board
 * print_msg - indicates whether to print messages of cells' changes
 * @return
 * 2 - if move redone successfully
 * 3 - otherwise
 */
int redo (MODE mode, MoveList** curr_move, Num*** board, int* count_hid, int m, int n, int mark_errors) {
	int row; int col; int prev_val; int next_val; Move* step;
	char prev[10]; char next[10];
	if (mode == INIT) {
		print_invalid();
		return 3;
	}
	/*update pointer to next node*/
	if ((*curr_move)->next == NULL) {
		print_no_moves_redo();
		return 3;
	}
	*curr_move = (*curr_move)->next;
	step = (*curr_move)->head_move;
	while (step != NULL) { /*redo all steps in a move*/
		row = step->change->row;
		col = step->change->col;
		prev_val = step->change->prev_val;
		next_val = step->change->new_val;
		if (prev_val != next_val) { /*if they are the same, no point of checking and changing*/
			board[row][col]->num = 0;
			if (board[row][col]->status == ERRONEOUS) { /* change cells that are now valid to SHOWN */
				parse_legitimate(col,row,prev_val,board,m,n);
			} /*update cell and change cells that are now wrong to ERRONEOUS*/
			if (validate_dig(next_val,row,col,m,n,n*m,board,1,1))
				board[row][col]->status = SHOWN;
			else
				board[row][col]-> status = ERRONEOUS;
			board[row][col]->num = next_val;
		/*update num of hidden cells*/
		if (next_val == 0) {
			*count_hid+=1;
			board[row][col]-> status = HIDDEN;
		}
		else if (prev_val == 0)
			*count_hid-=1;
		}
		step = step->next;
	}
	/*print board*/
	print_board(board,m,n,mode,mark_errors);
	/*print message*/
	step = (*curr_move)->head_move;
	while (step != NULL) {
		row = step->change->row;
		col = step->change->col;
		prev_val = step->change->prev_val;
		next_val = step->change->new_val;
		prev_val == 0? sprintf(prev, "_") : sprintf(prev,"%d",prev_val);
		next_val == 0? sprintf(next, "_") : sprintf(next,"%d",next_val);
		printf("Redo %d,%d: from %s to %s\n",col+1,row+1,prev,next);
		step = step->next;
	}
	/*update pointer to next node*/
	return 2;
}


/**
 * autofill - fills obvious values on board (cells with single legal value).
 * adds the move to the move list.
 * @param:
 * board - game's board
 * n - number of colums
 * @return
 * 2 - on success
 * 3 - if an error occured
 */
int autofill (Num*** board, int m, int n, MODE* mode, int* count_hid, MoveList** curr_move, int mark_errors) {
	int i; int j; int dig; int N = n*m;
	int legal_vals; int sol;
	Move* head_move; MoveList* move = NULL;
	if (*mode != SOLVE) {
		print_invalid();
		return 3;
	}
	if (erroneous_board(board,N)) {
		print_contains_error();
		return 3;
	}
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (board[i][j]->num == 0) { /*if cell is empty, check for solutions*/
				legal_vals = 0; sol = 0;
				for (dig = 1; dig <= N; dig++) {
					if (validate_dig(dig,i,j,m,n,N,board,0,1)) {
						legal_vals++;
						sol = dig;
					}
					if (legal_vals > 1) /*more than single solution, move to next cell*/
						break;
				}
				if (legal_vals == 1) { /*autofill cell*/
					*count_hid-=1;
					if (validate_dig(sol,i,j,m,n,N,board,1,0))
						board[i][j]->status = SHOWN;
					else
						board[i][j]->status = ERRONEOUS;
					board[i][j]->alt_num = sol;
					print_set_cell(j,i,sol);
				}
			}
		}
	}
	/*create a new move, remove forward moves, add move to move list and clear alt_nums*/
	head_move = create_move_from_board(board,N,"auto"); /*also clears alt_nums back to 0*/
	if (head_move->change!=NULL){
		empty_move_list_forward((*curr_move)->next);
		move = create_move_list(*curr_move,head_move);
		(*curr_move)->next = move;
		*curr_move = (*curr_move)->next;
	}
	else { /*otherwise, no cell has been changed and no need to make new move*/
		free (head_move);
	}
	print_board(board,m,n,*mode,mark_errors);
	/*check if reached end of the game*/
	if (*count_hid == 0) {
			if (validate(board,m,n,*mode,0) == 2) {
				printf("Puzzle solved successfully\n");
				switch_mode(mode,1,curr_move);
			}
		}
	return 2;
}

/**
 * hint - gives a hint to the user by showing a legal value of required cell, according to the ILP solution.
 * @param
 * board - the Sudoku board
 * col - column in which the cell is located
 * row - row in which the cell is located
 * m - number of rows in one block
 * n - number of columns in one block
 * mode - game's mode
 * @return
 * 2 - on success
 * 3 - if an error occured
 */
int hint(Num*** board, int col, int row, int m, int n, MODE mode) {
	int res;
	/*errors*/
	if (mode != SOLVE) {
		print_invalid();
		return 3;
	}
	if (col < 0 || row < 0 || col >= n*m || row >= n*m){ /*check for range*/
		print_invalid_range(n*m, "hint");
		return 3;
	}
	if (erroneous_board(board,n*m)) {
		print_contains_error();
		return 3;
	}
	if (board[row][col]->status == FIXED) {
		print_fixed();
		return 3;
	}
	if (board[row][col]->num != 0) {
		print_contains_val();
		return 3;
	}
	/*print hint or unsolvable board*/
	res = ilp(board,m,n,"hint",col,row);
	if (!res) {
		printf("Error: board is unsolvable\n");
		return 3;
	}
	print_hint(res);
	return 2;

}

/**
 * save - saves the board as a text file to the path inserted by the user.
 * @param
 * board -game's board
 * path - requested path of saving.
 * m - number of rows in one block
 * n- number of columns in one block
 * mode - game's mode.
 * @return
 * 2 - on success
 * 3 - if an error occured
 */
int save (Num*** board, char* path, int m, int n, MODE mode) {
	FILE* fp; int i; int j; int N = n*m;
	/*errors*/
	if (mode == INIT) {
		print_invalid();
		return 3;
	}
	if (mode == EDIT && erroneous_board(board,n*m)) {
		print_contains_error();
		return 3;
	}
	if (mode == EDIT && validate(board,m,n,mode,0) == 3) {
		print_err_validation();
		return 3;
	}
	fp = fopen (path, "w");
	if(!fp){
		print_file_err_save();
		return 3;
	}
	/*saving*/
	fprintf(fp,"%d %d\n", m, n);
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			write_num(fp,board,i,j,mode);
			if (j == N-1)
				fputc('\n',fp);
			else
				fputc(' ', fp);
		}
	}
	fclose(fp);
	printf("Saved to: %s\n", path);
	return 2;
}

/**
 * solve - loads a board from a file provided by the user in SOLVE mode. initializes the undo/redo list.
 * @param
 * path - path to the file
 * board - pointer to game's board
 * mode - pointer to game's mode
 * curr_move - pointer to pointer of current move
 * m - pointer to number of rows in one block
 * n- pointer to number of columns in one block
 * count_hid - pointer to number of hidden cells on the board
 * mark_errors - indicates whether to mark errors.
 * @return
 * 2 on success of creating the board.
 * 3 otherwise
 */
int solve(char* path, Num**** board, MODE* mode, MoveList** curr_move, int* m, int* n, int* count_hid, int mark_errors) {
	int res;
	FILE* fp = fopen (path, "r");
	if(!fp){
		print_file_err_solve();
		return 3;
	}
	switch_mode(mode,3,curr_move);
	res = parse_file(fp, board, m, n, count_hid, *mode, mark_errors);
	fclose(fp);
	return res;
}

/**
 * change_mark_errors - changes mark_errors to the requested value - either 1 or 0.
 * @param
 * x - the requested value
 * mode - game's mode
 * mark_errors - pointer to the mark_errors variable.
 * @return
 * 2 - if the variable was changes successfully
 * 3 - otherwise
 */
int change_mark_errors (double x, MODE mode, int* mark_errors) {
	if(mode != SOLVE){
		print_invalid();
		return 3;
	}
	if (x != 0.0 && x != 1.0){
		print_invalid_range(0,"mark_errors");
		return 3;
	}
	*mark_errors = (int) x;
	return 2;
}

/**
 * reset - undoes all moves and clears the undo/redo list.
 * prints a 'Board reset' message and afterwards prints the board.
 * @param
 * board- game's board
 * curr_move - pointer to pointer of current move
 * mode - game's mode
 * count_hid - pointer to counter of hidden cells on board
 * m - number of rows in one block
 * n - number of columns in one block
 * @return
 * 2 - if reset was done successfully
 * 3 - otherwise
 */
int reset (Num*** board, MoveList** curr_move, MODE mode, int* count_hid, int m, int n, int mark_errors) {
	if (mode == INIT) {
		print_invalid();
		return 3;
	} /*undo move and change pointer to previous move, untill we reach the empty move*/
	while ((*curr_move)->head_move != NULL) {
		undo(mode,curr_move,board,count_hid,m,n,0,0);
	} /*destroy all moves except the empty move*/
	empty_move_list_forward((*curr_move)->next);
	(*curr_move)-> next = NULL;
	print_reset();
	print_board(board,m,n,mode,mark_errors);
	return 2;
}

/**
 * free_board frees all memory resources
 *
 * @param  
 * board - pointer to the Sudoku board
 * N - number of cells in row/col
 *
 */
void free_board(Num**** board, int N){
	 int i; int j;
	 for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			destroy_num((*board)[i][j]); /*frees all nums*/
		}
		free((*board)[i]); /*frees all cols*/
	}
	free(*board); /*frees all rows*/
 }

/**
 * edit - loads a board from a file provided by the user in EDIT mode or creates an empty board with default size.
 * initializes the undo/redo list.
 * @params
 * parsed_command - the path provided by the user or NULL if not provided
 * board - pointer to game's board
 * mode - pointer to game's mode
 * curr_move - pointer to pointer of current move
 * m - pointer to number of rows in one block
 * n - pointer to number of columns in one block
 * count_hid - pointer to number of hidden cells
 * @return
 * 2 - if board has been loaded successfully
 * 3 - otherwise
 */
int edit (char* parsed_command, Num**** board, MODE* mode, MoveList** curr_move, int* m, int* n, int* count_hid) {
	FILE* fp; int res;
	if (parsed_command == NULL){ /* no file path supplied */
		switch_mode(mode,2,curr_move);
		if (*board!=NULL) {
			free_board(board,*n**m);
			*board = NULL;
		}
		*m = DEF_ROWS;
		*n = DEF_COLS;
		*count_hid = (DEF_ROWS*DEF_COLS)*(DEF_ROWS*DEF_COLS);
		*board = create_empty_board(*m,*n);
		print_board(*board,*m,*n,*mode,1);
		return 2;
	}
	fp = fopen (parsed_command, "r");
	if(!fp){
		print_file_err_edit();
		return 3;
	}
	switch_mode(mode,2,curr_move);
	/* create game board from File */
	res = parse_file(fp, board, m, n, count_hid, *mode, 1);
	fclose(fp);
	return res;
}

/**
 * num_of_solutions - prints how many solutions there are to the current board using ex_backtrack.
 * @param
 * board - game's board
 * m - number of rows in one block
 * n - number of columns in one block
 * count_hid - counter of hidden cells on board
 * mode - game's mode
 * @return
 * 2 - if check has been successful
 * 3 - otherwise
 */
int num_of_solutions (Num*** board, int m, int n, int count_hid, MODE mode) {
	int res;
	if (mode == INIT) {
		print_invalid();
		return 3;
	}
	if (erroneous_board(board,n*m)) {
		print_contains_error();
		return 3;
	}
	res = ex_backtrack(board, n, m, count_hid);
	print_num_sols(res);
	if (res == 1)
		print_good_board();
	else if (res > 1)
		print_multi_sols();
	return 2;
}

/**
 * exit_game frees all memory resources and exits the program.
 *
 * @param  
 * board - pointer to the Sudoku board
 * N - number of cells in row/col
 * curr_move - pointer to pointer of current move
 * @return
 * 1.
 */
int exit_game (Num**** board, int N, MoveList** curr_move) {
	printf("Exiting...\n");
	free_board(board, N);
	empty_move_list(curr_move);
	free(*curr_move); /*freeing the empty node*/
	return 1;
}
