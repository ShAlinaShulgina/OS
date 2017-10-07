all: ls

ls: ls.o
	gcc ls.o -o ls

ls.o: ls.c
	gcc -c  ls.c

clean:
	rm -rf ls.o ls

