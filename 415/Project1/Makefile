all : project1
	
	
project1: main.o command.o
	gcc -o project1 main.o command.o

command.o: command.c command.h
	gcc -c command.c	
	
main.o: main.c
	gcc -c main.c
	
clean:
	rm -f core *.o project1
