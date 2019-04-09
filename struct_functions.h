/*
 * struct_functions Summary:
 * Encapsulates the functions that are related directly to the programs' structures management.
 *
 * Supports the following functions:
 *
 * create_num - Initializes each board cell
 * destroy_num - Releases a memory used to initialize the cells
 * create_single_set - creats a SingleSet structure which represents one change of cell on board.
 * create_move - creates a move with one cell change of the board. (a Move node)
 * delete_move - frees the list of Moves.
 * create_move_list - creates a MoveList node, with a given previous Movelist node and a pointer to the first Move on the list of Moves in this node.
 * empty_move_list_forward - frees all forward MoveList nodes including the given one.
 * empty_move_list_backward - frees all backward MoveList nodes including the given one, except for the empty node.
 * empty move list - frees all nodes of the move list except for the empty node and changes the pointer to it.
 * create_stack - creates a stack which top is pointing to NULL.
 * push_to_stack - creates a new stack element, which consists of a cell getting a value, and pushes it on top of the stack.
 * pop_stack - pops the top element of the stack.
 */

extern Num* create_num(int n);

extern void destroy_num (Num* n);

extern SingleSet* create_single_set(int prev, int new, int col, int row);

extern Move* create_move(SingleSet* cell_set);

extern void delete_move (Move* head);

extern MoveList* create_move_list (MoveList* tail, Move* move);

extern void empty_move_list_forward (MoveList* curr_move);

extern MoveList* empty_move_list_backward (MoveList* curr_move);

extern void empty_move_list(MoveList** curr_move);

extern Stack* create_stack();

extern void push_to_stack (int dig, int row, int col, Stack *stk );

extern void pop_stack (Stack *stk);
