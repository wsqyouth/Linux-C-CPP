CC = g++
CFLAGS = -Wall -std=c++11

OBJS = BaseClass.o IntermediateClass.o FinalClass.o main.o

all: program

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS)

BaseClass.o: BaseClass.cpp BaseClass.h
	$(CC) $(CFLAGS) -c BaseClass.cpp

IntermediateClass.o: IntermediateClass.cpp IntermediateClass.h BaseClass.h
	$(CC) $(CFLAGS) -c IntermediateClass.cpp

FinalClass.o: FinalClass.cpp FinalClass.h IntermediateClass.h
	$(CC) $(CFLAGS) -c FinalClass.cpp

main.o: main.cpp FinalClass.h
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o program