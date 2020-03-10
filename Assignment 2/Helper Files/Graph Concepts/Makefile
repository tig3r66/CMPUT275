#  ==================================
#  Name: Edward (Eddie) Guo
#  ID: 1576381
#  CMPUT 275, Winter 2020
#
#  Weekly Exercise 5: Graph Concepts
#  ==================================

CC=g++
OBJS=graph_concepts.o digraph.o
INCLUDES=digraph.h
CFLAGS=-Wall -c -std=c++11 -O3
PROG=graph_concepts

$(PROG): $(OBJS)
	@$(CC) $(OBJS) -o $(PROG)
	@echo graph_concepts successfully generated.

graph_concepts.o: graph_concepts.cpp $(INCLUDES)
	@$(CC) graph_concepts.cpp -o graph_concepts.o $(CFLAGS)
	@echo graph_concepts.o successfully compiled.

digraph.o: digraph.cpp $(INCLUDES)
	@$(CC) digraph.cpp -o digraph.o $(CFLAGS)
	@echo digraph.o successfully compiled.

help:
	@echo Executable targets: $(PROG)
	@echo Object targets: $(OBJS)
	@echo Includes: $(INCLUDES)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROG)
	@echo Object files and executables successfully removed.
