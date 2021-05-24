driver: driver.o vm.o lex.o parser.o
	gcc driver.o vm.o lex.o parser.o -o driver

driver.o: driver.c
	gcc -c driver.c

vm.o: vm.c
	gcc -c vm.c

lex.o: lex.c
	gcc -c lex.c

parser.o: parser.c
	gcc -c parser.c

clean:
	rm *.o driver
