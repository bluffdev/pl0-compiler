#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define norw 15   // number of reserved words
#define nosym 16  // number of symbols

// enumeration for the token types
typedef enum {
    modsym = 1, identsym, numbersym, plussym, minussym, multsym,
    slashsym, oddsym, eqsym, neqsym, lessym, leqsym, gtrsym,
    geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym,
    writesym, readsym, elsesym, returnsym
} token_type;

// strucutre for a token
typedef struct {
    char value[11];
    token_type token_type;
} token;

// Function declarations
void append_string(char* buffer, int* i, char ch);
int compare_words(char* buffer, char* word[], int size);
int compare_symbols(char ch, char symbols[], int size);
void printTokenList(token lexeme_table[], int size);
int tokenize(char* arg, token* lexeme_table, int printLexeme);
