# TODO


.PHONY: clean, mrproper
  
# définition des variables
CC = gcc
CFLAGS = -Wall -lpthread
EXEC=hello
 
 

all: $(EXEC)

main : main.o libfractal/fractal.o
	$(CC) main.o libfractal/fractal.o -o 

lib : libfractal/fractal.o
	$(CC) main.o libfractal/fractal.o -o 

tests: #TO DO
 
main.o: main.c fonctions.h
	$(CC) -c main.c -o main.o $(CFLAGS)
 
libfractal/fractal.o: libfractal/fractal.c
	$(CC) -c libfractal/fractal.c -o libfractal/fractal.o $(CFLAGS)
 
 
# clean
clean:
	rm -rf *.o
 
# mrproper
mrproper: clean
	rm -rf $(EXEC)
