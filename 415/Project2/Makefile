CC = gcc
CFLAGS = -g
DEPS = hellomake.h
OBJ = hellomake.o hellofunc.o
NAME = hello

%.o: %.c $(DEPS)
	$(CC) $< $(CFLAGS) -c -o $@

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(NAME)