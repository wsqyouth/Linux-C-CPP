CC = g++
CFLAGS = -Wall -std=c++11

OBJS = SimpleFactory.o main.o

all: program

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS)

SimpleFactory.o: SimpleFactory.cpp SimpleFactory.h
	$(CC) $(CFLAGS) -c SimpleFactory.cpp

main.o: main.cpp SimpleFactory.h
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o program