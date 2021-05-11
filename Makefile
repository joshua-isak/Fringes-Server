CC = g++ -g -I. -pthread
CPPFLAGS = -g -I.
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

json.o:
	$(CC) -c server/lib/nlohmann/json.hpp -o json.hpp.gch

clean:
	rm $(PROGS)
