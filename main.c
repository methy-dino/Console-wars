#define _POSIX_C_SOURCE 199309L
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include "libs/conversor.h"
#include "libs/gameRules.h"
#include "libs/string.h"
#include <string.h>
int main(int argC, char** args){
	if (argC > 1 && strcmp(args[1], "help") == 0){
		int n = 0;
		if (argC == 2) print_help(NULL);
		for (n = 1; n < argC; n++){
			print_help(args[n]);
		}
		exit(0);
	}
	srand(time(0));
	if (argC == 1){
			char buff[512] = {'\0'};
			printf("please input the soldier count, or type \"HELP\" to acess the help interface\n>> ");
			int soldiers = 0;
			while (1){
				scanf("%512s", buff);
				soldiers = strtoimax(buff, NULL, 10);
				if (soldiers < 1){
					if (strcmp(buff, "HELP") == 0){
						printf("entering help mode, type \"QUIT\" to exit, and \"HELP\" for a guide\n>> ");
						while (1){
							scanf("%512s", buff);
							if (strcmp("quit", buff) == 0){
								printf("now exiting help mode...\n>> ");
								break;
							}
							print_help(buff);
							printf(">> ");
						}
					} else {
						printf("invalid soldier count, try again\n>> ");
					}
			} else {
				break;
			}
		}
		glob_init(soldiers);
		fprintf(stdout, "starting match with %d soldiers \n", soldiers);
		char cwd[1024];
		getcwd(cwd, 1024);
		Soldier* blue;
		Soldier* red;
		String* dir = ptrToStr(cwd);
		appendPtr(dir, "/", 1);
		size_t len = dir->length;	
		printf("current directory is \"%s\"\nplease input relative path to the red team's script\n>> ", dir->string);
		while (1){
			scanf("%512s", buff); 
			appendNoLen(dir, buff, 512);
			printf("got \"%s\" as a path\n", dir->string);
			FILE* code = fopen(dir->string, "rb+");
			if (code != NULL){
				red = translate(code);
				fclose(code);
				break;
			} else {
				printf("FAILED TO READ RED TEAM'S SCRIPT, TRY AGAIN\n>> ");
			}
			dir->length = len;
			dir->string[len] = '\0';
		}
		dir->length = len;
		dir->string[len] = '\0';

		printf("current directory is \"%s\"\nplease input relative path to the blue team's script\n>> ", dir->string);
		while (1){
			scanf("%512s", buff); 
			appendNoLen(dir, buff, 512);
			printf("got \"%s\" as a path\n", dir->string);
			FILE* code = fopen(dir->string, "rb+");
			if (code != NULL){
				blue = translate(code);
				fclose(code);
				break;
			} else {
				printf("FAILED TO READ BLUE TEAM'S SCRIPT, TRY AGAIN\n>> ");
			}
			dir->length = len;
			dir->string[len] = '\0';
		}		
		discardStr(dir);
		sleep(1);
		init_game(red, blue, soldiers);
		game_loop();
	} else if (argC != 4){
		printf("invalid argument count");
		return 0;
	}
	int soldiers = strtoimax(args[1], NULL, 10);
	if (soldiers < 1){
		printf("invalid soldier count\n");
		return 0;
	}
	glob_init(soldiers);
	fprintf(stdout, "started match with %d soldiers \n", soldiers);
	char cwd[1024];
	getcwd(cwd, 1024);
	Soldier* blue;
	Soldier* red;
	String* dir = ptrToStr(cwd);
	appendPtr(dir, "/", 1);
	size_t len = dir->length;
	appendNoLen(dir, args[2], 128);
	fprintf(stdout, "red team code file = \"%s\"\n", dir->string);
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
	appendNoLen(dir, args[3], 128);
	dir->string[dir->length] = '\0';
	fprintf(stdout, "blue team code file = \"%s\"\n", dir->string);
	code = fopen(dir->string, "rb+");
	if (code != NULL) {
		blue = translate(code);
	} else {
		printf("FAILED TO READ BLUE TEAM'S SCRIPT\n");
		return 0;
	}
	fclose(code);
	discardStr(dir);
	sleep(1);
	init_game(red, blue, soldiers);
	game_loop();
	return 0;
}
