#include "wdigraph.h"
#include <unordered_map>
#define SCALE 100000

struct Point {
	long long lat;
	long long lon;
};

void hold();

void processRequest(long, long, long, long, WDigraph&);

void serverComm(WDigraph&);

long long manhattan(const Point&, const Point&);

void readGraph(string file, WDigraph&, unordered_map<int, Point>);


