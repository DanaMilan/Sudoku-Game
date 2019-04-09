/**
* parser Summary:
* Parses through user's commands. Reads a command from user, checks for validity of command and calls the relevant function.
*
* supports the following functions:
*
* parse_file - The function parses a file and creates a board according to it and to the game's mode.
* get_command - parses through the user's input, calls the relevant command or prints an error message.
*
*/

extern int parse_file (FILE* fp, Num**** board, int* m, int* n, int* count_hid, MODE mode, int mark_errors);

extern int get_command (Num**** board, int* m,int* n, int* count_hid, MODE* mode, int* mark_errors, MoveList** curr_move);



