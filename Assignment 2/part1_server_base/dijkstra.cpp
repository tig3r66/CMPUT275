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


/*
    Computes the least cost paths that starts from a given vertex. This
    implementation of Dijkstra's algorithm uses a binary heap to efficiently
    retrieve an unexplored vertex that has the minimum distance from the start
    vertex at every iteration.

    Parameters:
        graph (const WDigraph&): instance of a weighted directed graph.
        startVertex (int): ID of the start vertex.
        tree (unordered_map<int, PIL>&): the search tree to build.
    Note:
        PIL is an alias for "pair<int, long long>".
*/
void dijkstra(const WDigraph& graph, int startVertex,
    unordered_map<int, PIL>& tree
) {
	BinaryHeap< pair<int, int>, long long> events;
    // placing the first vertex into the events heap (cost = 0)
    pair<int, int> tempPair = make_pair(startVertex, startVertex);
	events.insert(tempPair, 0);

	while (events.size() > 0) {
		HeapItem< pair<int, int>, long long> currentEvent = events.min();
		events.popMin();
        int v = currentEvent.item.second;

		if (tree.find(v) == tree.end()) {
            // vertex v visited, record the predecessor
			tree[v] = PIL(currentEvent.item.first, currentEvent.key);
            // search each neighbour of vertex v
			for (auto iter = graph.neighbours(v); iter != graph.endIterator(v);
                iter++
            ) {
                tempPair = make_pair(v, *iter);
                events.insert(tempPair,
                    currentEvent.key + graph.getCost(v, *iter));
			}
		}
	}
}



/*
    Computes the Manhattan distance between two Point objects.

    Parameters:
        pt1 (const Point&): the first point.
        pt2 (const Point&): the second point.
    Returns:
        |z| (int): the Manhattan distance between pt1 and pt2.
*/
long long heuristic(const Point& pt1, const Point& pt2) {
	return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}


void astar(const WDigraph& graph, int startVertex, int endVertex,
    unordered_map<int, PIL>& tree, unordered_map<int, Point>& points
) {
	BinaryHeap< pair<int, int>, long long> events;
    // placing the first vertex into the events heap (cost = 0)
    pair<int, int> tempPair = make_pair(startVertex, startVertex);
	events.insert(tempPair, 0);

	while (events.size() > 0) {
		HeapItem< pair<int, int>, long long> currentEvent = events.min();
		events.popMin();
		int v = currentEvent.item.second;

		if (tree.find(v) == tree.end()) {
            // vertex v visited, record the predecessor
			tree[v] = PIL(currentEvent.item.first, currentEvent.key);
            // search each neighbour of vertex v
			for (auto iter = graph.neighbours(v); iter != graph.endIterator(v);
                iter++
            ) {
                tempPair = make_pair(v, *iter);
                events.insert(tempPair,
                    currentEvent.key + graph.getCost(v, *iter));
                    // + heuristic(points[*iter], points[endVertex]));
			}
		}
        if (v == endVertex) return;
	}
}
