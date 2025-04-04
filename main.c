#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "libs/conversor.h"
#include "libs/gameRules.h"
#include "libs/string.h"
int main(int argC, char** args){
	if (argC != 4){
		printf("invalid argument count");
		return 0;
	}
	int soldiers = strtoimax(args[1], NULL, 10);
	fprintf(stderr ,"%d soldiers \n", soldiers);
	char cwd[1024];
	getcwd(cwd, 1024);
	Soldier* blue;
	Soldier* red;
	String* dir = ptrToStr(cwd);
	appendPtr(dir, "/", 1);
	size_t len = dir->length;
	appendNoLen(dir, args[2], 128);
	fprintf(stderr, "red team code file = \"%s\"\n", dir->string);
	FILE* code = fopen(dir->string, "rb+");
	if (code != NULL){
		red = translate(code);
	} else {
		printf("FAILED TO READ RED TEAM'S SCRIPT\n");
		return 0;
	}
	fclose(code);
	dir->length = len;
	dir->string[len] = '\0';
	//fprintf(stderr, "bef %s", dir->string);
	appendNoLen(dir, args[3], 128);
	dir->string[dir->length] = '\0';
	fprintf(stderr, "blue team code file = \"%s\" %d \n", dir->string, dir->length);
	code = fopen(dir->string, "rb+");
	if (code != NULL) {
		blue = translate(code);
		//fclose(code);
		//while (blue->curr < blue->instruction_total-1){
		//RUN(blue);
		//sleep(1);
		//}
		//printf("## PROGRAM END ##\n");
	} else {
		printf("FAILED TO READ BLUE TEAM'S SCRIPT\n");
		return 0;
	}
	fclose(code);
	discardStr(dir);
	init_game(red, blue, soldiers);
	game_loop();
	return 0;
}
