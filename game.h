/*
 * game Summary:
 * Encapsulates the Sudoku puzzle game and the Sudoku board.
 *
 * Supports the following functions which execute the commands given by the user of the game: 
 *
 * print_board - Prints the Sudoku puzzle
 * fill_k_cells - fill k random cells with legal random values
 * create_empty_board - creates a new empty board of the given size.
 * validate - validates that the current state of the board is solvable by calling ilp.
 * switch_mode - switches the game's mode and makes the necessary adjustments
 * set - Sets/clears the number of a cell as requested by the user.
 * generate - generates a board with random values by user's request.
 * undo - undoes the last move.
 * redo - redoes the last move.
 * autofill - fills cells with single legal value.
 * hint - gives a hint to the user by showing a legal value of required cell, according to the ILP solution.
 * save - saves the board as a text file to the path inserted by the user.
 * solve - loads a board from a file provided by the user in SOLVE mode.
 * change_mark_errors - changes mark_errors to the requested value - either 1 or 0.
 * reset - undoes all moves and clears the undo/redo list, then prints a 'Board reset' message and the board.
 * free_board - Frees all memory resources
 * edit - loads a board from a file provided by the user in EDIT mode or creates an empty board with default size.
 * num_of_solutions - prints how many solutions there are to the current board using ex_backtrack.
 * exit_game - Frees all memory resources and exits the program
 *
 */

#include <stdio.h>
 
extern int print_board(Num*** board, int m, int n, MODE mode, int mark_errors);

extern Num*** create_empty_board(int m, int n);

extern int validate (Num*** board, int m, int n, MODE mode, int print_msg);

extern void switch_mode (MODE* mode, int val, MoveList** curr_move);

extern int set (int col,int row,int dig, Num*** board, int m, int n, int* count_hid, MODE* mode, MoveList** curr_move, int mark_errors);

extern int generate (Num*** board, int m, int n, int x, int y, MODE mode, int* count_hid, MoveList** curr_move);

extern int undo (MODE mode, MoveList** curr_move, Num*** board, int* count_hid, int m, int n, int mark_errors, int print_msg);

extern int redo (MODE mode, MoveList** curr_move, Num*** board, int* count_hid, int m, int n, int mark_errors);

extern int autofill (Num*** board, int m, int n, MODE* mode, int* count_hid, MoveList** curr_move, int mark_errors);

extern int hint (Num*** board, int col, int row, int m, int n, MODE mode);

extern int save (Num*** board, char* path, int m, int n, MODE mode);

extern int solve(char* path, Num**** board, MODE* mode, MoveList** curr_move, int* m, int* n, int* count_hid, int mark_errors);

extern int reset (Num*** board, MoveList** curr_move, MODE mode, int* count_hid, int m, int n, int mark_errors);

extern int change_mark_errors (double x, MODE mode, int* mark_errors);

extern void free_board(Num**** board, int N);

extern int edit (char* parsed_command, Num**** board, MODE* mode, MoveList** curr_move, int* m, int* n, int* count_hid);

extern int num_of_solutions (Num*** board, int m, int n, int count_hid, MODE mode);

extern int exit_game (Num**** board, int N, MoveList** curr_move);


