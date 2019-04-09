#include <stdlib.h>
#include "structs.h"

/**
 * create_num initializes struct Num, which represents a cell, by saving the given number, and calls
 * create_arr function to initialize the array of valid numbers for this cell.
 *
 * @param
 * input_num - the number to put in cell
 * N - represents the number of cells in row/col and the max valid number for Sudoku puzzle
 *
 * @return
 * pointer to struct Num which represents a cell
 *
 */
Num* create_num(int input_num) { 
	Num* new_num = (Num*) malloc(sizeof(Num));
	if (!new_num) {
		return NULL;
	}
	new_num -> num = input_num;
	new_num -> alt_num = 0;
	new_num -> status = HIDDEN;
	return new_num;
}

/**
 * destroy_num - releases a memory used to initialize the struct Num
 *
 * @param n - pointer to struct Num, which represents the number that appears in the cell.
 *
 */
void destroy_num (Num* n) {
	if (!n) {
		return;
	}
	free(n);
	return;
}

/** create_single_set - creats a SingleSet structure which represents one change of cell on board.
 * @param
 * prev_val - previous value of the cell
 * new_val - new value of the cell
 * col - column in which the cell is located
 * row - row in which the cell is located.
 * @return
 * pointer to SingleSet created.
 */
SingleSet* create_single_set(int prev_val, int new_val, int col, int row) {
	SingleSet* new_set = (SingleSet*) malloc(sizeof(SingleSet));
	if (!new_set) {
		return NULL;
	}
	new_set->prev_val = prev_val;
	new_set->new_val = new_val;
	new_set->col = col;
	new_set->row = row;
	return new_set;
}

/**
 * create_move - creates a move with one cell change of the board.
 * @param
 * cell_set - SingleSet of the change of the board.
 * @return
 * pointer to new move
 */
Move* create_move(SingleSet* cell_set) {
	Move* head = (Move*) malloc(sizeof(Move));
	head->change = cell_set;
	head->next=NULL;
	return head;
}

/**
 * delete_move - frees the list of Moves.
 * @param
 * head - the move to free.
 */
void delete_move (Move* head) {
	if (head != NULL){
		free(head->change);
		delete_move (head->next);
		free(head);
	}
}

/**
 * create_move_list - creates a MoveList node, with a given previous Movelist node and a pointer to the first Move on the list of Moves in this node.
 * @param
 * tail - the previous MoveList, which is the previous tail of the list
 * head_move - first Move of the list of moves to be contained in this node.
 */
MoveList* create_move_list (MoveList* tail, Move* head_move) {
	MoveList* new_move = (MoveList*) malloc(sizeof(MoveList));
	new_move->prev = tail;
	new_move->head_move = head_move;
	new_move->next = NULL;
	return new_move;
}

/**
 * empty_move_list_forward - frees all forward MoveList nodes including the given one.
 * @param
 * curr_move - pointer to current node in the MoveList list.
 */
void empty_move_list_forward (MoveList* curr_move) {
	if (curr_move != NULL){
		delete_move(curr_move->head_move);
		empty_move_list_forward(curr_move->next);
		free(curr_move);
	}
}

/**
 * empty_move_list_backward - frees all backward MoveList nodes including the given one, except for the empty node.
 * @param
 * curr_move - pointer to current node in the MoveList list.
 * @return
 * pointer to the empty node
 */
MoveList* empty_move_list_backward (MoveList* curr_move) {
	MoveList * empty_node;
	if (curr_move->head_move == NULL)
		return curr_move;
	delete_move(curr_move->head_move);
	empty_node = empty_move_list_backward(curr_move->prev);
	free(curr_move);
	return empty_node;
}

/**
 * empty move list - frees all nodes of the move list except for the empty node and changes the pointer to it.
 * the function sets the curr_move pointer to the empty node.
 * @param
 * curr_move - pointer to the current node on the move list.
 */
void empty_move_list(MoveList** curr_move) {
	MoveList* empty_node;
	if ((*curr_move)->head_move != NULL) {
		delete_move((*curr_move)->head_move);
		empty_move_list_forward((*curr_move)->next);
		empty_node = empty_move_list_backward((*curr_move)->prev);
		free (*curr_move);
		*curr_move = empty_node;
	}
	else
		empty_move_list_forward((*curr_move)->next);
	(*curr_move)->next = NULL;
}

/**
 * create_stack - creates a stack which top is pointing to NULL.
 * @return
 * pointer to the stack created.
 */
Stack* create_stack() {
	Stack* stk = malloc(sizeof(Stack));
	stk->top = NULL;
	return stk;
}

/**
 * push_to_stack - creates a new stack element, which consists of a cell getting a value and pushes it on top of the stack.
 * @param
 * dig - the value the cell gets
 * row - row in which the cell is located
 * col - column in which the cell is located
 */
void push_to_stack (int dig, int row, int col, Stack *stk ) {
	Elem *p = malloc(sizeof (Elem));
	p->row = row;
	p->col = col;
	p->val = dig;
	p->next = stk->top;
	stk->top = p;
}

/**
 *  pop_stack - pops the top element of the stack
 *  @param
 *  stk - pointer to the stack.
 */
void pop_stack (Stack *stk) {
	Elem *p = stk->top;
	stk->top = stk->top->next;
	free(p);
}
