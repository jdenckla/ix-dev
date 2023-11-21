CC = gcc
CFLAGS = -g -pthread -lpthread

all: bank

bank: bank.o account.h
	$(CC) $(CFLAGS) -o bank bank.o

bank.o: bank.c
	$(CC) $(CFLAGS) -c bank.c

clean:
	rm -f *.o bank