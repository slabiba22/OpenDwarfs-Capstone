CC       = g++
CXXFLAGS = -Wall -Werror -O2 -fopenmp
LDLIBS   = -fopenmp -pthread
OBJECTS = gameoflife.o gol_serial.o gol_parallel.o 

all: gameoflife

gameoflife.o: gameoflife.cpp gameoflife.h

gol_serial.o: gol_serial.cpp gol_serial.h

gol_parallel.o: gol_parallel.cpp gol_parallel.h


gameoflife: $(OBJECTS) gol_parallel.h
	$(CC) $(CXXFLAGS) -o gameoflife $(OBJECTS) $(LDLIBS)

clean:
	rm -f *.o *~ gameoflife
	