CC = gcc
CFLAGS = -g

all: part1 part2 part3 part4

part1: part1.o MCP.h
	$(CC) $(CFLAGS) -o part1 part1.o

part1.o: part1.c
	$(CC) $(CFLAGS) -c part1.c

part2: part2.o MCP.h
	$(CC) $(CFLAGS) -o part2 part2.o

part2.o: part2.c
	$(CC) $(CFLAGS) -c part2.c

part3: part3.o MCP.h
	$(CC) $(CFLAGS) -o part3 part3.o

part3.o: part3.c
	$(CC) $(CFLAGS) -c part3.c

part4: part4.o MCP.h
	$(CC) $(CFLAGS) -o part4 part4.o

part4.o: part4.c
	$(CC) $(CFLAGS) -c part4.c
clean:
	rm -f *.o part1 part2 part3 part4