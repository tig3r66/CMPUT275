//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: [redacted]
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 1: Directions (Server)
//  ========================================

#include "include/dijkstra.h"
#include "include/heap.h"
#include <iostream>

using namespace std;


// tree is "reached"
void dijkstra(const WDigraph& graph, int startVertex,
    unordered_map<int, PIL>& tree
) {
	BinaryHeap< pair<int, int>, long long> events;
    pair<int, int> tempPair = make_pair(startVertex, startVertex);
	events.insert(tempPair, 0);

	while (events.size() > 0) {
		HeapItem< pair<int, int>, long long> currentFire = events.min();
		events.popMin();

		int v = currentFire.item.second;

		if (tree.find(v) == tree.end()) {
			tree[v] = PIL(currentFire.item.first, currentFire.key);
			for (auto iter = graph.neighbours(v); iter != graph.endIterator(v);
                iter++
            ) {
                tempPair = make_pair(v, *iter);
                events.insert(tempPair, currentFire.key + graph.getCost(v, *iter));
			}
		}
	}
}
