CC = gcc
CFLAGS = -g

all: MCP1

MCP1: MCP1.o string_parser.o
	$(CC) $(CFLAGS) -o MCP1 MCP1.o string_parser.o

MCP1.o: MCP1.c
	$(CC) $(CFLAGS) -c MCP1.c

string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c

cpubound: cpubound.c
	$(CC) $(CFLAGS) -o cpubound.c -o

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o

clean:
	rm -f MCP1.o MCP1 string_parser.o