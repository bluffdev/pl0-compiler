#include "parser.h"

int main(int argc, char** argv) {

    if (argv[1] == NULL) {
        fprintf(stderr, "No arguments given\n");
        return 1;
    }

    int printLexeme = 0;
    int printAssembly = 0;
    int printVM = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            printLexeme = 1;
        if (strcmp(argv[i], "-a") == 0)
            printAssembly = 1;
        if (strcmp(argv[i], "-v") == 0)
            printVM = 1;
    }

    parse(argv[1], printLexeme, printAssembly, printVM);

    return 0;
}
