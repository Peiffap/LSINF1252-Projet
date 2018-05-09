all: test
	./test

test: test.o ../prodcons/prodcons.o ../fractstack/fractstack.o ../libfractal/tools.o ../libfractal/fractal.o
	gcc -Wall -lcunit -o test ../libfractal/fractal.o test.o ../prodcons/prodcons.o ../fractstack/fracttack.o ../libfractal/tools.o -lSDL -L$(HOME)/local/lib -lpthread

test.o: test.c
	gcc -Wall -c test.c -I$(HOME)/local/include
