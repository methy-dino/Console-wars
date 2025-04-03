#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "hashMap.h"
#include "gameRules.h"
#include "conversor.h"
#include <inttypes.h>
/*typedef struct arg_std_a {
	char arg_mode;
	int arg;
} ONE_ARG;
typedef struct arg_std_b {
	char arg1_mode;
	char arg2_mode;
	int arg1;
	int arg2;
} TWO_ARGS;
typedef struct cmp_arg_std {
	unsigned char comparison;
	char arg1_mode;
	char arg2_mode;
	int arg1;
	int arg2;
} CMP_ARGS;
typedef struct instruc {
	void* args;
	char instruction_id;
} Instruction;
#define TMP_RET 0
#define TMP_MATH 1
#define SOL_X 2
#define SOL_Y 3
#define SOL_STAT 4
#define DATA_PTR 'p'
#define RAW_DATA 'r'
#define EQUAL 1
//#define DIFFERENT 2
#define BIGGER 4
#define SMALLER 8
typedef struct soldier {
	int vars[32];
	Instruction* instructions;
	int instruction_total;
	int curr;
} Soldier;*/
void MEM_CP(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int receive = convert->arg1;
	int send = convert->arg2;
	soldier->vars[receive] = convert->arg2_mode == DATA_PTR ? soldier->vars[send] : send;
}
void CMP(Soldier* soldier, void* args){
	CMP_ARGS* convert = (CMP_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	int ret = 0;
	//	printf("comparison: %d %d\n", numa, numb);
	//printf("results: %d %d %d\n",convert->comparison & EQUAL, convert->comparison & SMALLER, convert->comparison & BIGGER);	
	if ((convert->comparison & EQUAL) > 0){
		//printf("equality check\n");
		ret = numa==numb;
	}
	if (ret == 0 && (convert->comparison & SMALLER) > 0){
		//printf("lesser check\n");
		ret = numa<numb;
	}
	if (ret == 0 && (convert->comparison & BIGGER) > 0){
		//fprintf(stderr, "greater check\n");
		ret = numa>numb;
	}
	//printf("comparison result: %d\n", ret);
	soldier->vars[TMP_RET] = ret;
}
void JMP(Soldier* soldier, void* args){
	ONE_ARG* convert = (ONE_ARG*) args;
	soldier->curr = convert->arg;
}
void CON_JMP(Soldier* soldier, void* args){
	ONE_ARG* convert = (ONE_ARG*) args;
	soldier->curr = soldier->vars[TMP_RET] == 1 ? convert->arg : soldier->curr;
}
void SOL_ATK(Soldier* soldier, void* args){
	ONE_ARG* convert = (ONE_ARG*) args;
	int move_dir = convert->arg_mode == DATA_PTR ? soldier->vars[convert->arg] : convert->arg;
	//fprintf(stderr, "movedir is: %d\n", move_dir);
	move_dir = move_dir % 4;
	//fprintf(stderr, "movedir is: %d\n", move_dir);
	char moveY = (move_dir == 0) - (move_dir == 2);
	char moveX = (move_dir == 1) - (move_dir == 3);
	attack_try(soldier, moveX, moveY);
}
void CHECK(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int x = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int y = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	int result = game_check_at(x, y);
	// same team returns 0, empty returns -1, diff team returns 1. 
	soldier->vars[TMP_RET] = ((soldier->vars[SOL_ID] > 0 && result > 0) || (soldier->vars[SOL_ID] < 0 && result > INT_MIN && result < 0)) - (result == INT_MIN || result == 0);
}
void SOL_MOVE(Soldier* soldier, void* args){
	ONE_ARG* convert = (ONE_ARG*) args;
		int move_dir = convert->arg_mode == DATA_PTR ? soldier->vars[convert->arg] : convert->arg;
	//fprintf(stderr, "movedir is: %d\n", move_dir);
	move_dir = move_dir % 4;
	//fprintf(stderr, "movedir is: %d\n", move_dir);
	char moveY = (move_dir == 0) - (move_dir == 2);
	char moveX = (move_dir == 1) - (move_dir == 3);
	//fprintf(stderr, "move try by %d X, %d Y\n", moveX, moveY);
	soldier->vars[TMP_RET] = move_try(soldier, moveX, moveY); 
}
void RAND(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int min = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int max = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_RET] = min + (rand() % max-min);
	//printf("min %d, max %d generated %d \n", min, max, soldier->vars[TMP_RET]);
}
void ADD(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_MATH] = numa+numb;
}
void SUB(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_MATH] = numa-numb;
}
void MUL(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_MATH] = numa*numb;
}
void DIV(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_MATH] = numa/numb;
}
void MOD(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int numa = convert->arg1_mode == DATA_PTR ? soldier->vars[convert->arg1] : convert->arg1;
	int numb = convert->arg2_mode == DATA_PTR ? soldier->vars[convert->arg2] : convert->arg2;
	soldier->vars[TMP_MATH] = numa%numb;
}
void SEEK(Soldier* soldier, void* args){
	TWO_ARGS* convert = (TWO_ARGS*) args;
	int x = convert->arg1;
	int y = convert->arg2;
	seek_try(soldier,x,y);
}
#define MEM_CP_IND 0
#define CMP_IND 1
#define JMP_IND 2
#define CON_JMP_IND 3
#define CHECK_IND 4
#define SOL_MOVE_IND 5
#define SOL_ATK_IND 6
#define RAND_IND 7
#define ADD_IND 8
#define SUB_IND 9
#define MUL_IND 10
#define DIV_IND 11
#define MOD_IND 12
#define SEEK_IND 13
#define DECLARATION 200
#define NO_KEYWORD 201
void (*instructions[])(Soldier* soldier, void* args) = {&MEM_CP, &CMP, &JMP, &CON_JMP, &CHECK, &SOL_MOVE, &SOL_ATK, &RAND, &ADD, &SUB, &MUL, &DIV, &MOD, &SEEK};
int strcmp_wrap(void* a, void* b){
	if (a == NULL || b == NULL){
		return 1;
	}
	return strcmp((void*)a,(void*)b);
}
unsigned char check_keywords(char* buff){
	int i = 0;
	//while (buff[i] == ' ' || buff[i] == '	'){
	//	i++;
	//}
	if (strcmp("MOVE", buff + i) == 0){
		return SOL_MOVE_IND;
	} else if (strcmp("SEEK", buff + i) == 0){
		return SEEK_IND;
	} else if (strcmp("GOTO", buff + i) == 0){
		return JMP_IND;
	} else if (strcmp("ATTACK", buff + i) == 0){
		return SOL_ATK_IND;
	} else if (strcmp("CON_GOTO", buff + i) == 0){
		return CON_JMP_IND;
	} else if (strcmp("LET", buff + i) == 0){
		return DECLARATION;
	} else if (strcmp("RAND", buff + i) == 0){
		return RAND_IND;
	}
	return NO_KEYWORD;
}
char is_math(char* str){
	return (str[0] == '*' || str[0] == '/' || str[0] == '+' || str[0] =='-' || str[0] == '%');
}
char is_comparator(char* str){
	return str[0] == '=' && (str[1] == '=' || str[1] == '>' || str[1] == '<');	
}
char** tokenize(char* raw, unsigned char* token_count){
	unsigned int prev = 0;
	unsigned int curr = 0;
	char** tokens = malloc(sizeof(char*) * 20);
	unsigned char max_token = 20;
	token_count[0] = 0;
	while (raw[curr] != '\0'){
		if (raw[curr] == '=' || raw[curr] == '<' || raw[curr] == '>') {
			if (raw[curr+1] == '=' || raw[curr+1] == '<' || raw[curr+1] == '>'){
				tokens[*token_count] = malloc(sizeof(char)*3);
				tokens[*token_count][0] = raw[curr];
				tokens[*token_count][1] = raw[curr+1];
				tokens[*token_count][2] = '\0';
				//printf("token made\n");
				token_count[0]++;
				curr++;
			} else {
			tokens[*token_count] = malloc(sizeof(char)*2);
			tokens[*token_count][0] = raw[curr];
			tokens[*token_count][1] = '\0';
			token_count[0]++;
			//printf("token made\n");
			}
			curr++;
			//printf("aa");
		} else if (raw[curr] == '>' || raw[curr] == '<'){
			tokens[*token_count] = malloc(sizeof(char)*2);
			tokens[*token_count][0] = raw[curr];
			tokens[*token_count][1] = '\0';
			token_count[0]++;
		} else if(is_math(raw+curr)){
		tokens[*token_count] = malloc(sizeof(char)*2);
		tokens[*token_count][0] = raw[curr];
		tokens[*token_count][1] = '\0';
		token_count[0]++;
//printf("token made\n");
		curr++;
		prev = curr;
		} else if (raw[curr] > '0'-1 && raw[curr] < '9'+1){
			prev = curr;
			while (raw[curr] > '0'-1 && raw[curr] < '9'+1){
				curr++;
			}
			tokens[*token_count] = malloc(sizeof(char)*(curr-prev+1));
			memcpy(tokens[*token_count], raw + prev, curr-prev);
			tokens[*token_count][curr-prev] = '\0'; 
			token_count[0]++;
//printf("token made\n");
		} else if (raw[curr] != '\n' && raw[curr] != ' ' && raw[curr] != '	'){
			prev = curr;
			while (raw[curr] != '\n' && raw[curr] != '\0' && raw[curr] != ' ' && raw[curr] != '	' && is_math(raw+curr) == 0){
				curr++;
			}
			tokens[token_count[0]] = malloc(sizeof(char)*(curr-prev+1));
			memcpy(tokens[token_count[0]], raw + prev, curr-prev);
			/*for (int i = prev; i < curr; i++){
				tokens[token_count[0]][i-prev] = raw[i];
				printf("%d ", i-prev);
			}*/
			tokens[*token_count][curr-prev] = '\0';
			token_count[0]++;
//printf("token made\n");
		} else {
			curr++;
		}
	}
	return tokens;
}
void build_con_jmp(HashMap* var_mp,Soldier* emul, char** tokens){
	int* var;
	Instruction cmp = (Instruction) {NULL, CMP_IND}; 
	Instruction jmp = (Instruction) {NULL, CON_JMP_IND};
	if (tokens[1][0] < '0' || tokens[1][0] > '9'){
		printf("CON_GOTO malformed, argument 1 is NOT A NUMBER!\n");
		exit(0);
	}
	int curr_i = 1;
	jmp.args = malloc(sizeof(ONE_ARG));
	if (tokens[curr_i][0] == '-'){
		fprintf(stderr, "CON GOTO WITH NEGATIVE INDEX\n");
		exit(0);
	}
	((ONE_ARG*)jmp.args)->arg_mode = RAW_DATA;
	((ONE_ARG*)jmp.args)->arg = strtoimax(tokens[curr_i], NULL, 10);
	curr_i++;
	cmp.args = malloc(sizeof(CMP_ARGS));
	if ((var = (int*) getValue(var_mp, tokens[curr_i])) != NULL){
		((CMP_ARGS*)cmp.args)->arg1 = *var;
		((CMP_ARGS*)cmp.args)->arg1_mode = DATA_PTR;
		curr_i++;
	} else if (!(tokens[curr_i][0] < '0' || tokens[curr_i][0] > '9')){
		((CMP_ARGS*)cmp.args)->arg1 = strtoimax(tokens[curr_i], NULL, 10);
		((CMP_ARGS*)cmp.args)->arg1_mode = RAW_DATA;
		curr_i++;
	} else if ((tokens[curr_i+1][0] > '0' - 1 && tokens[curr_i+1][0] < '9' + 1) && tokens[curr_i][0] == '-') {
		((CMP_ARGS*)cmp.args)->arg1 = -strtoimax(tokens[curr_i+1], NULL, 10);
		((CMP_ARGS*)cmp.args)->arg1_mode = RAW_DATA; 
		curr_i += 2;
	} else {
		printf("CON_GOTO malformed, invalid comparison\n");
		exit(0);
	}
//	printf("cmp arg 1 done\n");
	unsigned char mode = '\0';
	if (tokens[curr_i][0] == '='){
		mode = mode | EQUAL;
	} else if (tokens[curr_i][0] == '>'){
		mode = mode | BIGGER;
	} else if (tokens[curr_i][0] == '<'){
		mode = mode | SMALLER;
	}
	//printf("mode %d \n", mode);
	if (tokens[curr_i][1] != '\0'){
		if (tokens[curr_i][1] == '='){
			mode = mode | EQUAL;
		}	else if (tokens[curr_i][1] == '>'){
			mode = mode | BIGGER;
		} else if (tokens[curr_i][1] == '<'){
			mode = mode | SMALLER;
		}
	}
	fprintf(stderr, "mode: %d\n", mode);
	curr_i++;
//printf("mode %d \n", mode);
	//printf("cmp comparison done.\n");
	((CMP_ARGS*)cmp.args)->comparison = mode;
	if (!(tokens[curr_i][0] < '0' || tokens[curr_i][0] > '9')){
		((CMP_ARGS*)cmp.args)->arg2 = strtoimax(tokens[curr_i], NULL, 10);
		((CMP_ARGS*)cmp.args)->arg2_mode = RAW_DATA;
		curr_i++;
	} else if ((var = (int*) getValue(var_mp, tokens[curr_i])) != NULL){
		((CMP_ARGS*)cmp.args)->arg2 = *var;
		((CMP_ARGS*)cmp.args)->arg2_mode = DATA_PTR;
		curr_i++;
	} else if ((tokens[curr_i+1][0] > '0' - 1 && tokens[curr_i+1][0] < '9' + 1) && tokens[curr_i][0] == '-') {
		((CMP_ARGS*)cmp.args)->arg2 = -strtoimax(tokens[curr_i+1], NULL, 10);
		((CMP_ARGS*)cmp.args)->arg2_mode = RAW_DATA;
		curr_i+=2;
	} else {
		printf("CON_GOTO malformed, invalid comparison\n");
		exit(0);
	}
	emul->instructions[emul->instruction_total] = cmp;
	emul->instruction_total++;
	emul->instructions[emul->instruction_total] = jmp;
	emul->instruction_total++;
}
void build_jmp(Soldier* emul, char** tokens){
	Instruction jmp = (Instruction) {NULL, JMP_IND};
	if (tokens[1][0] < '0' || tokens[1][0] > '9'){
		printf("GOTO malformed, argument 1 is NOT A NUMBER!\n");
		exit(0);
	}	
	jmp.args = malloc(sizeof(ONE_ARG));
	((ONE_ARG*)jmp.args)->arg_mode = RAW_DATA;
	((ONE_ARG*)jmp.args)->arg = strtoimax(tokens[1], NULL, 10);
	emul->instructions[emul->instruction_total] = jmp;
	emul->instruction_total++;
}
void build_move(Soldier* emul, char** tokens, HashMap* vars){
	Instruction move = (Instruction) {NULL, SOL_MOVE_IND};
	move.args = malloc(sizeof(ONE_ARG));
	if (tokens[1][0] < '0' || tokens[1][0] > '9'){
		((ONE_ARG*)move.args)->arg_mode = DATA_PTR;
		((ONE_ARG*)move.args)->arg = *(int*)getValue(vars, tokens[1]);
	} else {
		((ONE_ARG*)move.args)->arg_mode = RAW_DATA;
		((ONE_ARG*)move.args)->arg = strtoimax(tokens[1], NULL, 10);
		//fprintf(stderr, "got %d as move arg", ((ONE_ARG*)move.args)->arg);
	}
	emul->instructions[emul->instruction_total] = move;
	emul->instruction_total++;
}
void build_atk(Soldier* emul, char** tokens, HashMap* vars){
	Instruction atk = (Instruction) {NULL, SOL_ATK_IND};
	atk.args = malloc(sizeof(ONE_ARG));
	if (tokens[1][0] < '0' || tokens[1][0] > '9'){
		((ONE_ARG*)atk.args)->arg_mode = DATA_PTR;
		((ONE_ARG*)atk.args)->arg = *(int*)getValue(vars, tokens[1]);
	} else {
		((ONE_ARG*)atk.args)->arg_mode = RAW_DATA;
		((ONE_ARG*)atk.args)->arg = strtoimax(tokens[1], NULL, 10);
		//fprintf(stderr, "got %d as atk arg", ((ONE_ARG*)move.args)->arg);
	}
	emul->instructions[emul->instruction_total] = atk;
	emul->instruction_total++;
}
void build_seek(Soldier* emul, char** tokens, HashMap* vars){
	Instruction seek = (Instruction) {NULL, SEEK_IND};
	seek.args = malloc(sizeof(TWO_ARGS));
	((TWO_ARGS*)seek.args)->arg1_mode = DATA_PTR;
	((TWO_ARGS*)seek.args)->arg2_mode = DATA_PTR;
	((TWO_ARGS*)seek.args)->arg1 = *(int*)getValue(vars, tokens[1]);
	((TWO_ARGS*)seek.args)->arg2 = *(int*)getValue(vars, tokens[2]);
}
unsigned char priority(char* math_sym){
	if (math_sym[0] == '+' || math_sym[0] == '-'){
		return 1;
	}
	if (math_sym[0] == '*' || math_sym[0] == '/' || math_sym[0] == '%'){
		return 2;
	}
	return 0;
}
void rpn_math(HashMap* var_map, Soldier* emul, char** tokens, unsigned char token_length){
	char** num_stack = malloc(sizeof(char*) * token_length);
	unsigned char num_i = 0;
	char* prev_sym = NULL;
	for (int i = 0; i < token_length; i++){
		if (is_math(tokens[i])){
			if (prev_sym != NULL){
				num_stack[num_i] = prev_sym;
				num_i++;
			}
			prev_sym = tokens[i];
		} else if (check_keywords(tokens[i]) == NO_KEYWORD){
			num_stack[num_i] = tokens[i];
			num_i++;
		} else {
			printf("KEYWORD OR FUNCTION INSIDE MATH OPERATION!\n");
			exit(0);
		}
	}
	num_stack[num_i] = prev_sym;
	num_i++;
	if (num_i < 3){
		printf("MALFORMED MATH, LACK OF SYMBOLS\n");
		exit(0);
	}
	TWO_ARGS* args;
	int i = 0;
	while (i < num_i){
	 while (is_math(num_stack[i]) == 0) i++;
	 	if (num_stack[i][0] == '*') {
			emul->instructions[emul->instruction_total] = (Instruction) {NULL, MUL_IND};
		 } else if (num_stack[i][0] == '/') {
			emul->instructions[emul->instruction_total] = (Instruction) {NULL, DIV_IND};
	 	} else if (num_stack[i][0] == '+') {
			emul->instructions[emul->instruction_total] = (Instruction) {NULL, ADD_IND};
	 	} else if (num_stack[i][0] == '-') {
			emul->instructions[emul->instruction_total] = (Instruction) {NULL, SUB_IND};
	 	} else if (num_stack[i][0] == '%') {
			emul->instructions[emul->instruction_total] = (Instruction) {NULL, MOD_IND};
	 	}
		args = malloc(sizeof(TWO_ARGS));
		//printf("equaltion!\n");
		if (i < 3){
			if (num_stack[i-2][0] > '0'-1 && num_stack[i-2][0] < '9'+1){
				args->arg1_mode = RAW_DATA;
				args->arg1 = strtoimax(num_stack[i-2], NULL, 10);
			} else {
				args->arg1_mode = DATA_PTR;
				args->arg1 = *(int*)getValue(var_map, num_stack[i-2]);
			}
		} else {
			args->arg1_mode = DATA_PTR;
			args->arg1 = TMP_MATH;
		}
		if (num_stack[i-1][0] > '0'-1 && num_stack[i-1][0] < '9'+1){
			args->arg2_mode = RAW_DATA;
			args->arg2 = strtoimax(num_stack[i-1], NULL, 10);
		} else {
			args->arg2_mode = DATA_PTR;
			args->arg2 = *(int*)getValue(var_map, num_stack[i-1]);
		}
		emul->instructions[emul->instruction_total].args = args;
		emul->instruction_total++;
		i++;
	}
}
void build_rand(HashMap* var_map, Soldier* emul, char** tokens){
	Instruction rand = (Instruction) {NULL, RAND_IND};
	rand.args = malloc(sizeof(TWO_ARGS));
	int* var = NULL;
	//printf("aaa\n");
	if (!(tokens[1][0] < '0' || tokens[1][0] > '9')){
		((TWO_ARGS*)rand.args)->arg1 = strtoimax(tokens[1], NULL, 10);
		((TWO_ARGS*)rand.args)->arg1_mode = RAW_DATA;
	} else if ((var = (int*)getValue(var_map, tokens[1])) != NULL){
		//printf("AAAA\n");
		((TWO_ARGS*)rand.args)->arg1 = *var;
		((TWO_ARGS*)rand.args)->arg1_mode = DATA_PTR;
	} else {
		printf("RAND malformed, invalid arguments\n");
		exit(0);
	}
	//printf("bbb\n");
	if (!(tokens[2][0] < '0' || tokens[2][0] > '9')){
		((TWO_ARGS*)rand.args)->arg2 = strtoimax(tokens[2], NULL, 10);
		((TWO_ARGS*)rand.args)->arg2_mode = RAW_DATA;
	} else if ((var = (int*)getValue(var_map, tokens[2])) != NULL){
		//printf("AAAA\n");
		((TWO_ARGS*)rand.args)->arg2 = *var;
		((TWO_ARGS*)rand.args)->arg2_mode = DATA_PTR;
	} else {
		printf("RAND malformed, invalid arguments\n");
		exit(0);
	}
	//printf("ccc\n");
	emul->instructions[emul->instruction_total] = rand;
	emul->instruction_total++;
}
Soldier* translate(FILE* read){
	Soldier* emul = malloc(sizeof(Soldier));
	emul->instructions = malloc(sizeof(Instruction) * 256);
	emul->instruction_total = 0;
	emul->curr = 0;
	char buff[1024];
	int i = 0;
	int line_relation[256];
	int curr_line = 0;
	int var_ind = 6;
	//Instruction curr_inst = ({0};
	int* curr_var = NULL;
	unsigned char keyword_code = NO_KEYWORD;
	HashMap* var_map = createMap(16, &strHash, &strcmp_wrap, &defaultFree);
	//ADDING MAGIC MEMORY SPACES.
	char* var_name = (char*) malloc(8);
	memcpy(var_name, "TMP_RET", 8);
	int* var_val = malloc(sizeof(int));
	var_val[0] = 0;
	addPair(var_map, var_name, var_val);
	var_name = (char*) malloc(9);
	memcpy(var_name, "TMP_MATH", 9);
	var_val = (int*) malloc(sizeof(int));
	var_val[0] = 1;
	addPair(var_map, var_name, var_val);
	var_name = (char*) malloc(6);
	memcpy(var_name, "SOL_X", 6);
	var_val = (int*) malloc(sizeof(int));
	var_val[0] = 2;
	addPair(var_map, var_name, var_val);
	var_name = (char*) malloc(6);
	memcpy(var_name, "SOL_Y", 6);
	var_val = (int*) malloc(sizeof(int));
	var_val[0] = 3;
	addPair(var_map, var_name, var_val);
	var_name = (char*) malloc(9);
	memcpy(var_name, "SOL_STAT", 9);
	var_val = (int*) malloc(sizeof(int));
	var_val[0] = 4;
	addPair(var_map, var_name, var_val);
	var_name = (char*) malloc(7);
	memcpy(var_name, "SOL_ID", 7);
	var_val = (int*) malloc(sizeof(int));
	var_val[0] = 5;
	addPair(var_map, var_name, var_val);
	while (fgets(buff, 1024, read) != NULL){
		line_relation[curr_line] = emul->instruction_total;
		curr_line++;
		// skip white space.
		unsigned char tok_ct = 0;
		char** tokens = tokenize(buff, &tok_ct);
		printf("tokenized to %d tokens: \n[", tok_ct);
		for (int j = 0; j < tok_ct; j++){
			printf("%s, ", tokens[j]);
		}
		printf("]\n");
		int assign_ind = -1;
		if ((keyword_code = check_keywords(tokens[0])) != NO_KEYWORD){
			//printf("keyword code = %d\n", keyword_code); 
			if (keyword_code == CON_JMP_IND){
				build_con_jmp(var_map, emul, tokens);
			} else if (keyword_code == JMP_IND){
				build_jmp(emul, tokens);
			} else if (keyword_code == SOL_ATK_IND){
				if (tok_ct != 2){
					fprintf(stderr, "IMPROPER ATTACK CALL SYNTAX\n");
					exit(0);
				}
				build_atk(emul, tokens, var_map);
			} else if (keyword_code == SOL_MOVE_IND){
				if (tok_ct != 2){
					fprintf(stderr, "IMPROPER MOVE CALL SYNTAX\n");
					exit(0);
				}
				//DONE: move builder.
				build_move(emul, tokens, var_map);
			} else if (keyword_code == DECLARATION){
					//printf("var declared\n");	
						//printf("%d \n", strlen(); 
					char* var_name = (char*) malloc(strlen(tokens[1]) + 1);
//printf("test\n");
					memcpy(var_name, tokens[1], strlen(tokens[1])+1);
					int* var_val = malloc(sizeof(int));
					var_val[0] = var_ind;
					addPair(var_map, var_name, var_val);
					var_ind++;
//printf("test\n");
				if (tok_ct > 2){
					if (strcmp(tokens[2], "=")){
						fprintf(stderr, "INVALID SYMBOL OR KEYWORD INSTEAD OF EQUAL SYMBOL AT LINE %d \n", curr_line);
						exit(0);
					}
					if (tok_ct > 3) {
					} else {
						TWO_ARGS* args = malloc(sizeof(TWO_ARGS));
						args->arg1_mode = DATA_PTR;
						args->arg1 = *var_val;
						if (tokens[2][0] > '0'-1 && tokens[2][1] < '9'+1){
							args->arg2_mode = RAW_DATA;
							args->arg1 = strtoimax(tokens[2], NULL, 10);
						} else {
							args->arg1_mode = DATA_PTR;
							args->arg1 = *(int*)getValue(var_map, tokens[2]);
						}
						emul->instructions[emul->instruction_total].instruction_id = MEM_CP_IND;
						emul->instructions[emul->instruction_total].args = args;
						emul->instruction_total++;
					}
				}
			}
		} else if (keyword_code == SEEK_IND){
			build_seek(emul, tokens, var_map);
		} else if ((curr_var = getValue(var_map, tokens[i])) != NULL){
			printf("variable match\n");
			if (strcmp(tokens[1], "=")){
						printf("INVALID SYMBOL OR KEYWORD INSTEAD OF EQUAL SYMBOL AT LINE %d \n", curr_line);
						exit(0);
					}
			if (curr_var[0] < 5){
			fprintf(stderr, "ATTEMPTED TO MODIFY READ ONLY VARIABLE!\n");
			}
			if (tok_ct > 3) {
				if ((keyword_code = check_keywords(tokens[2])) != NO_KEYWORD){
					TWO_ARGS* args;
					switch (keyword_code){
						case RAND_IND:
							build_rand(var_map, emul, &tokens[2]);
							emul->instructions[emul->instruction_total] = (Instruction) {NULL, MEM_CP_IND};
							args = malloc(sizeof(TWO_ARGS));
							args->arg1_mode = DATA_PTR;
							args->arg1 = curr_var[0];
							args->arg2_mode = DATA_PTR;
							args->arg2 = TMP_RET;
							emul->instructions[emul->instruction_total].args = args;
							emul->instruction_total++;
							break;
						case CHECK_IND:
							//TODO: IMPLEMENT CHECK
							//build_check(var_map, emul, &tokens[2]);
							emul->instructions[emul->instruction_total] = (Instruction) {NULL, MEM_CP_IND};
							args= malloc(sizeof(TWO_ARGS));
							args->arg1_mode = DATA_PTR;
							args->arg1 = curr_var[0];
							args->arg2_mode = DATA_PTR;
							args->arg2 = TMP_RET;
							emul->instructions[emul->instruction_total].args = args;
							emul->instruction_total++;
							break;
						case SOL_MOVE_IND:
							build_move(emul, &tokens[2], var_map);
							emul->instructions[emul->instruction_total] = (Instruction) {NULL, MEM_CP_IND};
							args= malloc(sizeof(TWO_ARGS));
							args->arg1_mode = DATA_PTR;
							args->arg1 = curr_var[0];
							args->arg2_mode = DATA_PTR;
							args->arg2 = TMP_RET;
							emul->instructions[emul->instruction_total].args = args;
							emul->instruction_total++;
							break;
						default:
							printf("INVALID MEMORY ASSERT!");
							break;
					}
				} else {
				rpn_math(var_map, emul, &tokens[2], tok_ct - 2);
				emul->instructions[emul->instruction_total] = (Instruction) {NULL, MEM_CP_IND};
				TWO_ARGS* args= malloc(sizeof(TWO_ARGS));
				args->arg1_mode = DATA_PTR;
				args->arg1 = curr_var[0];
				args->arg2_mode = DATA_PTR;
				args->arg2 = TMP_MATH;	
				emul->instructions[emul->instruction_total].args = args;
				emul->instruction_total++;
			}
		} else {
				TWO_ARGS* args = malloc(sizeof(TWO_ARGS));
				args->arg1_mode = DATA_PTR;
				args->arg1 = *(int*)getValue(var_map, tokens[0]);
				if (tokens[2][0] > '0'-1 && tokens[2][1] < '9'+1){
					args->arg2_mode = RAW_DATA;
					args->arg2 = strtoimax(tokens[2], NULL, 10);
				} else {
					args->arg2_mode = DATA_PTR;
					args->arg2 = *(int*)getValue(var_map, tokens[2]);
				}
				emul->instructions[emul->instruction_total].instruction_id = MEM_CP_IND;
				emul->instructions[emul->instruction_total].args = args;
				emul->instruction_total++;
			}
		} else {
			printf("COMPILE ERROR, LINE %d PRODUCED NO VALID STARTING TOKENS\n", curr_line);
			exit(0);
		}
		for (int j = 0; j < tok_ct; j++){
			free(tokens[j]);
		}

		free(tokens);
	}
	fprintf(stderr, "compiled to %d instructions: \n", emul->instruction_total);
	for (int i = 0; i < emul->instruction_total; i++){
		if (emul->instructions[i].instruction_id == JMP_IND || emul->instructions[i].instruction_id == CON_JMP_IND){
			((ONE_ARG*)emul->instructions[i].args)->arg = line_relation[((ONE_ARG*)emul->instructions[i].args)->arg]-1;
		}
		fprintf(stderr, "%dth - ID: %d \n", i, emul->instructions[i].instruction_id);
	}
	for (int i = 0; i < 32; i++){
		emul->vars[i] = 0;
	}
	emul->curr = -1;
	discardMap(var_map);
	//fclose(read);
	return emul;
}
void RUN(Soldier* soldier){
	//is dead check.
	if (soldier->curr < -1){
		return;
	}
	soldier->curr++;
	//printf("running instruction number %d\n", soldier->curr);
	instructions[soldier->instructions[soldier->curr].instruction_id](soldier, soldier->instructions[soldier->curr].args);	
	//printf("variable state: \n[");
	//for (int i = 0; i < 32; i++){
	//printf("%d, ", soldier->vars[i]);
	//}
	//printf("]\n");
	//printf("%d %d comparisons\n", EQUAL | BIGGER, EQUAL | SMALLER);
}
