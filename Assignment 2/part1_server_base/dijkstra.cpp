#include "include/dijkstra.h"
#include "include/heap.h"
#include <iostream>

using namespace std;

void dijkstra(const WDigraph& graph, int startVertex,
              unordered_map<int, PIL>& tree) {
	// heap of all events, where pair represents edge on fire and key represents time fires reaches end node
	BinaryHeap<PIL, int> events; 
	HeapItem<PIL, int> currentFire;
	events.insert(PIL(startVertex, startVertex), 0);
	while (events.size() > 0) {
		currentFire = events.min();
		events.popMin();
		auto v = currentFire.item.second;
		if (tree.find(v) == tree.end()) {
			tree[v] = PIL(currentFire.item.first, currentFire.key);
			for (auto iter = graph.neighbours(v); 
				iter != graph.endIterator(v); iter++) {
				events.insert(PIL(v, *iter), graph.getCost(v, *iter));
			}
		}
		//cout << tree.size() << endl;
	} 
}
