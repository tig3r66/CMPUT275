//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: [redacted]
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 1: Directions (Server)
//  ========================================

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

/*
    Computes the least cost paths that starts from a given vertex. This
    implementation of Dijkstra's algorithm uses a binary heap to efficiently
    retrieve an unexplored vertex that has the minimum distance from the start
    vertex at every iteration.
*/
void dijkstra(const WDigraph& graph, int startVertex,
    unordered_map<int, PIL>& tree);

#endif
