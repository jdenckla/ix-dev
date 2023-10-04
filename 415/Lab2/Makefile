all : lab2.exe
	
	
lab2.exe: main.o command.o
	gcc -o lab2.exe main.o command.o

command.o: command.c command.h
	gcc -c command.c	
	
main.o: main.c
	gcc -c main.c
	
clean:
	rm -f core *.o lab2.exe
