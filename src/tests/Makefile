all: test
	./test

test: test.o ../prodcons/prodcons.o ../fractstack/fractstack.o ../libfractal/tools.o ../libfractal/fractal.o
	gcc -Wall -o test ../libfractal/fractal.o test.o ../prodcons/prodcons.o ../fractstack/fractstack.o ../libfractal/tools.o -lSDL -L$(HOME)/local/lib -lpthread -lcunit

test.o: test.c
	gcc -Wall -c test.c -I$(HOME)/local/include
