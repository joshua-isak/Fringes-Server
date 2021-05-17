CC = g++ -g -I. -pthread
CPPFLAGS = -g -I.
OBJS = main.o ship.o spaceport.o listener.o connection.o logger.o company.o
PROGS = run.exe

default: build

build: $(OBJS)
	$(CC) $(OBJS) -o $(PROGS)

main.o:
	$(CC) -c server/src/main.cpp -o main.o

ship.o:
	$(CC) -c server/src/ship.cpp -o ship.o

spaceport.o:
	$(CC) -c server/src/spaceport.cpp -o spaceport.o

listener.o:
	$(CC) -c server/src/network/listener.cpp -o listener.o

connection.o:
	$(CC) -c server/src/network/connection.cpp -o connection.o

logger.o:
	$(CC) -c server/src/logger.cpp -o logger.o

company.o:
	$(CC) -c server/src/company.cpp -o company.o

clean:
	rm *.o
