#include "vm.h"

/* Returns index some L levels down. Used to find a
 * variable in a different Activation Record
 * @param stack: array representing the program's stack
 * @param level: how many levels the functions will down
 * @param BP: the base pointer
 */
int base(int* stack, int level, int BP) {

    int base = BP;
    int counter = level;

    while (counter > 0) {
        base = stack[base];
        counter--;
    }

    return base;
}

// Runs the virtual machine and prints output if printVM = 1
void runVM(int printVM) {

    SP = -1;
    BP = PC = 0;
    IR = NULL;

    // secondary stack used for keeping track of activation records
    int AR[10];
    int arPointer = -1;

    memset(stack, 0, MAX_STACK_HEIGHT);
    memset(AR, -1, 10);

    if (printVM) {
        printf("                PC   BP   SP   stack\n");
        printf("Initial values: %2d    %d   %d\n", PC, BP, SP);
    }

    int halt = 1;

    while (halt == 1) {

        // Fetch instruction
        IR = &code[PC];
        int tempPC = PC;
        PC++;

        // Execute instruction
        switch(IR->opcode) {

            case 1: // LIT
                SP++;
                stack[SP] = IR->m;
                if (printVM) printf("%2d LIT %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 2: // OPR
                if (printVM) printf("%2d OPR %2d %2d ", tempPC, IR->l, IR->m);

                switch (IR->m) {

                    case 0: // RTN
                        arPointer--;
                        stack[BP-1] = stack[SP];
                        SP = BP-1;
                        BP = stack[SP+2];
                        PC = stack[SP+3];
                        break;

                    case 1: // NEG
                        stack[SP] = -1 * stack[SP];
                        break;

                    case 2: // ADD
                        SP--;
                        stack[SP] += stack[SP+1];
                        break;

                    case 3: // SUB
                        SP--;
                        stack[SP] -= stack[SP+1];
                        break;

                    case 4: // MUL
                        SP--;
                        stack[SP] *= stack[SP+1];
                        break;

                    case 5: // DIV
                        SP--;
                        stack[SP] /= stack[SP+1];
                        break;

                    case 6: // ODD
                        stack[SP] %= 2;
                        break;

                    case 7: // MOD
                        SP--;
                        stack[SP] %= stack[SP+1];
                        break;

                    case 8: // EQL
                        SP--;
                        stack[SP] = (stack[SP] == stack[SP+1]);
                        break;

                    case 9: // NEQ
                        SP--;
                        stack[SP] = (stack[SP] != stack[SP+1]);
                        break;

                    case 10: // LSS
                        SP--;
                        stack[SP] = (stack[SP] < stack[SP+1]);
                        break;

                    case 11: // LEQ
                        SP--;
                        stack[SP] = (stack[SP] <= stack[SP+1]);
                        break;

                    case 12: // GTR
                        SP--;
                        stack[SP] = (stack[SP] > stack[SP+1]);
                        break;

                    case 13: // GEQ
                        SP--;
                        stack[SP] = (stack[SP] >= stack[SP+1]);
                        break;

                    default: break;
                }
                break;

            case 3: // LOD
                SP++;
                stack[SP] = stack[base(stack, IR->l, BP) + IR->m];
                if (printVM) printf("%2d LOD %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 4: // STO
                stack[base(stack, IR->l, BP) + IR->m] = stack[SP];
                SP--;
                if (printVM) printf("%2d STO %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 5: // CAL
                arPointer++;
                AR[arPointer] = SP+1;
                stack[SP+1] = base(stack, IR->l, BP); // static link
                stack[SP+2] = BP;                     // dynamic link
                stack[SP+3] = PC;                     // return address
                stack[SP+4] = stack[SP];              // parameter
                BP = SP + 1;
                PC = IR->m;
                if (printVM) printf("%2d CAL %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 6: // INC
                SP += IR->m;
                if (printVM) printf("%2d INC %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 7: // JMP
                if (printVM) printf("%2d JMP %2d %2d ", tempPC, IR->l, IR->m);
                PC = IR->m;
                break;

            case 8: // JPC
                if (!stack[SP]) PC = IR->m;
                SP--;
                if (printVM) printf("%2d JPC %2d %2d ", tempPC, IR->l, IR->m);
                break;

            case 9: // SYS
                switch (IR->m) {
                    case 1:
                        printf("Top of Stack Value: %d\n", stack[SP]);
                        if (printVM) printf("%2d SYS %2d %2d ", tempPC, IR->l, IR->m);
                        SP--;
                        break;

                    case 2:
                        SP++;
                        printf("Please Enter an Integer: ");
                        scanf("%d", &stack[SP]);
                        if (printVM) printf("\n%2d SYS %2d %2d ", tempPC, IR->l, IR->m);
                        break;

                    case 3:
                        halt = 0;
                        if (printVM) printf("%2d SYS %2d %2d ", tempPC, IR->l, IR->m);
                        break;

                    default: printf("error\n");
                }
                break;

            default:
                printf("Error\n");
                halt = 0;
        }

        if (printVM) printf("   %2d   %2d   %2d   ", PC, BP, SP);

        // prints the stack trace if printVM = 1
        if (printVM) {
            for (int i = 0; i <= SP; i++) {

            // if i == address at AR[j] then an activation record begins here
            for (int j = 0; j <= arPointer; j++) {
                if (AR[j] == i) {
                    printf("| ");
                }
            }
            printf("%d ", stack[i]);
            }
            printf("\n");
        }
    }
}
