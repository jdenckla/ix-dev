CC = gcc
CFLAGS = -g

all: part1 cpubound iobound

part1: part1.o string_parser.o
	$(CC) $(CFLAGS) -o part1 part1.o string_parser.o

part1.o: part1.c
	$(CC) $(CFLAGS) -c part1.c

string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

clean:
	rm -f *.o part1 cpubound iobound