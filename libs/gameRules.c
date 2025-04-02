#include "conversor.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#define ROW 1
#define COL 0
#define RED_TEAM 1
#define BLUE_TEAM 2
#define ATK_COL 3
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
void display_update(){
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
	init_pair(BLUE_TEAM, COLOR_BLUE, COLOR_BLACK);
	init_pair(ATK_COL, COLOR_WHITE, COLOR_BLACK);
	noecho();
	curs_set(0);
	printf("starting display\n");
	display_update();
}
void game_move_to(int p_x, int p_y, int n_x, int n_y, Soldier* target){
	int id = board[convert_1d(p_x, p_y)];
	board[convert_1d(p_x, p_y)] = INT_MIN;
	board[convert_1d(n_x, n_y)] = id;
	target->vars[SOL_X] = n_x;
	target->vars[SOL_Y] = n_y;
	//fprintf(stderr, "moved from X:%d Y:%d to X:%d Y:%d\n", p_x, p_y, n_x, n_y);
}
void attack_try(Soldier* user, int x_change, int y_change){
	fprintf(stderr, "attack attempt\n");
	if (user->vars[SOL_X] + x_change >= board[COL] || user->vars[SOL_Y] + y_change >= board[ROW]){
		return;
	}
	if (user->vars[SOL_X] + x_change < 0 || user->vars[SOL_Y] + y_change < 0){
		return;
	}
	int results = game_check_at(user->vars[SOL_X] + x_change, user->vars[SOL_Y] + y_change);
		board[convert_1d(user->vars[SOL_X] + x_change, user->vars[SOL_Y] + y_change)] = 0;
	atk_tiles[atk_ct] = convert_1d(user->vars[SOL_X] + x_change, user->vars[SOL_Y] + y_change);
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
void game_step(char quantity){
	for (char i = 0; i < quantity; i++){
		for (int j = 0; j < base_ct; j++){
			RUN(&red_team[j]);
			RUN(&blue_team[j]);
		}
	}
	display_update();
}
void game_loop(){
	while(1){
		usleep(100000);
		game_step(1);
		if (red_ct == 0){
			endwin();
			break;
		}
		if (blue_ct == 0){
			endwin();
			break;
		}
	}
}
