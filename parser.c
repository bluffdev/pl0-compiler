#include "parser.h"

symbol symbol_table[500];
int table_index;

token global_token;
int token_index;

token tokens[10000];

int procedurecount = 0;

int i = 0;

// sets global token equal to the current token in the list
void getToken() {
    global_token = tokens[token_index++];
}

void printSymbols() {
    for (int i = 0; i < table_index; i++)
        printf("name: %s level: %d mark: %d kind: %d\n", symbol_table[i].name, symbol_table[i].level, symbol_table[i].mark, symbol_table[i].kind);
}

// adds instruction to the buffer containing all of the instructions
void addInstruction(int opcode, int l, int m, char* op) {
    code[instruction_length].opcode = opcode;
    code[instruction_length].l = l;
    code[instruction_length].m = m;
    strcpy(code[instruction_length].op, op);
    instruction_length++;
}

// inserts symbol into the symbol table
void enter(int kind, char name[11], int val, int level, int addr, int mark, int param) {

    symbol* temp_symbol = &symbol_table[table_index++];

    temp_symbol->kind = kind;
    strcpy(temp_symbol->name, name);
    temp_symbol->val = val;
    temp_symbol->level = level;
    temp_symbol->addr = addr;
    temp_symbol->mark = mark;
    temp_symbol->param = param;
}

// returns index of symbol that matches name and lexlevel
int symbol_table_check(char* name, int lexlevel) {
    for (int i = 0; i < 500; i++) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].level == lexlevel && symbol_table[i].mark == 0)
            return i;
    }
    return -1;
}

// return index of symbol that matches name, lexlevel, and kind
int symbol_table_search(char* name, int lexlevel, int kind) {
    for (int i = table_index-1; i >= 0; i--) {
        if (strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].level <= lexlevel && symbol_table[i].kind == kind && symbol_table[i].mark == 0)
            return i;
    }
    return -1;
}

void mark(int count) {
    for (int i = table_index - 1; count > 0; i--) {
        if (symbol_table[i].mark == 0) {
            symbol_table[i].mark = 1;
            count--;
        }
    }
}

int find_procedure(int index) {
    for (int i = 0; i < 500; i++) {
        if (symbol_table[i].val == index && symbol_table[i].kind == 3)
            return i;
    }
    return -1;
}

void printProc() {
    for (int i = 0; i < 500; i++) {
        if (symbol_table[i].kind == 3)
            printf("addr: %d val: %d\n", symbol_table[i].addr, symbol_table[i].val);
    }
}

void program() {

    int numProc = 1;
    addInstruction(7, 0, 0, "JMP");

    for (int i = 0; i < 500; i++) {
        if (tokens[i].token_type == procsym) {
            numProc++;
            addInstruction(7, 0, 0, "JMP");
        }
    }

    enter(3, "main", 0, 0, 0, 0, 0);
    procedurecount++;

    block(0, 0, 0);

    if (global_token.token_type != periodsym) {
        printf("\n\nError : program must end with period\n");
        exit(0);
    }

    for (int i = 0; i < numProc; i++) {
        code[i].m = symbol_table[find_procedure(i)].addr;
    }

    for (int i = 0; i < instruction_length; i++) {
        if (code[i].opcode == 5)
            code[i].m = symbol_table[find_procedure(code[i].m)].addr;
    }

    addInstruction(9, 0, 3, "SYS"); // emit HALT
}

/*
 * These paramters are the same for the remaining functions in the parser
 * @param  lexlevel: current lexographical level
 * @param  param: equal to 1 if there is a paramater
 * @param  procedureIndex: index of current procedure in the symbol table
 */
void block(int lexlevel, int param, int procedureIndex) {

    int c = const_declaration(lexlevel);
    int v = var_declaration(lexlevel, param);
    int p = procedure_declaration(lexlevel);

    symbol_table[procedureIndex].addr = instruction_length;

    addInstruction(6, 0, 4 + v, "INC"); // emit INC (M = 4 + numVars)

    statement(lexlevel);

    mark(c + v + p);
}

// Adds constants to symbol table
int const_declaration(int lexlevel) {

    int numConst = 0;

    // checks if token is a const
    if (global_token.token_type == constsym) {

        do {
            numConst++;
            getToken();

            // checks if current token is an identifier
            if (global_token.token_type != identsym) {
                printf("\n\nError : const, var, procedure, call and read keywords must be followed by identifier\n");
                exit(0);
            }

            // checks if a symbol with the given identifier already exists in the table at the current level
            if (symbol_table_check(global_token.value, lexlevel) != -1) {
                printf("\n\nError : competing symbol declarations at the same level\n");
                exit(0);
            }

            char name[11];
            strcpy(name, global_token.value);

            getToken();

            // checks if current token value is =
            if (global_token.token_type != eqsym) {
                printf("\n\nError : constants must be assigned with =\n");
                exit(0);
            }

            getToken();

            // checks if current token is a number
            if (global_token.token_type != numbersym) {
                printf("\n\nError : constants must be assigned an integer value\n");
                exit(0);
            }

            enter(1, name, atoi(global_token.value), lexlevel, 0, 0, 0);
            getToken();

        } while (global_token.token_type == commasym);

        // checks if current token is a semicolon
        if (global_token.token_type != semicolonsym) {
            printf("\n\nError : symbol declarations must be followed by a semicolon\n");
            exit(0);
        }

        getToken();
    }

    return numConst;
}

// Adds vars to symbol table
int var_declaration(int lexlevel, int param) {

    int numVars;

    if (param == 1)
        numVars = 1;
    else
        numVars = 0;

    // checks if token is a variable
    if (global_token.token_type == varsym) {

         do {
            numVars++;
            getToken();

            // checks if current token is an identifier
            if (global_token.token_type != identsym) {
                printf("\n\nError : const, var, procedure, call and read keywords must be followed by identifier\n");
                exit(0);
            }

            // checks if a symbol with the given identifier already exists in the table at the current level
            if (symbol_table_check(global_token.value, lexlevel) != -1) {
                printf("\n\nError : symbol name has already been declared\n");
                exit(0);
            }

            enter(2, global_token.value, 0, lexlevel, numVars + 3, 0, 0); // adds variable to symbol table
            getToken();

        } while (global_token.token_type == commasym);

        // if there is no semicolon after the variable declarations than there is an error
        if (global_token.token_type != semicolonsym) {
            printf("\n\nError : symbol declarations must be followed by a semicolon\n");
            exit(0);
        }

        getToken();
    }

    return numVars;
}

// Adds procedures to symbol table
int procedure_declaration(int lexlevel) {

    int numProc = 0;

    if (global_token.token_type == procsym) {

        do {
            numProc++;
            getToken();

            // checks if current token is an identifier
            if (global_token.token_type != identsym) {
                printf("\n\nError : const, var, procedure, call, and read keywords must be followed by identifier\n");
                exit(0);
            }

            // checks if a symbol with the given identifier already exists in the table at the current level
            if (symbol_table_check(global_token.value, lexlevel) != -1) {
                printf("\n\nError : symbol name has already been declared\n");
                exit(0);
            }

            int procIdx = table_index;

            enter(3, global_token.value, procedurecount, lexlevel, 0, 0, 0);
            procedurecount++;

            getToken();

            // checks if there is a left paranthesis after the identifier indicating that the procedure has a parameter
            if (global_token.token_type == lparentsym) {
                getToken();

                if (global_token.token_type != identsym) {
                    printf("\n\nError : parameters may only be specified by an identifier\n");
                    exit(0);
                }

                enter(2, global_token.value, 0, lexlevel+1, 3, 0, 0);
                symbol_table[procIdx].param = 1;

                getToken();
                if (global_token.token_type != rparentsym) {
                    printf("\n\nError : right parenthesis must follow left parenthesis\n");
                    exit(0);
                }

                getToken();
                if (global_token.token_type != semicolonsym) {
                    printf("\n\nError : symbol declarations must be followed by a semicolon\n");
                    exit(0);
                }

                getToken();

                block(lexlevel + 1, 1, procIdx);
            }

            // the procedure does not have a paramter
            else {

                if (global_token.token_type != semicolonsym) {
                    printf("\n\nError : symbol declarations must be followed by a semicolon\n");
                    exit(0);
                }

                getToken();

                block(lexlevel + 1, 0, procIdx);

            }

            if (code[instruction_length-1].opcode != 2 && code[instruction_length-1].m != 0) {
                addInstruction(1, 0, 0, "LIT"); // emit LIT 0
                addInstruction(2, 0, 0, "OPR"); // emit RTN
            }

            // checks if current token is a semicolon
            if (global_token.token_type != semicolonsym) {
                printf("\n\nError : symbol declarations must be followed by a semicolon\n");
                exit(0);
            }

            getToken();

        } while (global_token.token_type == procsym);
    }

    return numProc;
}

void statement(int lexlevel) {

    // checks if current token is an identifier
    if (global_token.token_type == identsym) {

        int symIdx = symbol_table_search(global_token.value, lexlevel, 2);

        if (symIdx == -1) {
            printf("\n\nError : undeclared variable or constant in equation\n");
            exit(0);
        }

        getToken();

        // checks if current token is :=
        if (global_token.token_type != becomessym) {
            printf("\n\nError : assignment statements must use :=\n");
            exit(0);
        }

        getToken();
        expression(lexlevel);

        addInstruction(4, lexlevel - symbol_table[symIdx].level, symbol_table[symIdx].addr, "STO");  // emit STO (M = table[symIdx].addr)

        return;
    }

    // checks if current token is the call keyword
    if (global_token.token_type == callsym) {

        getToken();

        if (global_token.token_type != identsym) {
            printf("\n\nError : const, var, procedure, call, and read keywords must be followed by an identifier \n");
            exit(0);
        }

        int symIdx = symbol_table_search(global_token.value, lexlevel, 3);

        if (symIdx == -1) {
            printf("\n\nError : undeclared procedure for call\n");
            exit(0);
        }

        getToken();

        // checks if current token is a left parenthesis
        if (global_token.token_type == lparentsym) {
            getToken();

            if (symbol_table[symIdx].param != 1) {
                printf("\n\nError : paramters must be declared\n");
                exit(0);
            }

            expression(lexlevel);

            // checks if current token is a right parenthesis
            if (global_token.token_type != rparentsym) {
                printf("\n\nError : right parenthesis must follow left parenthesis\n");
                exit(0);
            }

            getToken();
        }
        else
            addInstruction(1, 0, 0, "LIT"); // emit LIT 0

        addInstruction(5, lexlevel - symbol_table[symIdx].level, symbol_table[symIdx].val, "CAL"); // emit CAL L = lexlevel - symIdx.level, M = symIdx.value

        return;
    }

    // checks if current token is the return keyword
    if (global_token.token_type == returnsym) {

        if (lexlevel == 0) {
            printf("\n\nError : cannot return from main\n");
            exit(0);
        }

        getToken();

        // checks if current token is a left parenthesis
        if (global_token.token_type == lparentsym) {
            getToken();
            expression(lexlevel);

            addInstruction(2, 0, 0, "OPR"); // emit RTN

            // checks if current token is a right parenthesis
            if (global_token.token_type != rparentsym) {
                printf("\n\nError : right parenthesis must follow left parenthesis\n");
                exit(0);
            }

            getToken();
        }
        else {
            addInstruction(1, 0, 0, "LIT"); // emit LIT 0
            addInstruction(2, 0, 0, "OPR"); // emit RTN
        }

        return;
    }

    // checks if current token is the begin keyword
    if (global_token.token_type == beginsym) {

        do {
            getToken();
            statement(lexlevel);
        } while (global_token.token_type == semicolonsym);

        // checks if current token is the end keyword
        if (global_token.token_type != endsym) {
            printf("\n\nError : begin must be followed by end\n");
            exit(0);
        }

        getToken();

        return;
    }

    // checks if current token is the if keyword
    if (global_token.token_type == ifsym) {

        getToken();
        condition(lexlevel);

        int jpcIdx = instruction_length; // stores the current index of the array of instructions

        addInstruction(8, 0, 0, "JPC"); // emit JPC

        // checks if current token is the then keyword
        if (global_token.token_type != thensym) {
            printf("\n\nError : if must be followed by then\n");
            exit(0);
        }

        getToken();
        statement(lexlevel);

        // checks if current token is the else keyword
        if (global_token.token_type == elsesym) {
            getToken();

            int jmpIdx = instruction_length;
            addInstruction(7, 0, 0, "JMP"); // emit JMP

            code[jpcIdx].m = instruction_length;
            statement(lexlevel);
            code[jmpIdx].m = instruction_length;
        }
        else {
            code[jpcIdx].m = instruction_length; // sets the m value of the instruction array at index jpcIdx to current index
        }

        return;
    }

    // checks if current token is the while keyword
    if (global_token.token_type == whilesym) {

        getToken();
        int loopIdx = instruction_length; // stores the current index of the array of instructions

        condition(lexlevel);

        // checks if current token is the do keyword
        if (global_token.token_type != dosym) {
            printf("\n\nError : while must be followed by do\n");
            exit(0);
        }

        getToken();
        int jpcIdx = instruction_length; // stores the current index of the array of instructions

        addInstruction(8, 0, 0, "JPC"); // emit JPC

        statement(lexlevel);

        addInstruction(7, 0, loopIdx, "JMP"); // emit JMP
        code[jpcIdx].m = instruction_length; // sets the m value of the instruction array at index jpcIdx to current index

        return;
    }

    // checks if token is the read keyword
    if (global_token.token_type == readsym) {

        getToken();

        if (global_token.token_type != identsym) {
            printf("\n\nError: const, var, procedure, call, and read keywords must be followed by an identifier\n");
            exit(0);
        }

        int symIdx = symbol_table_search(global_token.value, lexlevel, 2);

        if (symIdx == -1) {
            printf("\n\nError : undeclared variable or constant in equation\n");
            exit(0);
        }

        getToken();

        addInstruction(9, 0, 2, "SYS"); // emit READ
        addInstruction(4, lexlevel - symbol_table[symIdx].level, symbol_table[symIdx].addr, "STO"); // emit STO (M = table[symIdx].addr)

        return;
    }

    // checks if current token is the write keyword
    if (global_token.token_type == writesym) {
        getToken();
        expression(lexlevel);
        addInstruction(9, 0, 1, "SYS"); // emit WRITE
        return;
    }
}

void condition(int lexlevel) {

    // checks if current token is the odd keyword
    if (global_token.token_type == oddsym) {
        getToken();
        expression(lexlevel);
        addInstruction(2, 0, 6, "OPR"); // emit ODD
    }

    else {

        expression(lexlevel);

        // checks if currnet token is =
        if (global_token.token_type == eqsym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 8, "OPR"); // emit EQL
        }

        // checks if current token is <>
        else if (global_token.token_type == neqsym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 9, "OPR"); // emit NEQ
        }

        // checks if current token is <
        else if (global_token.token_type == lessym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 10, "OPR"); // emit LSS
        }

        // checks if current token is <=
        else if (global_token.token_type == leqsym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 11, "OPR"); // emit LEQ
        }

        // checks if current token is >
        else if (global_token.token_type == gtrsym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 12, "OPR"); // emit GTR
        }

        // checks if current token is >=
        else if (global_token.token_type == geqsym) {
            getToken();
            expression(lexlevel);
            addInstruction(2, 0, 13, "OPR"); // emit GEQ
        }

        else {
            printf("\n\nError : condition must contain comparison operator\n");
            exit(0);
        }
    }
}

void expression(int lexlevel) {

    // checks if current token is -
    if (global_token.token_type == minussym) {

        getToken();
        term(lexlevel);
        addInstruction(2, 0, 1, "OPR"); // emit NEG

        while (global_token.token_type == plussym || global_token.token_type == minussym) {

            // checks if current token is +
            if (global_token.token_type == plussym) {
                getToken();
                term(lexlevel);
                addInstruction(2, 0, 2, "OPR"); // emit ADD
            }

            // current token is -
            else {
                getToken();
                term(lexlevel);
                addInstruction(2, 0, 3, "OPR"); // emit SUB
            }
        }
    }

    else {

        // checks if current token is +
        if (global_token.token_type == plussym)
            getToken();

        term(lexlevel);

        while (global_token.token_type == plussym || global_token.token_type == minussym) {

            // checks if currnet token is +
            if (global_token.token_type == plussym) {
                getToken();
                term(lexlevel);
                addInstruction(2, 0, 2, "OPR"); // emit ADD
            }

            // current token is -
            else {
                getToken();
                term(lexlevel);
                addInstruction(2, 0, 3, "OPR"); // emit SUB
            }
        }
    }
}

void term(int lexlevel) {

    factor(lexlevel);

    while (global_token.token_type == multsym || global_token.token_type == slashsym || global_token.token_type == modsym) {

        // checks if current token is *
        if (global_token.token_type == multsym) {
            getToken();
            factor(lexlevel);
            addInstruction(2, 0, 4, "OPR"); // emit MUL
        }

        // checks if current token is /
        else if (global_token.token_type == slashsym) {
            getToken();
            factor(lexlevel);
            addInstruction(2, 0, 5, "OPR"); // emit DIV
        }

        // current token is %
        else {
            getToken();
            factor(lexlevel);
            addInstruction(2, 0, 7, "OPR"); // emit MOD
        }
    }
}

void factor(int lexlevel) {

    // checks if current token is an identifier
    if (global_token.token_type == identsym) {

        int symIdxV = symbol_table_search(global_token.value, lexlevel, 2);
        int symIdxC = symbol_table_search(global_token.value, lexlevel, 1);

        if (symIdxV == -1 && symIdxC == 1) {
            printf("\n\nError : undeclared variable or constant in equation\n");
            exit(0);
        }
        else if (symIdxC == -1 || (symIdxV != -1 && symbol_table[symIdxV].level > symbol_table[symIdxC].level))
            addInstruction(3, lexlevel - symbol_table[symIdxV].level, symbol_table[symIdxV].addr, "LOD"); // emit LOD L = lexlevel - symIdxV.level, M = symIdxV.addr
        else
            addInstruction(1, 0, symbol_table[symIdxC].val, "LIT"); // emit LIT M = symIdxC.value

        getToken();
    }

    // checks if current token is a number
    else if (global_token.token_type == numbersym) {
        addInstruction(1, 0, atoi(global_token.value), "LIT"); // emit LIT
        getToken();
    }

    // checks if current token is a left parenthesis
    else if (global_token.token_type == lparentsym) {

        getToken();
        expression(lexlevel);

        // checks if current token is a right parenthesis
        if (global_token.token_type != rparentsym) {
            printf("\n\nError : right parenthesis must follow left parenthesis\n");
            exit(0);
        }

        getToken();
    }

    // checks if current token is the call keyword
    else if (global_token.token_type == callsym) {
        statement(lexlevel);
    }

    else {
        printf("\n\nError : arithmetic equations must contain operands, parantheses, numbers, or symbols\n");
        exit(0);
    }
}



/* Begins parsing by generating the lexeme tables and calling program() and
 * prints output to the terminal based off of the command line arguments given
 * @param  arg: the file given in the command line
 * @param  printLexeme: equal to 1 if the command line argument "-l" is given
 * @param  printAssembly: equal to 1 if the command line argument "-a" is given
 * @param  printVM: equal to 1 if the command line argument "-v" is given
 */
void parse(char* arg, int printLexeme, int printAssembly, int printVM) {

    // returns 1 if the was an error when tokenizing the input
    int failed = tokenize(arg, tokens, printLexeme);

    if (failed) exit(0);

    token_index = 0;          // index for the token array
    table_index = 0;          // index for the symbol table array
    instruction_length = 0;   // index for the instructions array

    getToken();
    program();

    // prints list of instructions if -a argument was passed
    if (printAssembly) {
        if (printLexeme == 1) printf("\n\n");
        printf("Generated Assembly:\n");
        printf("Line   OP   L   M\n");
        for (int i = 0; i < instruction_length; i++)
            printf("%2d    %s  %2d  %2d\n", i, code[i].op, code[i].l, code[i].m);
    }

    // prints the vm output if the -v argument was passed
    if ((printLexeme == 1 || printAssembly == 1) && printVM == 1) printf("\n\n");
    runVM(printVM);
}
