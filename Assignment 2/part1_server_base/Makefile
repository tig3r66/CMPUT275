server: server.o dijkstra.o digraph.o
	g++ server.o digraph.o dijkstra.o -o server
server.o: server.cpp 
	g++ server.cpp -o server.o -c -Wall
dijkstra.o: dijkstra.cpp 
	g++ dijkstra.cpp -o dijkstra.o -c -Wall
digraph.o: digraph.cpp
	g++ digraph.cpp -o digraph.o -c -Wall
clean: 
	rm -f server server.o dijkstra.o digraph.o



