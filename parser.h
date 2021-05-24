#include "lex.h"
#include "vm.h"

// structure for a symbol
typedef struct {
    int kind;
    char name[11];
    int val;
    int level;
    int addr;
    int mark;
    int param;
} symbol;

// Function declarations
void getToken();
void addInstruction(int opcode, int l, int m, char* op);
void enter(int kind, char name[11], int val, int level, int addr, int mark, int param);
int symboltype(int index);
int symbol_table_check(char* name, int lexlevel);
int symbol_table_search(char* name, int lexlevel, int kind);
void mark(int count);
void program();
void block(int lexlevel, int param, int procIndex);
int const_declaration(int lexlevel);
int var_declaration(int lexlevel, int param);
int procedure_declaration(int lexlevel);
void statement(int lexlevel);
void condition(int lexlevel);
void expression(int lexlevel);
void term(int lexlevel);
void factor(int lexlevel);
void parse(char* arg, int printLexeme, int printAssembly, int printVM);
