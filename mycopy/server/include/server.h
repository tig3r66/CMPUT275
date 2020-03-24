//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 1: Directions (Server)
//  ========================================

#ifndef SERVER_H
#define SERVER_H

#include "wdigraph.h"
#include "dijkstra.h"
#include "points.h"
#include "serialport.h"
#include <unordered_map>

#define SCALE 100000

/*
    Finds the closest waypoint on the map to the current vertex based on
    Manhattan distance.
*/
int findClosestPointOnMap(const Point& currPt,
    const unordered_map<int, Point> points);

/*
    Determines whether a path is possible between two points. If possible, it
    builds a path of waypoints to traverse.
*/
bool findShortestPath(unordered_map<int, PIL>& tree, list<int>& path,
    int start, int end);

/*
    Computes the Manhattan distance between two Point objects.
*/
long long manhattan(const Point&, const Point&);

/*
    Helper function that processes valid requests sent to the server via stdin
    and stdout. This function will print waypoints to the command-line from the
    requested start and end points.
*/
void processWaypoints(unordered_map<int, Point>& points, list<int>& path,
    SerialPort Serial);

/*
    Reads the Edmonton map from the provided file and loads it into the WDigraph
    object. The function stores vertex coordinates in the Point struct and maps
    each vertex to its corresponding Point struct.
*/
void readGraph(string filename, WDigraph& graph,
    unordered_map<int, Point>& points);

/*
    Validates whether the input file can be opened. If not, it exits the
    program and informs the user of the correct usage.
*/
void isValidIfstream(const ifstream& filename);

#endif
