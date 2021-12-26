CC = g++
CFLAGS = -g -Wall

all: a.out

a.out: main.o auxiliary.o
	$(CC) $(CFLAGS) main.o auxiliary.o -o a.out -lsfml-graphics -lsfml-window -lsfml-system

auxiliary.o: auxiliary.hpp auxiliary.cpp
	$(CC) $(CFLAGS) -c auxiliary.cpp

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f a.out main.o auxiliary.o MiddleAverageFilter.h.gch 
