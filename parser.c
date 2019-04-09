#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "main_aux.h"
#include "structs.h"
#include "game.h"
#include "solver.h"
#include "time.h"
#define DELIMITERS " \n\t\v\f\r"
#define COMMAND_LEN 256


/**
* using the function strtok, the function checks if the next token in command is legal.
* if token isn't legal, function prints an error message. if token is legal, function returns it. (a pointer to the first character of token)
*
* @param parsed_command - the current token.
* @return
* 0 - if next token got from strtok == NULL
* parsed_command (the next token) - otherwise
*/
char* check_next_tok(char* parsed_command) { 
	parsed_command = strtok(NULL,DELIMITERS);
	if (parsed_command == NULL) {
		print_invalid();
	}
	return parsed_command;
}

/**
 * check_integer:
 * @param
 * num - a double
 * @return
 * 1 if num is integer (its fraction is 0)
 * 0 otherwise
 */
int check_integer(double num) {
	if (num - (int) num == 0)
		return 1;
	return 0;
}

/**
 * read_args - reads the specified number of arguments from the given command and checks if they are legal.
 * function only checks proper number of arguments and integer value of them when needed.
 * if number is not integer, function changes it to -1.
 * function updates the values of parameters. prints error messages if values aren't legal.
 * @param
 * parsed_command - the given command
 * args_num - number of arguments of the proper command
 * x - pointer to variable x
 * y - pointer to variable y
 * z - pointer to variable z
 * func - name of calling function
 * @return
 * 1 - if command is legal or number is not integer
 * 0 - otherwise (illegal command)
 */
int read_args(char** parsed_command, int args_num, double* x, double* y, double* z, char* func) {
	*parsed_command = check_next_tok(*parsed_command);
	if (*parsed_command == NULL)
		return 0;
	*x = atof(*parsed_command);
	if (args_num > 1) {
		*parsed_command = check_next_tok(*parsed_command);
		if (*parsed_command == NULL)
			return 0;
		*y = atof(*parsed_command);
	}
	if (args_num > 2) {
		*parsed_command = check_next_tok(*parsed_command);
		if (*parsed_command == NULL)
			return 0;
		*z = atof(*parsed_command);
	}
	if ((!check_integer(*x) || (*x == 0.0 && **parsed_command != '0')) && strcmp(func,"solve") != 0 && strcmp(func,"save") != 0)
		*x = -1;
	if (args_num > 1 && (!check_integer(*y) || (*y == 0.0 && **parsed_command != '0')))
		*y = -1;
	if (args_num > 2 && (!check_integer(*z) || (*z == 0.0 && **parsed_command != '0')))
		*z = -1;
	return 1;
}


/**
* parse_file - The function parses a file and creates a board according to it and to the game's mode.
* @param
* fp - the file we are reading from
* board - pointer to the game board
* m - pointer to the number of rows in one block
* n - pointer to the number of columns in one block
* count_hid - pointer to the number of hidden cells on board
* mode - current game mode
* mark_errors - indicates whether errors should be marked
* @return
* 2 - board was properly loaded.
* 0 - an error occured and we exit the game.
*/
int parse_file (FILE* fp, Num**** board, int* m, int* n, int* count_hid, MODE mode, int mark_errors){
	char file_content [COMMAND_LEN];
	char* read_tok;
	int row = 0; int col = 0;
	int N; int dig; int valid;
	int count = -2; /*count the numbers on board */
	if (*board != NULL) {
		free_board(board,*m**n);
		*board = NULL;
	}
	while (fgets(file_content, COMMAND_LEN, fp) != NULL) {
		read_tok = strtok(file_content, DELIMITERS);
		while (read_tok != NULL) {
			if (count==-2)
				*m = atoi(read_tok);
			else if (count==-1)
				*n = atoi(read_tok);
			else if (count >= 0){
				if (count == 0) { /*initialization of board*/
					N = *n**m;
					*count_hid = N*N;
					*board = create_empty_board (*m, *n);
				}
				row = count/N;
				col = count%N;
				dig = atoi(read_tok);
				if (dig != 0) {
					*count_hid -= 1;
					valid = validate_dig(dig,row,col,*m,*n,N,*board,1,1);
					if (read_tok[strlen(read_tok)-1] == '.' && mode == SOLVE)
						(*board)[row][col]->status = FIXED;
					else if ( valid == 0)
						(*board)[row][col]->status = ERRONEOUS;
					else
						(*board)[row][col]->status = SHOWN;
					(*board)[row][col]->num = dig;
				}
			}
			count++;
			read_tok = strtok(NULL, DELIMITERS);
		}
	}
	if (ferror(fp)) { /* fgets error*/
		printf("Error: fgets has failed\n");
		return 0;
	}
	print_board (*board,*m,*n,mode,mark_errors);
	return 2;
}

/**
 * check_command_length - checks that the command is of legal value, if not, parses the command until it ends,
 * that is - we reach a new line or the end of file.
 * @param
 * user_command - user's command
 * @return
 * 1 - if command is of valid length
 * 0 - otherwise
 */
int check_command_length(char user_command[]) {
	int valid_command = 1;
	int ch;
	if (strlen(user_command) == COMMAND_LEN && user_command[COMMAND_LEN - 2] != '\n') {
	    while ((ch = fgetc(stdin)) != '\n' && ch != EOF) { /*get rid of the rest of command*/
	    	valid_command = 0;
	    }
	}
	return valid_command;
}

/**
* get_command -  parses through user's command using fgets and strtok.
* function checks if we reached EOF, if so calls exit_game.
* function checks if command is valid, and executes it if so.
* if command isn't valid, function prints error message.
* @param board - the game board
* @param n - number of rows in one block
* @param m - number of columns in one block
* @param count_hid - number of hidden cells on board
* @return
* 0 - if we exit the game
* 2 - if a command has been executed
* 3 - otherwise, meaning got an invalid command
*/
int get_command (Num**** board, int* m,int* n, int* count_hid, MODE* mode, int* mark_errors, MoveList** curr_move) {
	char user_command [COMMAND_LEN];
	double x = 0; double y = 0; double z = 0;
	char* parsed_command;
	printf("Enter your command:\n");
	if (fgets(user_command, COMMAND_LEN+1, stdin) == NULL) { /*read command*/
		if (feof(stdin)) /*end of file*/
			return (!exit_game(board,*n**m, curr_move));
		else {
			printf("Error: fgets has failed\n");
			return 3;
		}
	}
	if (!check_command_length(user_command)) {
		print_invalid();
		return 3;
	}
	parsed_command = strtok(user_command,DELIMITERS); /*parse command*/
	if (parsed_command != NULL) { /*got a word*/
		if (!strcmp(parsed_command,"validate"))
			return validate(*board, *m, *n, *mode, 1);
		else if (!strcmp(parsed_command,"reset"))
			return reset(*board,curr_move,*mode,count_hid,*m,*n, *mark_errors);
		else if (!strcmp(parsed_command,"hint")) {
			if (!read_args(&parsed_command, 2, &x,&y,&z,"hint"))
				return 3;
			return hint(*board,(int) x-1, (int) y-1,*m, *n, *mode);
		}
		else if (!strcmp(parsed_command,"set"))  {
			if (!read_args(&parsed_command, 3, &x,&y,&z,"set"))
				return 3;
			return(set((int) x-1,(int) y-1, (int) z,*board,*m,*n,count_hid,mode,curr_move,*mark_errors));
		}
		else if (!strcmp(parsed_command,"solve")) {
			if (!read_args(&parsed_command, 1, &x,&y,&z,"solve"))
				return 3;
			return solve(parsed_command,board,mode,curr_move,m,n,count_hid,*mark_errors);
		}
		else if (!strcmp(parsed_command,"edit")) {
			parsed_command = strtok(NULL,DELIMITERS);
			return edit(parsed_command,board,mode,curr_move,m,n,count_hid);
		}
		else if (!strcmp(parsed_command,"save")) {
			if (!read_args(&parsed_command, 1, &x,&y,&z,"save"))
				return 3;
			return save(*board,parsed_command,*m,*n,*mode);
		}
		else if (!strcmp(parsed_command,"generate")) {
			if (!read_args(&parsed_command, 2, &x,&y,&z,"gen"))
				return 3;
			return generate(*board,*m, *n, (int) x, (int) y, *mode, count_hid, curr_move);
		}
		else if (!strcmp(parsed_command,"autofill"))
			return (autofill(*board,*m,*n,mode,count_hid,curr_move,*mark_errors));
		else if (!strcmp(parsed_command,"mark_errors")) {
			if (!read_args(&parsed_command, 1, &x,&y,&z,"mark_errors"))
				return 3;
			return change_mark_errors(x, *mode, mark_errors);
		}
		else if (!strcmp(parsed_command,"print_board"))
			return print_board(*board, *m, *n, *mode, *mark_errors);
		else if (!strcmp(parsed_command,"undo"))
			return (undo (*mode,curr_move,*board,count_hid,*m,*n, *mark_errors, 1));
		else if (!strcmp(parsed_command,"redo"))
			return (redo (*mode,curr_move,*board,count_hid,*m,*n, *mark_errors));
		else if (!strcmp(parsed_command,"num_solutions"))
			return num_of_solutions(*board,*m,*n,*count_hid,*mode);
		else if (!strcmp(parsed_command,"exit"))
			return (!exit_game(board,*n**m, curr_move));
		else {
			print_invalid();
			return 3;
		}
	}
	if (!isspace(user_command[0])) /*could get here for a space which is not an invalid command */
		print_invalid();
	return 3;
}
