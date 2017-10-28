all : fs

fs: fs.o
	gcc fs.o -o fs

fs.o: fs.c
	gcc -c fs.c

clean:
	rm -rf fs.o fs