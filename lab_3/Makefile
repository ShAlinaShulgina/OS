CC = gcc

LIBS = -lpthread

all : main

main: main.o
	$(CC) $(CFLAGS) main.o -o main $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -rf main.o main
