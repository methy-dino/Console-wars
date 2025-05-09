/*header automatically generated by autoHead*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "hashMap.h"
#include <inttypes.h>
#include <stdint.h>
typedef struct arg_std_a {
	char arg_mode;
	int32_t arg;
} ONE_ARG;
typedef struct arg_std_b {
	char arg1_mode;
	char arg2_mode;
	int32_t arg1;
	int32_t arg2;
} TWO_ARGS;
typedef struct cmp_arg_std {
	unsigned char comparison;
	char arg1_mode;
	char arg2_mode;
	int32_t arg1;
	int32_t arg2;
} CMP_ARGS;
typedef struct instruc {
	void* args;
	unsigned char instruction_id;
} Instruction;
#define TMP_RET 0
#define TMP_MATH 1
#define SOL_X 2
#define SOL_Y 3
#define SOL_STAT 4
#define SOL_ID 5
#define DATA_PTR 'p'
#define RAW_DATA 'r'

#define EQUAL 1
#define BIGGER 4
#define SMALLER 8
typedef struct soldier {
	int32_t vars[32];
	Instruction* instructions;
	int32_t instruction_total;
	int32_t curr;
} Soldier;
void MEM_CP(Soldier* soldier, void* args);
void CMP(Soldier* soldier, void* args);
void JMP(Soldier* soldier, void* args);
void CON_JMP(Soldier* soldier, void* args);
void SOL_ATK(Soldier* soldier, void* args);
void CHECK(Soldier* soldier, void* args);
void SOL_MOVE(Soldier* soldier, void* args);
void RAND(Soldier* soldier, void* args);
void ADD(Soldier* soldier, void* args);
void SUB(Soldier* soldier, void* args);
void MUL(Soldier* soldier, void* args);
void DIV(Soldier* soldier, void* args);
void MOD(Soldier* soldier, void* args);
/*#define MEM_CP_IND 0
#define CMP_IND 1
#define JMP_IND 2
#define CON_JMP_IND 3
#define CHECK_IND 4
#define SOL_MOVE_IND 5
#define RAND_IND 6
#define ADD_IND 7
#define SUB_IND 8
#define MUL_IND 9
#define DIV_IND 10
#define MOD_IND 11
#define DECLARATION 200
#define NO_KEYWORD 201
#define SOL_ATTACK_IND 224*/
int32_t strcmp_wrap(void* a, void* b);
unsigned char check_keywords(char* buff);
char is_math(char* str);
char is_comparator(char* str);
char** tokenize(char* raw, unsigned char* token_count);
void build_con_jmp(HashMap* var_mp,Soldier* emul, char** tokens);
void build_jmp(Soldier* emul, char** tokens);
unsigned char priority(char* math_sym);
void rpn_math(HashMap* var_map, Soldier* emul, char** tokens, unsigned char token_length, int32_t* max_inst);
void build_rand(HashMap* var_map, Soldier* emul, char** tokens);
void glob_init(int32_t sol_ct);
Soldier* translate(FILE* read);
void RUN(Soldier* soldier);
