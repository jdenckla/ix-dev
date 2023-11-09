CC = gcc
CFLAGS = -g
DEPS = MCP.h
OBJ = part1.o part2.o part3.o part4.o
NAME = part1 part2 part3 part4

all: $(NAME)

%.o: %.c $(DEPS)
	$(CC) $< $(CFLAGS) -c -o $@

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -f *.o $(NAME)