#ifndef SERVER_H
#define SERVER_H

#include "wdigraph.h"
#include "dijkstra.h"
#include <unordered_map>

#define SCALE 100000

// stores a point read from the map in 100,000-ths of a degree as integers
struct Point {
	long long lat;
	long long lon;
};

bool findShortestPath(unordered_map<int, PIL>, list<int>&, int start, int end);

long long findClosestPointOnMap(const Point&, unordered_map<int, Point>);

long long manhattan(const Point&, const Point&);

void readGraph(string file, WDigraph&, unordered_map<int, Point>&);

void isValidIfstream(const ifstream& filename);

#endif
