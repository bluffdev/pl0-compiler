#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 500

// Register declarations
int SP, BP, PC;
int stack[MAX_STACK_HEIGHT];

// structure for a insturction
typedef struct instruction {
    int opcode;
    char op[4];
    int l;
    int m;
} instruction;

instruction* IR;
instruction code[MAX_CODE_LENGTH];
int instruction_length;

int base(int* stack, int level, int BP);
void runVM(int printVM);
