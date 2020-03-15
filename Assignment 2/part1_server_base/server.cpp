//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: [redacted]
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 1: Directions (Server)
//  ========================================

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <list>
#include "include/server.h"
#include "include/wdigraph.h"
#include "include/dijkstra.h"

using namespace std;


/*
    Finds the closest waypoint on the map to the current vertex based on
    Manhattan distance.

    Parameters:
        currPoint (const Point&): the current point.
        points (const unordered_map<int, Point>): the set of points to search
            in order to find the closest point to currPoint.
    Returns:
        pointID (int): the ID of the closest point to currPoint.
*/
long long findClosestPointOnMap(const Point& currPoint,
    const unordered_map<int, Point> points
) {
	long long minDistance;
	int pointID;
    bool firstRun = true;

	for (auto x : points) {
		long long ptDistance = manhattan(x.second, currPoint);
        // initializing minDistance for the first run
        if (firstRun) {
            pointID = x.first;
            minDistance = ptDistance;
            firstRun = false;
        }
        // searching to find the closest point to currPoint
        if (ptDistance < minDistance) {
			pointID = x.first;
			minDistance = ptDistance;
		}
	}

	return pointID;
}


/*
    Determines whether a path is possible between two points. If possible, it
    builds a path of waypoints to traverse.

    Parameters:
        tree (unordered_map<int, PIL>&): the search tree to obtain the path.
        path (list<int>&): the path of waypoints to build.
    Returns:
        bool: true if a path is possible, otherwise false.
*/
bool findShortestPath(unordered_map<int, PIL>& tree, list<int>& path,
    int start, int end
) {
	if (tree.find(end) == tree.end()) {
		return false;
	} else {
		int currentNode = end;
		while (currentNode != start) {
			path.push_front(currentNode);
			currentNode = tree[currentNode].first;
		}

		path.push_front(start);
		return true;
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
long long manhattan(const Point& pt1, const Point& pt2) {
	return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}


/*
    Validates whether the input file can be opened. If not, it exits the
    program and informs the user of the correct usage.

    Parameters:
        filename (const ifstream&): the filename to verify.
*/
void isValidIfstream(const ifstream& filename) {
    if (!filename.good()) {
	    cout << "error: digraph text file not found" << endl;
	    exit(EXIT_FAILURE);
    }
}


/*
    Reads the Edmonton map from the provided file and loads it into the WDigraph
    object. The function stores vertex coordinates in the Point struct and maps
    each vertex to its corresponding Point struct.

    Parameters:
        filename (const char*): the filename describing the road network file.
        graph (WDigraph&): the weighted directed map to build.
        points (unordered_map<int, Point>&): a mapping between vertex
            identifiers and their coordinates.
*/
void readGraph(const char* filename, WDigraph& graph,
    unordered_map<int, Point>& points
) {
	// input validation
	ifstream textIn(filename);
    isValidIfstream(textIn);

	char graphID, comma;
    int ID, start, end;
	double lat, lon;
	string name;
    Point point;

	while (textIn >> graphID >> comma) {
        if (graphID == 'V') {
            // vertex format: V,ID,Lat,Lon
            textIn >> ID >> comma >> lat >> comma >> lon;
            long long convertedLon = static_cast<long long>(lon * SCALE);
            long long convertedLat = static_cast<long long>(lat * SCALE);

            point = {convertedLat, convertedLon};
            points[ID] = point;
            graph.addVertex(ID);
        } else if (graphID == 'E') {
            // edge format: E,start,end,name
            textIn >> start >> comma >> end >> comma;
            getline(textIn, name);

            // find points and gets Manhattan distance
            long long distance = manhattan(points[start], points[end]);
            graph.addEdge(start, end, distance);
        }
    }
    textIn.close();
}


/*
    Helper function that processes valid requests sent to the server via stdin
    and stdout. This function will print waypoints to the command-line from the
    requested start and end points.

    Parameters:
        points (unordered_map<int, Point>&): a mapping between vertex
            identifiers and their coordinates.
        path (list<int>&): the path of waypoints to traverse.
*/
void processWaypoints(unordered_map<int, Point>& points, list<int>& path) {
    char input;
    int numWaypoints = path.size();
    cout << "N " << numWaypoints << endl;

    for (int i = 0; i < numWaypoints; i++) {
        cin >> input;
        if (input == 'A') {
            int waypointID = path.front();
            path.pop_front();
            Point wayPoint = points[waypointID];
            cout << "W " << wayPoint.lat << " " << wayPoint.lon << endl;
        }
    }

    // last acknowledge
    cin >> input;
    if (input == 'A') {
        cout << 'E' << endl;
    }
}


int main(int argc, char* argv[]) {
    WDigraph graph;
    unordered_map<int, Point> points;
    unordered_map<int, PIL> tree;
    list<int> path;
	char input;
	long long startLon, startLat, endLon, endLat;
	const char* yegGraph = "edmonton-roads-2.0.1.txt";
	int startIndex, endIndex;

    // builds the weighted directed graph from the yegGraph input file
    readGraph(yegGraph, graph, points);

    cin >> input;
    if (input == 'R') {
        cin >> startLat >> startLon >> endLat >> endLon; 
        Point start = {startLat, startLon};
        Point end = {endLat, endLon};

        startIndex = findClosestPointOnMap(start, points);
        endIndex = findClosestPointOnMap(end, points);

        dijkstra(graph, startIndex, tree);
        // astar(graph, startIndex, endIndex, tree, points);

        if (!findShortestPath(tree, path, startIndex, endIndex)) {
            // no path
            cout << "N 0" << endl;
        } else {
            processWaypoints(points, path);
        }
    }

	return 0;
}
