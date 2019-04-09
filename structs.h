#ifndef STRUCTS_H_
#define STRUCTS_H_

/**
* This module consists of declerations of structures and enums used in the program.
*/


/**
* Type represents cell status
*/
typedef enum num_status {
	HIDDEN,
	SHOWN,
	FIXED,
	ERRONEOUS
} STAT;


/**
* Type represents cell
*/
typedef struct sudoku_number {
	int num;
	int alt_num;
	STAT status;
} Num;

/**
* Type represents game mode
*/
typedef enum game_mode {
	INIT,
	SOLVE,
	EDIT
} MODE;

/**
* Type represents a single cell 'set' (change of cell's value).
*/
typedef struct set_cell {
	int prev_val; /*previous value of cell*/
	int new_val; /*new value of cell*/
	int col; int row; /*coordinates of cell*/
} SingleSet;

/**
 * type is a singly linked list which as a whole reprsents one move of the user. 
 * Each node contains a SingleSet, that is, each node represents one cell change done on board in this move.
 */
typedef struct move {
	SingleSet* change; /* set of one cell*/
	struct move* next; /* next step in this move*/
} Move;

/**
* Type is a doubly linked list which as a whole represents the undo/redo move list of the program.
* Each node contains a Move, that is, each node represents on move done by the user (set/autofill/generate).
*/
typedef struct move_list {
	Move* head_move;
	struct move_list* next;
	struct move_list* prev;
} MoveList;

/**
 * type represents element of stack of cell changes, for the exhaustive backtrackign.
 */
typedef struct elem
{
	int row; /*row of cell changed*/
	int col; /*column of cell changed*/
	int val; /*new value of cell*/
	struct elem *next;
} Elem;

/**
* Type represents a stack of cell changes, for the exhaustive backtracking.
*/
typedef struct stack {
	Elem *top;
} Stack;


#endif
