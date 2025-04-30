#include "conversor.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdint.h>
#define ROW 1
#define COL 0
#define RED_TEAM 1
#define RED_WIN 2
#define BLUE_TEAM 3
#define BLUE_WIN 4
#define ATK_COL 5
/*board of all soldiers*/
extern int32_t* board;
extern int32_t red_ct;
extern int32_t blue_ct;
extern int32_t base_ct;
extern int32_t* atk_tiles;
extern int32_t atk_ct;
extern Soldier* red_team;
extern Soldier* blue_team;
/* returns the soldier at the set coordinates.*/
void display_update(char flush_c);	
void init_game(Soldier* red_team_snippet, Soldier* blue_team_snippet, int32_t soldier_count);
void game_move_to(int32_t p_x, int32_t p_y, int32_t n_x, int32_t n_y, Soldier* target);
int32_t check_try(Soldier* user, int32_t x_change, int32_t y_change);
void game_end(int32_t C_PAIR);
int32_t game_secure(Soldier* user, int32_t dir);
void attack_try(Soldier* user, int32_t dir, int32_t distance);
void seek_try(Soldier* soldier, int32_t ptrX, int32_t ptrY);
int32_t move_try(Soldier* soldier, int32_t x_change, int32_t y_change);
void game_step();
extern int32_t key_code;
void game_loop();
void print_help(char* arg);
