all : print

print: printf.o
	gcc printf.o -o print

print.o : printf.c
	gcc -c printf.c

clean:
	rm -rf printf.o print