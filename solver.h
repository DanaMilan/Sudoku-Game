/**
* solver Summary:
* module contains functions related to solving the board.
*
* supports the following functions:
*
* validate_dig - checks if a placement of a given digit in a given cell is valid.
* validate_row - checks if a placement of a given digit in a given cell is valid, according to its row.
* validate_col - checks if a placement of a given digit in a given cell is valid, according to its column.
* validate_block - checks if a placement of a given digit in a given cell is valid, according to its block.
* ex_backtrack - executes exhaustive backtrack to find number of solutions of the board using a stack.
* ilp - function solves Sudoku board with ILP using Gurobi.
*
*/


extern int validate_dig (int dig, int row, int col, int m, int n, int N, Num*** board, int change_err, int num_alt);

extern int validate_row(int dig, int row, int N, Num*** board, int change_err, int num_alt);

extern int validate_col(int dig, int col, int N, Num*** board, int change_err, int num_alt);

extern int validate_block(int dig, int row, int col, int m, int n, Num*** board, int change_err, int num_alt);

extern int ex_backtrack (Num*** board, int m, int n, int count_hid);

extern int ilp(Num*** board, int m, int n, char* calling_func, int h_x, int h_y);
