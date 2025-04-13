#include "conversor.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#define ROW 1
#define COL 0
#define RED_TEAM 1
#define RED_WIN 2
#define BLUE_TEAM 3
#define BLUE_WIN 4
#define ATK_COL 5
/*board of all soldiers*/
int* board = NULL;
int red_ct = 0;
int blue_ct = 0;
int base_ct = 0;
int* atk_tiles;
int atk_ct;
Soldier* red_team = NULL;
Soldier* blue_team = NULL;
/* returns the soldier at the set coordinates.*/
int game_check_at(int x, int y){
	return board[2+x+y*board[COL]];
}
int convert_1d(int x, int y){
	return 2+x+y*board[COL];
}
int pause_show = 0;
int paused = 0;
unsigned int timer = 0;
void display_update(char flush_c){
	if (paused == 0 || flush_c == 1){
		move(0,0);
		unsigned int y = 0;
		unsigned int x = 0; 
		for (y = 0; y < board[ROW]; y++){
			for (x = 0; x < board[COL]; x++){
				if (board[convert_1d(x,y)] != INT_MIN){
					if (board[convert_1d(x,y)] == 0){
						attron(COLOR_PAIR(ATK_COL));
						printw("~");
					} else { 
						if (board[convert_1d(x,y)] > 0){
							attron(COLOR_PAIR(RED_TEAM));
						} else {
							attron(COLOR_PAIR(BLUE_TEAM));
						}
						printw("#");
					}
				} else {
					printw(" ");
				}
			}
		}
		int i = 0;
		for (i = 0; i < atk_ct; i++){
			board[atk_tiles[i]] = INT_MIN;	
		}
		atk_ct = 0;
	} else {
		move(0,0);
		pause_show += 17;
		if (pause_show < 1000){
			attron(COLOR_PAIR(ATK_COL));
			printw("PAUSED!");
		} else {
			if (pause_show > 2000){
				pause_show = 0;
			}
			int i = 0;
			for (i = 0; i < 7; i++){
				if (board[i+2] != INT_MIN){
					if (board[i+2] == 0){
						attron(COLOR_PAIR(ATK_COL));
						printw("~");
					} else { 
						if (board[i+2] > 0){
							attron(COLOR_PAIR(RED_TEAM));
						} else {
							attron(COLOR_PAIR(BLUE_TEAM));
						}
						printw("#");
					}
				} else {
					printw(" ");
				}
			}
		}
	}
	refresh();
}
void init_game(Soldier* red_team_snippet, Soldier* blue_team_snippet, int soldier_count){
	initscr();

	if (COLS < 5){
		printf("width of terminal is lower than 20, which is the minimum width.\n");
		exit(0);
	}
	if (COLS > 200){
		printf("width of terminal is higher than 200, which is the maximum width.\n");
		exit(0);
	}
	if (LINES < 5){
		printf("height of terminal is lower than 20, which is the minimum height.\n");
		exit(0);
	}
	if (LINES > 200){
		printf("height of terminal is higher than 200, which is the maximum height.\n");
		exit(0);
	}
	board = malloc((COLS * LINES + 2) * sizeof(int));
	int i;
	for (i = 2; i < COLS * LINES + 2; i++){
		board[i] = INT_MIN;
	}
	printf("COLS: %d LINES: %d\n", COLS, LINES);
	board[COL] = COLS;
	board[ROW] = LINES;
/*	for (int i = 0; i < board[ROW]; i++){
		printf("[");
		for (int j = 0; j < board[COL]; j++){
			printf(" %d,", game_check_at(j,i));
		}
		printf("]\n");
	}*/
	/*getting closest square size*/
	char side = 1;
	while (side*side < soldier_count){
		side++;
	}
	atk_tiles = malloc(sizeof(int) * soldier_count * 2);
	red_team = (Soldier*) malloc(sizeof(Soldier) * soldier_count);
	blue_team = (Soldier*) malloc(sizeof(Soldier) * soldier_count);
	red_ct = soldier_count;
	blue_ct = soldier_count;
	base_ct = soldier_count;
	int count_cp = soldier_count;
	while (soldier_count > 0){
		red_team[soldier_count-1] = *red_team_snippet;
		red_team[soldier_count-1].vars[SOL_ID] = soldier_count;
		blue_team[soldier_count-1] = *blue_team_snippet;
		blue_team[soldier_count-1].vars[SOL_ID] = -soldier_count;
		soldier_count--;
	}
    count_cp--;
	int row = 0;
	int col = 0;
	for (i = 0; i < side; i++){
		if (count_cp == -1){
			break;
		}
		int j = 0;
		for (j = 0; j < side; j++){
			/*places at upper left.*/
			board[convert_1d(col, row)] = red_team[count_cp].vars[SOL_ID];
			red_team[count_cp].vars[SOL_X] = col;
			red_team[count_cp].vars[SOL_Y] = row;
			/* places at bottom right.*/
			board[convert_1d(board[COL] - col-1, board[ROW]-row-1)] = blue_team[count_cp].vars[SOL_ID];
			blue_team[count_cp].vars[SOL_X] = board[COL]-col-1;
			blue_team[count_cp].vars[SOL_Y] = board[ROW]-row-1;
			count_cp--;
			col++;
			if (count_cp == -1){
				break;
			}
		}	
		/*reset X modifier and change Y modifier.*/
		col = 0;
		row++;
	}
	/*for (int i = 0; i < board[ROW]; i++){
			if (game_check_at(j,i) != INT_MIN){
				printf("ID FOUND %d at index %d\n", game_check_at(j,i), convert_1d(j,i));
			}
		}
	}*/
	start_color();
	init_pair(RED_TEAM, COLOR_RED, COLOR_BLACK);
	init_pair(RED_WIN, COLOR_WHITE, COLOR_RED);
	init_pair(BLUE_TEAM, COLOR_CYAN, COLOR_BLACK);
	init_pair(BLUE_WIN, COLOR_WHITE, COLOR_CYAN);
	init_pair(ATK_COL, COLOR_WHITE, COLOR_BLACK);
	noecho();
	nodelay(stdscr, 1);
	curs_set(0);
	display_update(1);
}
void game_move_to(int p_x, int p_y, int n_x, int n_y, Soldier* target){
	int id = board[convert_1d(p_x, p_y)];
	board[convert_1d(p_x, p_y)] = INT_MIN;
	board[convert_1d(n_x, n_y)] = id;
	target->vars[SOL_X] = n_x;
	target->vars[SOL_Y] = n_y;
}
int check_try(Soldier* user, int x_change, int y_change){
	int team = user->vars[SOL_ID] > 0;
	if (user->vars[SOL_X] + x_change >= board[COL] || user->vars[SOL_Y] + y_change >= board[ROW]){
		return -2;
	}
	if (user->vars[SOL_X] + x_change < 0 || user->vars[SOL_Y] + y_change < 0){
		return -2;
	}
	int result = game_check_at(user->vars[SOL_X] + x_change, user->vars[SOL_Y] + y_change);
	if (result == INT_MIN || result == 0){
		return 0;
	}
	return ((result > 0) == team) - ((result > 0) != team);
}

void game_end(int C_PAIR){
	display_update(1);
	move(0,0);
	attron(COLOR_PAIR(C_PAIR));
	int i = 0;
	for (i = 0; i < board[ROW] * board[COL]; i++){
		usleep(5000000 / (board[COL] * board[ROW]));
		printw(" ");
		refresh();
	}
	move(0,0);
	if (C_PAIR == RED_WIN){
		printw("RED");
	} else {
		printw("BLUE");
	}
	printw(" team wins!");
	refresh();
	sleep(3);
	endwin();
	while (getch() != ERR);
	exit(0);
}
int game_secure(Soldier* user, int dir){
	/* 1 FOR RED, 0 FOR BLUE.*/
	char user_team = (user->vars[SOL_ID] > 0);
	int x = user->vars[SOL_X];
	int y = user->vars[SOL_Y];
	int results = 0;
	int i = 0;
	for (i = 0; i < user->vars[SOL_STAT] + 1; i++){
		y += (dir == 2) - (dir == 0);
		if (y > board[ROW] || y < 0){
			return 1;
		}
		x += (dir == 1) - (dir == 3);
		if (x > board[COL] || x < 0){
			return 1;
		}
		results = game_check_at(x,y);
		if (((results > 0) == user_team) && results != INT_MIN && results != 0){
			return 0;
		} else if (((results > 0) != user_team) && results != INT_MIN && results != 0) {
		return 1;
		}
	}
	return 1;
}
void attack_try(Soldier* user, int dir, int distance){
	/*fprintf(stderr, "attack attempt\n");*/
	int results = 0;
	int x = user->vars[SOL_X];
	int y = user->vars[SOL_Y];
	while (distance > 0){
		y += (dir == 2) - (dir == 0);
		if (y > board[ROW]-1 || y < 0){
			return;
		}
		x += (dir == 1) - (dir == 3);
		if (x > board[COL]-1 || x < 0){
			return;
		}
		results = game_check_at(x,y);
		if (results != INT_MIN && results != 0){
			break;
		}
		distance--;
	}	
	board[convert_1d(x, y)] = 0;
	atk_tiles[atk_ct] = convert_1d(x, y);
	atk_ct++;
	if (results == INT_MIN || results == 0){
		return;
	}
	if (results > 0){
		red_team[results-1].curr = -2;
		red_ct--;
	} else {
		blue_team[-(results+1)].curr = -2;
		blue_ct--;
	}
	if (red_ct == 0){
			game_end(BLUE_WIN);
	}
	if (blue_ct == 0){
			game_end(RED_WIN);
	}
}
void seek_try(Soldier* soldier, int ptrX, int ptrY){
	int x = soldier->vars[SOL_X];
	int y = soldier->vars[SOL_Y];
	int length = 0;
	/* 0 for blue, 1 for red*/
	int team = soldier->vars[SOL_ID] > 0;
	int result = 0;
	/* THIS SHOULD BE GRANTED TO RETURN, AND NOT RUN AFTER A TEAM DIES.*/
	while (length < 6){
		/* ADD ONE TO X TO ACESS NEXT DIAGONAL.*/
		x++;
		length++;
		/*DOWN LEFT*/
		if (!(x >= board[COL] || y >= board[ROW] || x < 0 || y < 0)){
			result = game_check_at(x,y);
		}
		if (result != INT_MIN && ((result > 0) != team) && result != 0){
			soldier->vars[ptrX] = x - soldier->vars[SOL_X];
			soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
			soldier->vars[TMP_RET] = 1;
			return;
		}
		int i = 0;
		for (i = 0; i < length; i++){
			x--;
			y--;
			if (x >= board[COL] || y >= board[ROW] || x < 0 || y < 0){
				continue;
			}
			int result = game_check_at(x,y);
			if (result != INT_MIN && ((result > 0) != team) && result != 0){
				soldier->vars[ptrX] = x - soldier->vars[SOL_X];
				soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
				soldier->vars[TMP_RET] = 1;
				return;
			}
		}
		/* UP LEFT*/
		for (i = 0; i < length; i++){
			x--;
			y++;
			if (x >= board[COL] || y >= board[ROW] || x < 0 || y < 0){
				continue;
			}
			int result = game_check_at(x,y);
			if (result != INT_MIN && ((result > 0) != team) && result != 0){
				soldier->vars[ptrX] = x - soldier->vars[SOL_X];
				soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
				soldier->vars[TMP_RET] = 1;
				return;
			}
		}
		/* UP RIGHT */
		for (i = 0; i < length; i++){
			x++;
			y++;
			if (x >= board[COL] || y >= board[ROW] || x < 0 || y < 0){
				continue;
			}
			int result = game_check_at(x,y);
			if (result != INT_MIN && ((result > 0) != team) && result != 0){
				soldier->vars[ptrX] = x - soldier->vars[SOL_X];
				soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
				soldier->vars[TMP_RET] = 1;
				return;
			}
		}
		/*DOWN RIGHT*/
		for (i = 0; i < length; i++){
			x++;
			y--;
			if (x >= board[COL] || y >= board[ROW] || x < 0 || y < 0){
				continue;
			}
			int result = game_check_at(x,y);
			if (result != INT_MIN && ((result > 0) != team) && result != 0){
				soldier->vars[ptrX] = x - soldier->vars[SOL_X];
				soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
				soldier->vars[TMP_RET] = 1;
				return;
			}
		}
	}
	soldier->vars[ptrX] = 0;
	soldier->vars[ptrY] = 0;
	soldier->vars[TMP_RET] = 0;
}
int move_try(Soldier* soldier, int x_change, int y_change){
	if (soldier->vars[SOL_X] + x_change >= board[COL] || soldier->vars[SOL_Y] + y_change >= board[ROW]){
		return 1;
	}
	if (soldier->vars[SOL_X] + x_change < 0 || soldier->vars[SOL_Y] + y_change < 0){
		return 1;
	}
	int results = game_check_at(soldier->vars[SOL_X] + x_change, soldier->vars[SOL_Y] + y_change); 
	if (results != INT_MIN && results != 0){
		return 1;
	}
	game_move_to(soldier->vars[SOL_X], soldier->vars[SOL_Y], soldier->vars[SOL_X] + x_change, soldier->vars[SOL_Y] + y_change, soldier);
	return 0;
}
void game_step() {
	int j = 0;
	for (j = 0; j < base_ct; j++){
		RUN(&red_team[j]);
		RUN(&blue_team[j]);
	}
}

int key_code = 0;
void game_loop(){
	paused = 0;
	while(1){
		usleep(16666);
		timer += 16666;
		while ((key_code = getch()) != ERR){
			switch(key_code){
				case 'd':
					game_step();
					display_update(1);
					break;
				case 'D':
					game_step();
					display_update(1);
					break;
				case 'p':
					paused = !(paused == 1);
					break;
				case KEY_RESIZE:
					endwin();
					fprintf(stderr, "terminated due to terminal resize.\n");
					exit(1);
					break;
			}
		}
		if (paused == 0 && timer > 16666 * 3 -1){
			timer = 0;
			game_step();
		}	
		display_update(0);
	}
}

void print_help(char* arg){
	if (arg == NULL){
		printf("no soldier command specified, use one of those as arguments:\nATTACK\nCHARGE\nCHECK\nCON_GOTO\nGOTO\nGUIDE\nLET\nMATH\nMOVE\nPREDEFINED\nRAND\nSECURE\nSEEK\n");
		return;
	}
	if (strcmp(arg, "GUIDE") == 0){
		printf("- - - - GUIDE - - - -\nthis is a guide on how to read the instruction syntax, which is composed by 3 params, the first is <return>, which indicates if it writes it's result to any variable space, the second is NAME, which is the name of the instruction, and the third is it's arguments and it's possible states (i.e. similar to \"(VAR/CONST arg1, arg2)\", var meaning a variable, and const a numerical constant).\n");
	return;
	}
	if (strcmp(arg, "ATTACK") == 0){
		printf("- - - - ATTACK - - - -\n-> <NULL> ATTACK (VAR/CONST direction)\nasks for the soldier to attack in direction, having a range of SOL_STAT + 1, setting SOL_STAT to 0.\n");
		return;
	}
	if (strcmp(arg, "CHARGE") == 0){
		printf("- - - - CHARGE - - - -\n-> <NULL> CHARGE ()\nincreases the SOL_STAT variable by 1, effectively increasing the range of the next attack.\n");
		return;
	}
	if (strcmp(arg, "CHECK") == 0){
			printf("- - - - CHECK - - - -\n-> <TMP_RET> CHECK (VAR/CONST relative_x, VAR/CONST relative_y)\nsets TMP_RET to -2 if the targetted spot is out of bounds, -1 if it's occupied by an enemy, or 1 if it's occupied by an ally.\n");
		return;
	}
	if (strcmp(arg, "CON_GOTO") == 0){
		printf("- - - - CON_GOTO - - - -\n-> <TMP_RET> CON_GOTO (CONST line, COMPARISON)\nsets TMP_RET to the result of the comparison, and goes to the targetted line if the comparison is true.\n");
		return;
	}
	if (strcmp(arg, "GOTO") == 0){
		printf("- - - - GOTO - - - -\n-> <NULL> GOTO (CONST line)\njumps to the targeted line.\n");
		return;
	}
	if (strcmp(arg, "LET") == 0){
		printf("- - - - LET - - - -\n-> <NULL> LET (CONST STRING name)\ndeclares a variable, which can be used in place of VAR arguments.\n");
		return;
	}
	if (strcmp(arg, "MATH") == 0){
		printf("- - - - MATH - - - -\n-> <TMP_MATH> NO_NAME (MATH EQUATION)\nrequires an assigment to a variable, sets the variable and TMP_MATH to the result of the equation, is calculated strictly in the order the symbols appear, can not have functions in it's body.\n");
		return;
	}
	if (strcmp(arg, "MOVE") == 0){
			printf("- - - - MOVE - - - -\n-> <TMP_RET> MOVE (VAR/CONST direction)\nattempts to move the soldier in the direction specified, can be assigned to a variable, return 1 if successful, and 0 if not.\n");
	return;
	}
if (strcmp(arg, "PREDEFINED") == 0){
			printf("- - - - PREDEFINED VARIABLES - - - -\nthere are some predefined variables at compilation time, which are separated in soldier-specific and game specific\n- - SOLDIER SPECIFIC VARS - -\nTMP_RET, address used for returns, read-only\nTMP_MATH address used to write math results\nSOL_X represents the soldier's X coordinate, read-only\nSOL_Y represents the soldier's Y coordinate\nSOL_STAT, represent's the ammount of \'charge\' a soldier has, read-only\nSOL_ID represents the ID number of the soldier, in the red team it is positive, in the blue team it is negative, will never be zero, read only\n- - GAME SPECIFIC VARS - -\nWORLD_W, specifies the map width.\nWORLD_H specifies the map height.\nSOL_CT, specifies the quantity of soldiers per team.\n");
	return;
	}
	if (strcmp(arg, "RAND") == 0){
			printf("- - - - RAND - - - -\n-> <TMP_RET> RAND (VAR/CONST min, VAR/CONST max)\ngenerates a random number from min to max, writing it to TMP_RET, may be assigned to a variable.\n");
	return;
	}
	if (strcmp(arg, "SECURE") == 0){
			printf("- - - - SECURE - - - -\n-> <TMP_RET> SECURE (VAR/CONST direction)\nchecks if the attack hits an ally in direction, writing 1 to TMP_RET if not, and 0 if it does, may be assigned to a variable.\n");
	return;
	}
	if (strcmp(arg, "SEEK") == 0){
			printf("- - - - SEEK - - - -\n-> <TMP_RET> SEEK (VAR targetX, VAR/CONST targetY)\nwrites the coordinates of the closest enemy to targetX, targetY, relative to the soldier's X, and Y, also setting TMP_RET to 1, if there is no enemy in a radius of 5, sets targetX, targetY and TMP_RET to 0, can be assigned.\n");
	return;
	}
}

