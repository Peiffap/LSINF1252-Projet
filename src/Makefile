all: main
	make clean

main: main.o fractstack/fractstack.o prodcons/prodcons.o
	cd libfractal && make
	gcc -Wall -g -O0 -o main main.o fractstack/fractstack.o libfractal/libfractal.a prodcons/prodcons.o -lSDL2 -lpthread

main.o: main.c
	gcc -Wall -g -O0 -c main.c -lpthread

fractstack.o: fractstack/fractstack.c
	gcc -Wall -g -O0 -c fractstack/fractstack.c -lpthread

prodcons.o: prodcons/prodcons.c
	gcc -Wall -g -O0 -c prodcons/prodcons.c -lpthread

#test:
#	cd libfractal && make
#	cd Test && make
#	make clean

lib:
	cd libfractal && make
	make clean

# Still need to add Test/*.o between libfractal/*.o and fractstack/*.o
clean:
	rm -rf *.o libfractal/*.o libfractal/*.a fractstack/*.o prodcons/*.o

.SILENT:
