CC = g++ -g -I. -pthread
CPPFLAGS =
OBJS = main.o ship.o spaceport.o
PROGS = run.exe

default: build

build: $(OBJS)
	$(CC) $(OBJS) -o $(PROGS)
	rm $(OBJS)

main.o:
	$(CC) -c server/src/main.cpp -o main.o

ship.o:
	$(CC) -c server/src/ship.cpp -o ship.o

spaceport.o:
	$(CC) -c server/src/spaceport.cpp -o spaceport.o

clean:
	rm $(PROGS)
