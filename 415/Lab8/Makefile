CC = gcc
CFLAGS = -g -pthread

all: lab8

lab8: lab8.o
	$(CC) $(CFLAGS) -o lab8 lab8.o

lab8.o: lab8.c
	$(CC) $(CFLAGS) -c lab8.c

clean:
	rm -f *.o lab8