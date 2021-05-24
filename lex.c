#include "lex.h"

/*==========UTILITY FUNCTIONS==========*/

/* Adds character to the end of a string
 * @param buffer: temporary array storing token value
 * @param i: index in the array where '\0' is located
 * @param ch: character being added to the buffer
 */
void append_string(char* buffer, int* i, char ch) {
    buffer[*i] = ch;
    buffer[*i+1] = '\0';
    *i += 1;
}

// returns 1 if given string is a reserved word else returns 0
int compare_words(char* buffer, char* word[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(buffer, word[i]) == 0)
            return 1;
    }
    return 0;
}

// returns 1 if given char is a special symbol else returns 0
int compare_symbols(char ch, char symbols[], int size) {
    for (int i = 0; i < size; i++) {
        if (ch == symbols[i])
            return 1;
    }
    return 0;
}

// prints all tokens inside the list
void printTokenList(token lexeme_table[], int size) {
    for (int i = 0; i < size; i++) {
        token current = lexeme_table[i];
        printf("%d ", current.token_type);

        if (current.token_type == identsym  || current.token_type == numbersym) {
            printf("%s ", lexeme_table[i].value);
        }
    }
    printf("\n");
}

/*======================================*/

/* Tokenizes the input from the given file
 * @param arg: file name given in the command line
 * @param lexeme_table: array storing the tokens
 * @param printLexeme: determines if the output will be printed
 */
int tokenize(char* arg, token* lexeme_table, int printLexeme) {

    // set to 1 if -l command line argument was given
    int printOutput = printLexeme;

    FILE* fp = fopen(arg, "r");

    // list of reserved words
    char* word[] = {"begin", "call", "const", "do", "else", "end", "if",
                    "odd", "procedure", "read", "then", "var", "while", "write", "return"};

    char symbols[] = {'+', '-', '/', '*', '%', ',', ';', '.', ':', '(', ')', '=', '\'', '\"', '>', '<'};

    char ch;
    int table_index = 0; // points to top of lexeme_table

    char buffer[12] = {'\0'}; // temp buffer for storing lexemes
    int i = 0; // points to the end of string

    int isCommented = 0;      // equal to 1 if current input is in a comment
    int isIdentifier = 0;     // equal to 1 if current input is an identifier
    int exceedsNumLength = 0; // equal to 1 if a number is more than 5 digits

    char temp;

    if (printOutput) {
        printf("Lexeme Table:\n");
        printf("       lexeme       token type\n");
    }

    while ((ch = fgetc(fp)) != EOF) {

        if (isspace(ch)) continue;

        i = 0;
        append_string(buffer, &i, ch);

        // checks to see if input is the start of a comment
        if (ch == '/') {

            ch = fgetc(fp);
            if (ch == '*') {
                isCommented = 1;
            }

            else if (isCommented == 1) continue;

            else {
                lexeme_table[table_index].token_type = slashsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
            }
            ungetc(ch, fp);
        }

        else if (ch == '*') {
            if (isCommented == 1) {
                ch = fgetc(fp);
            }

            // checks to see if the input is the end of a comment
            if (ch == '/') {
                isCommented = 0;
            }

            else if (isspace(ch) || isCommented == 1) continue;

            else {
                lexeme_table[table_index].token_type = multsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
            }
        }

        // checks to see if current input is inside of a comment
        else if (isCommented == 1) {
            continue;
        }

        else if (isalpha(ch)) {
            ch = fgetc(fp);
            if (!compare_symbols(ch, symbols, nosym) && (isspace(ch) == 0) && ch != EOF)
                append_string(buffer, &i, ch);

            while (isalpha(ch) || isdigit(ch)) {
                ch = fgetc(fp);
                if ((i+1) < 13 && (isspace(ch) == 0) && !compare_symbols(ch, symbols, nosym) && ch != EOF) {
                    append_string(buffer, &i, ch);
                }
            }

            // checks to see if the identifier name is too long
            if (strlen(buffer) > 11) {
                if (printOutput) printf("Error : Identifier names cannot exceed 11 characters\n");
                return 1;
                ungetc(ch, fp);
                buffer[0] = '\0';
                continue;
            }

            if (strcmp(buffer, "var") == 0) {
                lexeme_table[table_index].token_type = varsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "const") == 0) {
                lexeme_table[table_index].token_type = constsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "procedure") == 0) {
                lexeme_table[table_index].token_type = procsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "begin") == 0) {
                lexeme_table[table_index].token_type = beginsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "end") == 0) {
                lexeme_table[table_index].token_type = endsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "while") == 0) {
                lexeme_table[table_index].token_type = whilesym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "do") == 0) {
                lexeme_table[table_index].token_type = dosym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "call") == 0) {
                lexeme_table[table_index].token_type = callsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "odd") == 0) {
                lexeme_table[table_index].token_type = oddsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "if") == 0) {
                lexeme_table[table_index].token_type = ifsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "else") == 0) {
                lexeme_table[table_index].token_type = elsesym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "then") == 0) {
                lexeme_table[table_index].token_type = thensym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "read") == 0) {
                lexeme_table[table_index].token_type = readsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "write") == 0) {
                lexeme_table[table_index].token_type = writesym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (strcmp(buffer, "return") == 0) {
                lexeme_table[table_index].token_type = returnsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            if (compare_words(buffer, word, norw) == 0) {
                lexeme_table[table_index].token_type = identsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            }

            table_index++;
            ungetc(ch, fp);
        }

        else if (ch == ',') {
            lexeme_table[table_index].token_type = commasym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == ';') {
            lexeme_table[table_index].token_type = semicolonsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == '.') {
            lexeme_table[table_index].token_type = periodsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == '%') {
            lexeme_table[table_index].token_type = modsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == ':') {
            ch = fgetc(fp);
            if (ch == '=') {
                append_string(buffer, &i, ch);
                lexeme_table[table_index].token_type = becomessym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
            }
        }

        else if (ch == '=') {
            lexeme_table[table_index].token_type = eqsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == '+') {
            lexeme_table[table_index].token_type = plussym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == '-') {
            lexeme_table[table_index].token_type = minussym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == '(') {
            lexeme_table[table_index].token_type = lparentsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (ch == ')') {
            lexeme_table[table_index].token_type = rparentsym;
            strcpy(lexeme_table[table_index].value, buffer);
            if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
            table_index++;
        }

        else if (isdigit(ch)) {
            ch = fgetc(fp);

            if (isdigit(ch) && !compare_symbols(ch, symbols, nosym)) {
                append_string(buffer, &i, ch);
            }

            while (isdigit(ch) || isalpha(ch)) {
                // checks to see if current input is an identifier
                if (isalpha(ch)) {
                    isIdentifier = 1;
                }

                ch = fgetc(fp);
                if ((i+1) < 13 && (isspace(ch) == 0) && !compare_symbols(ch, symbols, 9)) {
                    append_string(buffer, &i, ch);
                }

                // checks to see if a number is greater than 5
                if (isIdentifier == 0 && (strlen(buffer) > 5)) {
                    exceedsNumLength = 1;
                }
            }

            if (exceedsNumLength == 1) {
                if (printOutput) printf("Error : Numbers cannot exceed 5 digits\n");
                return 1;
                exceedsNumLength = 0;
                isIdentifier = 0;
            }

            else if (isIdentifier == 1 || isalpha(ch)) {
                if (printOutput) printf("Error : Identifiers cannot begin with a digit\n");
                return 1;
                isIdentifier = 0;
            }

            else {
                lexeme_table[table_index].token_type = numbersym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
            }

            ungetc(ch, fp);
        }

        else if (ch == '<') {
            ch = fgetc(fp);

            if (ch == '=') {
                append_string(buffer, &i, ch);
                lexeme_table[table_index].token_type = leqsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
                continue;
            }

            else {
                if (ch == '>') {
                    append_string(buffer, &i, ch);
                    lexeme_table[table_index].token_type = neqsym;
                    strcpy(lexeme_table[table_index].value, buffer);
                    if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                    table_index++;
                    continue;
                }
                else {
                    lexeme_table[table_index].token_type = lessym;
                    strcpy(lexeme_table[table_index].value, buffer);
                    if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                    table_index++;
                }
            }

            ungetc(ch, fp);
        }

        else if (ch == '>') {
            ch = fgetc(fp);

            if (ch == '=') {
                append_string(buffer, &i, ch);
                lexeme_table[table_index].token_type = geqsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
                continue;
            }

            else {
                lexeme_table[table_index].token_type = gtrsym;
                strcpy(lexeme_table[table_index].value, buffer);
                if (printOutput) printf("       %-11s  %d\n", lexeme_table[table_index].value, lexeme_table[table_index].token_type);
                table_index++;
            }

            ungetc(ch, fp);
        }

        else {
            if (printOutput) printf("Error : Invalid Symbol\n");
            return 1;
        }

        buffer[0] = '\0'; // clears buffer string
    }

    if (printOutput) {
        if (ch == EOF && isCommented == 1) {
            printf("Error : Invalid Symbol\n");
            return 1;
        }
    }

    if (printOutput) {
        printf("\nToken List:\n");
        printTokenList(lexeme_table, table_index);
    }

    return 0;

    fclose(fp);
}
