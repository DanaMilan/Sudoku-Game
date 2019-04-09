#include "gurobi_c.h"
#include "structs.h"
#include "struct_functions.h"
#include "game.h"
#include "main_aux.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*VALIDATION OF CELL*/

/**
* function checks if a placement of a given digit in a given cell is valid, according to its row.
* function goes through the row and checks if none of the already placed digits equals the given digit.
* function works differently for each stage of the game, indicated by param isv: init = 0, validate = 1, set = 2.
* on initialization (randomized backtrack), function works by cell.num values.
* on validation (deterministic backtrack), function works by cell.alt_num values.
* on setting, function works only with cells which their status != HIDDEN, by cell.num values.
* @param dig - the checked digit
* @param row - the cell's row
* @param N - number of digits in one row.
* @param board - game's board.
* @param change_err -  indicates whether to change the status of erroneous cells encountered to ERRONEOUS.
* @return
* 0 - if validation failed
* 1 - if validation succeeded
*/
int validate_row(int dig, int row, int N, Num*** board, int change_err, int num_alt) {
	int col; int valid = 1;
	for (col = 0; col < N; col++){
		if ((board[row][col]->num == dig && num_alt) || (board[row][col]->alt_num == dig && !num_alt)) {
			if (!change_err) /*no need to continue, we already know the answer*/
				return 0;
			valid = 0; /*otherwise, we need to change the status to each erroneous cell*/
			if (board[row][col]->status != FIXED && dig)
				board[row][col]->status = ERRONEOUS;
		}
	}
	return valid;
}

/**
* function checks if a placement of a given digit in a given cell is valid, according to its column.
* function goes through the column and checks if none of the already placed digits equals the given digit.
* function works differently for each stage of the game, indicated by param isv: init = 0, validate = 1, set = 2.
* on initialization (randomized backtrack), function works by cell.num values.
* on validation (deterministic backtrack), function works by cell.alt_num values.
* on setting, function works only with cells which their status != HIDDEN, by cell.num values.
* @param dig - the checked digit
* @param col - the cell's column
* @param N - number of digits in one row.
* @param board - game's board.
* @param change_err -  indicates whether to change the status of erroneous cells encountered to ERRONEOUS.
* @param num_alt - indicates whether to use num or alt num for the check - 1 = num, 0 = alt_num
* @return
* 0 - if validation failed
* 1 - if validation succeeded
*/
int validate_col(int dig, int col, int N, Num*** board, int change_err, int num_alt) {
	int row; int valid = 1;
	for (row = 0; row < N; row++){
		if ((board[row][col]->num == dig && num_alt) || (board[row][col]->alt_num == dig && !num_alt) ) {
			if (!change_err) /*no need to continue, we already know the answer*/ {
				return 0;
			}
			valid = 0; /*otherwise, we need to change the status to each erroneous cell*/
			if (board[row][col]->status != FIXED) {
				board[row][col]->status = ERRONEOUS;
			}
		}
	}
	return valid;
}

/**
* function checks if a placement of a given digit in a given cell is valid, according to its block.
* function goes through the block and checks if none of the already placed digits equals the given digit.
* function works differently for each stage of the game, indicated by param isv: init = 0, validate = 1, set = 2.
* on initialization (randomized backtrack), function works by cell.num values.
* on validation (deterministic backtrack), function works by cell.alt_num values.
* on setting, function works only with cells which their status != HIDDEN, by cell.num values.
* @param dig - the checked digit
* @param row - the cell's row
* @param col - the cell's column
* @param m - number of rows in one block.
* @param n - number of columns in one block.
* @param board - game's board.
* @param change_err -  indicates whether to change the status of erroneous cells encountered to ERRONEOUS.
* @param num_alt - indicates whether to use num or alt num for the check - 1 = num, 0 = alt_num
* @return
* 0 - if validation failed
* 1 - if validation succeeded
*/
int validate_block(int dig, int row, int col, int m, int n, Num*** board, int change_err, int num_alt) {
	int r_start = row - row % m;
	int c_start = col - col % n;
	int i; int j; int valid = 1;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			if ((board[r_start+i][c_start+j]->num == dig && num_alt) || (board[r_start+i][c_start+j]->alt_num == dig && !num_alt)) {
				if (!change_err) /*no need to continue, we already know the answer*/
					return 0;
				valid = 0; /*otherwise, we need to change the status to each erroneous cell*/
				if (board[r_start+i][c_start+j]->status != FIXED) {
					board[r_start+i][c_start+j]->status = ERRONEOUS;
				}
			}
		}
	}
	return valid;
}

/**
* function checks if a placement of a given digit in a given cell is valid, by calling the validation functions mentioned above.
* @param dig - the checked digit
* @param row - the cell's row
* @param col - the cell's column
* @param m - number of rows in one block.
* @param n - number of columns in one block.
* @param board - game's board.
* @param change_err - indicates whether to change the status of erroneous cells encountered to ERRONEOUS.
* @return
* 0 - if one of the validations has failed
* 1 - if all validations succeeded
*/
int validate_dig (int dig, int row, int col, int m, int n, int N, Num*** board, int change_err, int num_alt) {
	int valid = 1;
	if (dig == 0)
		return 1;
	if (!validate_row(dig, row, N, board,change_err,num_alt))
		valid = 0;
	if (!validate_col(dig, col, N, board,change_err, num_alt))
		valid = 0;
	if (!validate_block(dig,row,col,m, n, board,change_err, num_alt))
		valid = 0;
	return valid;
}


/*EXHAUSTIVE BACKTRACK*/

/**
* function finds first empty cell on board by alt_num values.
* iterates with pointers to row and col values until finds an index of which board[*row][*col]->alt_num == 0.
* @param board - game's board
* @param N - number of digits in one block
* @param row - pointer to variable holding value of the current row
* @param col - pointer to variable holding value of the current column
* @return
* 1 - if an empty cell has been found
* 0 - otherwise
*/
int find_first_empty(Num*** board, int N, int* row, int* col) {
	for (*row = 0; *row < N; (*row)++) {
		for (*col = 0; *col < N; (*col)++) {
			if (board[*row][*col]->alt_num == 0)
				return 1;
		}
	}
	return 0;
}

/**
 * clear_fill_alt_board - clears the alt_nums or copies the value of nums to them.
 * @param
 * board - game's board
 * N - number of cells in one row of the board.
 * fill - indicates whether to clear or copy - 0 = clear, 1 = copy num.
 */
void clear_fill_alt_board (Num*** board, int N, short fill) {
	int i; int j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (fill)
				board[i][j]-> alt_num = board[i][j]->num;
			else
				board[i][j]-> alt_num = 0;
		}
	}
}

/**
 * pop_till_inc - pops the stack while reseting the relevant cell on board, until reaching an incrementable cell or the end of the stack.
 * if reached an incrementable cell, pops it and pushes the cell again with the incremented value.
 * @param
 * stk - the stack
 * N - number of cells in one row of the board
 * board - game's board
 * count_hid - pointer to counter of hidden cells.
 */
void pop_till_inc (Stack* stk, int N, Num*** board, int* count_hid) {
	int col; int row; int val;
	/* pop all moves until we can increment the value or reach the end of stack*/
	while (stk->top != NULL && stk->top->val == N) { /*reset cells*/
		col = stk->top->col; /*temp*/
		row = stk->top->row; /*temp*/
		val = stk->top->val; /*temp*/
		board[stk->top->row][stk->top->col]-> alt_num = 0;
		(*count_hid)++;
		pop_stack(stk);
	}
	if (stk->top != NULL)  { /*increment value for cell means changing a valid cell to unkown validity cell*/
		col = stk->top->col;
		row = stk->top->row;
		val = stk->top->val;
		pop_stack(stk);
		(*count_hid)++;
		board[row][col]-> alt_num = 0;
		push_to_stack(val+1,row,col, stk);
	}
}

/*
 * ex_backtrack - executes exhaustive backtrack to find number of solutions of the board using a stack.
 * @param
 * board - game's board
 * m - number of rows in one block.
 * n - number of columns in one block.
 * count_hid - counter of hidden cells on board
 * @return
 * number of solutions of the board.
 */

int ex_backtrack (Num*** board, int n, int m, int count_hid) {
	int emp_row; int emp_col; int num_of_sols = 0; int N = n*m;
	int curr_row; int curr_col; int curr_val; Stack* stk;
	clear_fill_alt_board(board, n*m, 1);
	if (find_first_empty(board, n*m, &emp_row, &emp_col)) {
		stk = create_stack();
		push_to_stack(1,emp_row,emp_col,stk);
		while (stk->top != NULL) {
			curr_row = stk->top->row;
			curr_col = stk->top->col;
			curr_val = stk->top->val;
			if (validate_dig(curr_val,curr_row,curr_col,m,n,N,board,0,0)) { /*valid value for cell*/
				count_hid--;
				board[curr_row][curr_col]->alt_num = curr_val; /*updating board only when value is valid*/
				if (count_hid == 0) { /*found a solution*/
					num_of_sols++;
					pop_stack(stk);
					count_hid++;
					board[curr_row][curr_col]-> alt_num = 0;
					pop_till_inc(stk,N, board, &count_hid); /*backtrack*/
				}
				else { /*continue with next empty cell*/
					find_first_empty(board, n*m, &emp_row, &emp_col);
					push_to_stack(1,emp_row,emp_col,stk);
				}
			}
			else { /*value is not valid for this cell*/
				if (curr_val < N) { /*increment value of cell*/
					pop_stack(stk);
					push_to_stack(curr_val+1,curr_row,curr_col, stk);
				}
				else { /*no more options for this cell, backtrack*/
					pop_stack(stk);
					pop_till_inc(stk,N,board,&count_hid);
				}
			}
		}
	}
	else { /*got a full board*/
		clear_fill_alt_board(board, n*m, 0);
		return 1;
	}
	free(stk->top);
	free(stk);
	clear_fill_alt_board(board, n*m, 0);
	return num_of_sols;
}


/* ILP*/

/**
 * single_val_per_cell - adds the first constraint to the model: Single value per cell.
 * 
 * @param 
 * model - pointer to previously defined Gurobi model
 * N - number of cells in one block/row/col
 * error - pointer to binary variable that represents ILP algorithm status
 * ind - pointer to array of indices
 * val - pointer to array of values
 *
 * @return
 * 1 - error occurred while adding the constraint
 * 0 - otherwise
 *
 */
int single_val_per_cell(GRBmodel *model, int N, int *error, int  *ind, double *val){
	int i, j, k;
	
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      for (k = 0; k < N; k++) {
        ind[k] = i*N*N + j*N + k;
        val[k] = 1.0;
      }

      *error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
      if (*error) return *error;
    }
  }
	return 0;
	
}

/**
 * single_val_per_row - adds the second constraint to the model: Single value per row.
 * 
 * @param 
 * model - pointer to previously defined Gurobi model
 * N - number of cells in one block/row/col
 * error - pointer to binary variable that represents ILP algorithm status
 * ind - pointer to array of indices
 * val - pointer to array of values
 *
 * @return
 * 1 - error occurred while adding the constraint
 * 0 - otherwise
 *
 */
int single_val_per_row(GRBmodel *model, int N, int *error, int  *ind, double *val){
	int i, j, k;
	
  for (k = 0; k < N; k++) {
    for (j = 0; j < N; j++) {
      for (i = 0; i < N; i++) {
        ind[i] = i*N*N + j*N + k;
        val[i] = 1.0;
      }

      *error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
      if (*error) return *error;
    }
  }
	return 0;
	
}

/**
 * single_val_per_col - adds the third constraint to the model: Single value per column.
 * 
 * @param 
 * model - pointer to previously defined Gurobi model
 * N - number of cells in one block/row/col
 * error - pointer to binary variable that represents ILP algorithm status
 * ind - pointer to array of indices
 * val - pointer to array of values
 *
 * @return
 * 1 - error occurred while adding the constraint
 * 0 - otherwise
 *
 */
int single_val_per_col(GRBmodel *model, int N, int *error, int  *ind, double *val){
	int i, j, k;
	
  for (k = 0; k < N; k++) {
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        ind[j] = i*N*N + j*N + k;
        val[j] = 1.0;
      }

      *error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
      if (*error) return *error;
    }
  }
	return 0;
	
}

/**
 * single_val_per_block - adds the last constraint to the model: Single value per block.
 * 
 * @param 
 * model - pointer to previously defined Gurobi model
 * N - number of cells in one block/row/col
 * error - pointer to binary variable that represents ILP algorithm status
 * ind - pointer to array of indices
 * val - pointer to array of values
 * m - number of rows in one block
 * n - number of columns in one block
 *
 * @return
 * 1 - error occurred while adding the constraint
 * 0 - otherwise
 *
 */
int single_val_per_block(GRBmodel *model, int N, int *error, int  *ind, double *val, int n, int m){
	int i, j, k, col, row, count;
	
  for (k = 0; k < N; k++) {
    for (col = 0; col < n; col++) {
      for (row = 0; row < m; row++) {
        count = 0;
        for (i = col*m; i < (col+1)*m; i++) {
          for (j = row*n; j < (row+1)*n; j++) {
            ind[count] = i*N*N + j*N + k;
            val[count] = 1.0;
            count++;
          }
        }

				*error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
				if (*error) return *error;
			}
    }
  }
	return 0;
	
}

/**
 * save_results - saves results (received from ILP) to Sudoku board
 * 
 * @param 
 * board - the Sudoku board
 * N - number of cells in one block/row/col
 * y - array with results received from ILP
 *
 */
void save_results(Num*** board, int N, double *y){
	int i, j, k;
	
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				if(y[i*N*N + j*N + k]>0.5){
					board[i][j]->num = k+1;
					board[i][j]->status = SHOWN;
				}
			}
		}
	}
}


/**
 * define_model_vars - defines variables for the new model (such as:
 * $lb - array of lower bounds for the new variables.
 * $vtype - array of types of the new variables)
 * 
 * @param 
 * board - the Sudoku board
 * N - number of cells in one block/row/col
 * lb - array of lower bounds for the new variables
 * vtype - array of types of the new variables
 *
 */
void define_model_vars(Num*** board, int N, double *lb, char *vtype){
	int i, j, k;
	
	for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      for (k = 1; k <= N; k++) {
        if (board[i][j]->num == k)
          lb[i*N*N+j*N+k-1] = 1;
        else
          lb[i*N*N+j*N+k-1] = 0;
        vtype[i*N*N+j*N+k-1] = GRB_BINARY;
      }
    }
  }
}

/**
 * find_valid_assign - change the "res" variable to the valid number for requested cell
 * 
 * @param 
 * N - number of cells in one block/row/col
 * y - array with results received from ILP
 * h_x - column of required cell (used in hint command)
 * h_y - row of required cell (used in hint command)
 * res - pointer to "res" variable (stores the result)
 *
 */
void find_valid_assign(int N, double *y, int h_x, int h_y, int* res){
	int k;
	for (k = 0; k < N; k++) {
		if(y[h_y*N*N + h_x*N + k]>0.5) {
			*res = k+1;
			break;
		}
	}
	
}


/**
 * function solves Sudoku board with ILP. 
 * function defines binary variables: x[i,j,k] - indicate whether cell <i,j> takes value k.  
 * function sets constraints:
 *   1. Single value per cell (sum_k x[i,j,k] = 1)
 *   2. Single value per row (sum_i x[i,j,k] = 1)
 *   3. Single value per column (sum_j x[i,j,k] = 1)
 *   4. Single value per block (sum_ij x[i,j,k] = 1)
 * and solve the Sudoku board using Gurobi Optimizer. Each step's result is saved to "error" variable and 
 * if any error occurs, function frees the memory and returns 0.
 * For hint command, will be returned a valid number for requested cell
 * 
 * @param 
 * board - the Sudoku board
 * m - number of rows in one block
 * n - number of columns in one block
 * calling_func - the name of the function that calls ilp
 * h_x - column of required cell (used in hint command)
 * h_y - row of required cell (used in hint command)
 *
 * @return
 * 0 - an error occurred
 * 1 - no error occurred
 * k - valid number for requested cell (for hint command)
 *
 */
int ilp(Num*** board, int m, int n, char* calling_func, int h_x, int h_y) {
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
	int N = n*m;	int N3 = N*N*N;
	double *y = malloc(N3*sizeof(double));
  int *ind = malloc(N*sizeof(int));
  double *val = malloc(N*sizeof(double));
  double *lb = malloc(N3*sizeof(double));
  char *vtype = malloc(N3*sizeof(char));
	double objval;	int optimstatus;	int error = 0; int res = 1;

  define_model_vars(board, N, lb, vtype);

  /* Create environment */
  error = GRBloadenv(&env, "sudoku.log");
  if (error) goto QUIT;
	/* Disable console logging */
	error = GRBsetintparam(env, "LogToConsole", 0);
  if (error) goto QUIT;
  /* Create new model */
  error = GRBnewmodel(env, &model, "sudoku", N3, NULL, lb, NULL, vtype, NULL);
  if (error) goto QUIT;
	
	/* Constraints: */
	error = single_val_per_cell(model, N, &error, ind, val);
	if (error) goto QUIT;
	error = single_val_per_row(model, N, &error, ind, val);
	if (error) goto QUIT;
	error = single_val_per_col(model, N, &error, ind, val);
	if (error) goto QUIT;
	error = single_val_per_block(model, N, &error, ind, val, n ,m);
	if (error) goto QUIT;
	
  /* Optimize model */
  error = GRBoptimize(model);
  if (error) goto QUIT;
  /* Capture solution information */
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) goto QUIT;
  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) goto QUIT;
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N3, y);
	if (error) goto QUIT;
	
	/* Model was solved to optimality and an optimal solution is available */
  if (optimstatus == GRB_OPTIMAL){
		if (strcmp(calling_func,"gen") == 0)
			save_results(board, N, y);
		else if (strcmp(calling_func,"hint") == 0) {
			find_valid_assign(N, y, h_x, h_y, &res);
		}
  }

QUIT:
 /* Free model, environment, memory */
  GRBfreemodel(model);
  GRBfreeenv(env);
	free(y);
	free(ind);
	free(val);
	free(lb);
	free(vtype);
	
	return error ? 0 : res;
}

