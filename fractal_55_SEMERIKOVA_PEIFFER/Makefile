CC=gcc
CFLAGS=-g -Wall

all: main

main: main.o fractstack/fractstack.o prodcons/prodcons.o
	cd libfractal && make
	$(CC) $(CFLAGS) -o main main.o fractstack/fractstack.o libfractal/libfractal.a prodcons/prodcons.o -lSDL -lpthread

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -lpthread

fractstack.o: fractstack/fractstack.c
	$(CC) $(CFLAGS) -c fractstack/fractstack.c -lpthread

prodcons.o: prodcons/prodcons.c
	$(CC) $(CFLAGS) -c prodcons/prodcons.c -lpthread

test:
	cd libfractal && make
	cd tests && make
	make clean

lib:
	cd libfractal && make
	make clean

clean:
	rm -rf *.o libfractal/*.o libfractal/*.a tests/*.o fractstack/*.o prodcons/*.o main

.SILENT:
