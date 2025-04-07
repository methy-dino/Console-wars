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
//board of all soldiers
int* board = NULL;
int red_ct = 0;
int blue_ct = 0;
int base_ct = 0;
int* atk_tiles;
int atk_ct;
Soldier* red_team = NULL;
Soldier* blue_team = NULL;
// returns the soldier at the set coordinates.
int game_check_at(int x, int y){
	//printf("checking index: %d\n", 2+x*board[COL]+y*board[ROW]);
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
		for (unsigned int y = 0; y < board[ROW]; y++){
			for (unsigned int x = 0; x < board[COL]; x++){
				if (board[convert_1d(x,y)] != INT_MIN){
					if (board[convert_1d(x,y)] == 0){
						attron(COLOR_PAIR(ATK_COL));
						printw("~");
					} else { 
						if (board[convert_1d(x,y)] > 0){
							//printf("found red at: %d, %d\n",x,y);
							attron(COLOR_PAIR(RED_TEAM));
						} else {
							//printf("found blue at: %d, %d\n",x,y);
							attron(COLOR_PAIR(BLUE_TEAM));
						}
						printw("#");
					}
				} else {
					printw(" ");
				}
			}
		}
		for (int i = 0; i < atk_ct; i++){
			board[atk_tiles[i]] = INT_MIN;	
		}
		atk_ct=0;
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
			for (int i = 0; i < 7; i++){
				if (board[i+2] != INT_MIN){
					if (board[i+2] == 0){
						attron(COLOR_PAIR(ATK_COL));
						printw("~");
					} else { 
						if (board[i+2] > 0){
							//printf("found red at: %d, %d\n",x,y);
							attron(COLOR_PAIR(RED_TEAM));
						} else {
							//printf("found blue at: %d, %d\n",x,y);
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
		printf("width of terminal is lower than 20, which is the minimum width\n");
		exit(0);
	}
	if (COLS > 200){
		printf("width of terminal is higher than 200, which is the maximum width\n");
		exit(0);
	}
	if (LINES < 5){
		printf("height of terminal is lower than 20, which is the minimum height\n");
		exit(0);
	}
	if (LINES > 200){
		printf("height of terminal is higher than 200, which is the maximum height\n");
		exit(0);
	}
	board = malloc((COLS * LINES + 2) * sizeof(int));
	//printf("allocated %d ints", COLS * LINES + 2);
	for (int i = 2; i < COLS * LINES + 2; i++){
		//printf("%d i\n", i);
		board[i] = INT_MIN;
		//printf("huh %d\n", i);
	}
	printf("COLS: %d LINES: %d", COLS, LINES);
	board[COL] = COLS;
	board[ROW] = LINES;
/*	for (int i = 0; i < board[ROW]; i++){
		printf("[");
		for (int j = 0; j < board[COL]; j++){
			printf(" %d,", game_check_at(j,i));
		}
		printf("]\n");
	}*/
	//getting closest square size
	char side = 1;
	while (side*side < soldier_count){
		side++;
	}
	//printf("starting soldier creation\n");
	atk_tiles = malloc(sizeof(int) * soldier_count * 2);
	red_team = (Soldier*) malloc(sizeof(Soldier) * soldier_count);
    //printf("allocated: %d bytes", 
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
		//printf("id ref; %d\n", soldier_count);
		soldier_count--;
		//printf("%d sols remaining\n", soldier_count);
	}
	//TODO do placing logic.
    count_cp--;
	int row = 0;
	int col = 0;
	for (int i = 0; i < side; i++){
		if (count_cp == -1){
			break;
		}
		for (int j = 0; j < side; j++){
			//places at upper left.
			//printf("id; %d \n", red_team[count_cp-1].vars[SOL_ID]); 
			board[convert_1d(col, row)] = red_team[count_cp].vars[SOL_ID];
			red_team[count_cp].vars[SOL_X] = col;
			red_team[count_cp].vars[SOL_Y] = row;
			//printf("red inset %d with id: %d as index of list number %d\n", convert_1d(row, col), red_team[count_cp].vars[SOL_ID], count_cp); 
			// places at bottom right.
			board[convert_1d(board[COL] - col-1, board[ROW]-row-1)] = blue_team[count_cp].vars[SOL_ID];
			blue_team[count_cp].vars[SOL_X] = board[COL]-col-1;
			blue_team[count_cp].vars[SOL_Y] = board[ROW]-row-1;
			//printf("blu inset %d with id: %d as index of list number %d\n", convert_1d(board[COL] - col-1, board[ROW]-row-1), blue_team[count_cp].vars[SOL_ID], count_cp); 
			count_cp--;
			col++;
			if (count_cp == -1){
				break;
			}
		}	
		//reset X modifier and change Y modifier.
		col = 0;
		row++;
	}
	/*for (int i = 0; i < board[ROW]; i++){
		//printf("[");
		for (int j = 0; j < board[COL]; j++){
			if (game_check_at(j,i) != INT_MIN){
				printf("ID FOUND %d at index %d\n", game_check_at(j,i), convert_1d(j,i));
			}
		}
	//	printf("]\n");
	}*/
	//printf("]\n");
	//initscr();
	start_color();
	init_pair(RED_TEAM, COLOR_RED, COLOR_BLACK);
	init_pair(RED_WIN, COLOR_WHITE, COLOR_RED);
	init_pair(BLUE_TEAM, COLOR_CYAN, COLOR_BLACK);
	init_pair(BLUE_WIN, COLOR_WHITE, COLOR_CYAN);
	init_pair(ATK_COL, COLOR_WHITE, COLOR_BLACK);
	noecho();
	nodelay(stdscr, 1);
	curs_set(0);
	printf("starting display\n");
	display_update(1);
}
void game_move_to(int p_x, int p_y, int n_x, int n_y, Soldier* target){
	int id = board[convert_1d(p_x, p_y)];
	board[convert_1d(p_x, p_y)] = INT_MIN;
	board[convert_1d(n_x, n_y)] = id;
	target->vars[SOL_X] = n_x;
	target->vars[SOL_Y] = n_y;
	//fprintf(stderr, "moved from X:%d Y:%d to X:%d Y:%d\n", p_x, p_y, n_x, n_y);
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
	for (int i = 0; i < board[ROW] * board[COL]; i++){
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
	// 1 FOR RED, 0 FOR BLUE.
	char user_team = (user->vars[SOL_ID] > 0);
	int x = user->vars[SOL_X];
	int y = user->vars[SOL_Y];
	int results = 0;
	for (int i = 0; i < user->vars[SOL_STAT] + 1; i++){
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
	//fprintf(stderr, "attack attempt\n");
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
	int tmp_x = 0;
	int y = soldier->vars[SOL_Y];
	int tmp_y = 0;
	int length = 0;
	// 0 for blue, 1 for red
	int team = soldier->vars[SOL_ID] > 0;
	int result = 0;
	// THIS SHOULD BE GRANTED TO RETURN, AND NOT RUN AFTER A TEAM DIES.
	while (length < 6){
		// DONE DIAGONALS.
		// ADD ONE TO X TO ACESS NEXT DIAGONAL.
		x++;
		length++;
		//DOWN LEFT
		if (!(x >= board[COL] || y >= board[ROW] || x < 0 || y < 0)){
			int result = game_check_at(x,y);
		}
		if (result != INT_MIN && ((result > 0) != team) && result != 0){
			soldier->vars[ptrX] = x - soldier->vars[SOL_X];
			soldier->vars[ptrY] = y - soldier->vars[SOL_Y];
			soldier->vars[TMP_RET] = 1;
			return;
		}
		for (int i = 0; i < length; i++){
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
		// UP LEFT
		for (int i = 0; i < length; i++){
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
		// UP RIGHT
		for (int i = 0; i < length; i++){
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
		//DOWN RIGHT
		for (int i = 0; i < length; i++){
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
	//fprintf(stderr, "%d %d %d %d max check !! \n", soldier->vars[SOL_X] + x_change, board[COL], soldier->vars[SOL_Y] + y_change, board[ROW]);
	if (soldier->vars[SOL_X] + x_change >= board[COL] || soldier->vars[SOL_Y] + y_change >= board[ROW]){
		//fprintf(stderr, "move try failed, out of bounds (+)\n");
		return 1;
	}
	if (soldier->vars[SOL_X] + x_change < 0 || soldier->vars[SOL_Y] + y_change < 0){
		//fprintf(stderr, "move try failed, out of bounds (-)\n");
		return 1;
	}
	int results = game_check_at(soldier->vars[SOL_X] + x_change, soldier->vars[SOL_Y] + y_change); 
	if (results != INT_MIN){
		//fprintf(stderr, "move try failed, space occupied\n");
		return 1;
	}
	game_move_to(soldier->vars[SOL_X], soldier->vars[SOL_Y], soldier->vars[SOL_X] + x_change, soldier->vars[SOL_Y] + y_change, soldier);
	return 0;
}
void game_step(){
	for (int j = 0; j < base_ct; j++){
		RUN(&red_team[j]);
		RUN(&blue_team[j]);
	}
	//display_update();
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
