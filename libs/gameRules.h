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
extern int* board;
extern int red_ct;
extern int blue_ct;
extern int base_ct;
extern int* atk_tiles;
extern int atk_ct;
extern Soldier* red_team;
extern Soldier* blue_team;
/* returns the soldier at the set coordinates.*/
int game_check_at(int x, int y);
int convert_1d(int x, int y);
void display_update(char flush_c);	
void init_game(Soldier* red_team_snippet, Soldier* blue_team_snippet, int soldier_count);
void game_move_to(int p_x, int p_y, int n_x, int n_y, Soldier* target);
int check_try(Soldier* user, int x_change, int y_change);
void game_end(int C_PAIR);
int game_secure(Soldier* user, int dir);
void attack_try(Soldier* user, int dir, int distance);
void seek_try(Soldier* soldier, int ptrX, int ptrY);
int move_try(Soldier* soldier, int x_change, int y_change);
void game_step();
extern int key_code;
void game_loop();
void print_help(char* arg);
