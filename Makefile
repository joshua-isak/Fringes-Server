CC = g++ -g -I. -pthread
CPPFLAGS = -g -I.
OBJS = main.o ship.o spaceport.o listener.o connection.o logger.o company.o star.o planet.o cargo.o
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

star.o:
	$(CC) -c server/src/instances/star.cpp -o star.o

planet.o:
	$(CC) -c server/src/instances/planet.cpp -o planet.o

cargo.o:
	$(CC) -c server/src/instances/cargo.cpp -o cargo.o

clean:
	rm *.o
