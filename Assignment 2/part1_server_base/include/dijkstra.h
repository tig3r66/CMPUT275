#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "wdigraph.h"
#include <utility>
#include <unordered_map>

using namespace std;


typedef pair<int, long long> PIL;

// used to store a vertex v and its predecessor pair (u,d) on the search
// where u is the node prior to v on a path to v of cost d
// eg. PIPIL x;
// x.first is "v", x.second.first is "u" and x.second.second is "d" from this
typedef pair<int, PIL> PIPIL;


// NOTE: you are not required to use PIPIL in your solution if you would prefer
// to implement Dijkstra's algorithm differently, this is here simply because
// it was used in the lecture for the slower version of Dijkstra's algorithm.
void dijkstra(const WDigraph& graph, int startVertex,
    unordered_map<int, PIL>& tree);

#endif
