all : lab2
	
	
lab2: main.o command.o
	gcc -o lab2 main.o command.o

command.o: command.c command.h
	gcc -c command.c	
	
main.o: main.c
	gcc -c main.c
	
clean:
	rm -f core *.o lab2
