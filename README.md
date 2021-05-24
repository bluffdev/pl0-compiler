# PL0-Compiler
Small compiler for pl0 code

There is a make file for this program. All source code and header files must be in the same directory for it to work.

Run the command "make clean" in the terminal to delete the object files

To compile the code, enter the "make" command in a terminal

The program takes in up to 4 arguments: <filename>.txt, -l, -a, -v (only <filename>.txt is required and it must be the first argument)

* -l prints the lexeme table and token list
* -a prints the generated assembly code
* -v prints the output of the virtual machine

These 3 arguments are not required and you can use any number of them in any order

To run the compiled program, type "./driver <filename>.txt" inside of a terminal in the directory that the file is stored in
