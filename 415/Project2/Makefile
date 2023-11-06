CC = gcc
CFLAGS = -g

all: cpubound iobound part1 part2

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

part1: part1.o MCP.h
	$(CC) $(CFLAGS) -o part1 part1.o

part1.o: part1.c
	$(CC) $(CFLAGS) -c part1.c

part2: part2.o MCP.h
	$(CC) $(CFLAGS) -o part2 part2.o

part2.o: part2.c
	$(CC) $(CFLAGS) -c part2.c

clean:
	rm -f *.o cpubound iobound part1 part2