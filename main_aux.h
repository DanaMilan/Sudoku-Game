/**
 * main_aux Summary:
 *
 * Consist of auxiliary functions which print messages for the user:
 *
 * print_invalid - Prints error message for invalid command
 * print_no_moves_undo - prints error message when there are no moves to undo
 * print_no_moves_redo - prints error message when there are no moves to redo
 * print_invalid_range an error message of invalid range according to the function if was called from.
 * print_contains_error - prints that there are erroneous valus on board
 * print_validation_passed - prints that validation passed
 * print_validation_failed - prints that validation failed
 * print_set_cell - prints that cell on column x and row y was set to z
 * print_fixed - prints an error that requested cell is fixed
 * print_contains_val - prints an error that requested cell already contains a value
 * print_hint - prints a hint with value dig.
 * print_reset - prints that board was reset.
 * print_err_validation - prints an error that validation failed
 * print_file_err_save - prints that file could not be created or modified
 * print_file_err_solve - prints that file doesnt exist or cannot be opened
 * print_file_err_edit - prints that file cannot be opened
 * print_num_sols - prints number of solutions of the board.
 * print_good_board - prints that the board is good.
 * print_multi_sols - prints that the user should try to edit the board further.
 * print_gen_failed - prints that puzzle generator failed.
 */
 
extern void print_invalid();

extern void print_no_moves_undo();

extern void print_no_moves_redo();

extern void print_invalid_range(int N, char* func);

extern void print_contains_error ();

extern void print_validation_passed();

extern void print_validation_failed();

extern void print_set_cell(int x, int y, int z);

extern void print_fixed ();

extern void print_contains_val();

extern void print_hint(int dig);

extern void print_reset();

extern void print_err_validation();

void print_file_err_save();

void print_file_err_solve();

void print_file_err_edit();

void print_num_sols(int num);

void print_good_board();

void print_multi_sols();

void print_gen_failed();
