CC = g++
CFLAGS = -Wall -std=c++11

OBJS = Factory.o main.o

all: program

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS)

Factory.o: Factory.cpp Factory.h
	$(CC) $(CFLAGS) -c Factory.cpp

main.o: main.cpp Factory.h
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o program