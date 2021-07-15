CC=gcc
CFLAGS=-I . -Wall

all: search

range.o: range.c
	$(CC) $(CFLAGS) -c range.c -o range.o

search: main.c range.o
	$(CC) $(CFLAGS) main.c range.o -o search

clean:
	rm range.o
