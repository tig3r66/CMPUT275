#  ========================================
#  Name: Edward (Eddie) Guo
#  ID: 1576381
#  CMPUT 275, Winter 2020
#
#  Assignment 2 Part 2: Directions (Server)
#  ========================================

CC=g++
OBJS=server.o dijkstra.o digraph.o serialport.o
CFLAGS=-Wall -c -std=c++11 -O3
PROG=server
INCLUDES=include/server.h include/wdigraph.h include/digraph.h\
	include/dijkstra.h include/points.h include/serialport.h


server: $(OBJS)
	@$(CC) $(OBJS) -o $(PROG)
	@echo server executable successfully generated.

server.o: server.cpp $(INCLUDES)
	@g++ server.cpp -o server.o $(CFLAGS)
	@echo server.o successfully compiled.

dijkstra.o: dijkstra.cpp include/dijkstra.h include/heap.h
	@g++ dijkstra.cpp -o dijkstra.o $(CFLAGS)
	@echo dijkstra.o successfully compiled.

digraph.o: digraph.cpp include/digraph.h
	@g++ digraph.cpp -o digraph.o $(CFLAGS)
	@echo digraph.o successfully compiled.

serialport.o: serialport.cpp include/serialport.h
	@g++ serialport.cpp -o serialport.o $(CFLAGS)
	@echo serialport.o successfuly compiled

help:
	@echo Executable targets: $(PROG)
	@echo Object targets: $(OBJS)
	@echo Includes: $(INCLUDES)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROG)
	@echo Object files and executables successfully removed.
