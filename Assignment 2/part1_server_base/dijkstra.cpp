#include "include/dijkstra.h"
#include "include/heap.h"

using namespace std;

void dijkstra(const WDigraph& graph, int startVertex,
              unordered_map<int, PIL>& tree) {
	// heap of all events, where pair represents edge on fire and key represents time fires reaches end node
	BinaryHeap<PIL, int> events; 
	//put fire in starting vertex
	HeapItem<PIL, int> firstFire = {PIL(startVertex,startVertex), 0};
	HeapItem<PIL, int currentFire;
	events.insert(firstFire);
	while (events.size() > 0) {
		currentFire = events.popMin();
		if (tree.find((currentFire.item).first) != tree.end()) {
			tree[(currentFire.item).first] = PIL((currentFire.item).first, (currentFire.item).second);
		}
	} 




}