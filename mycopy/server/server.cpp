//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
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
int findClosestPointOnMap(const Point& currPt,
    const unordered_map<int, Point> points
) {
    pair<int, Point> minPnt = *points.begin();

	for (const auto& pt : points) {
        if (manhattan(currPt, pt.second) < manhattan(currPt, minPnt.second)) {
                minPnt = pt;
        };
    }
	return minPnt.first;
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
        graph (WDigraph&): the weighted directed graph to build.
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
            // SCALE = 100000
            long long convertedLon = static_cast<long long>(lon * SCALE);
            long long convertedLat = static_cast<long long>(lat * SCALE);
            // adding the vertex to the graph object
            point = {convertedLat, convertedLon};
            points[ID] = point;
            graph.addVertex(ID);
        } else if (graphID == 'E') {
            // edge format: E,start,end,name
            textIn >> start >> comma >> end >> comma;
            getline(textIn, name);
            // inserts Manhattan distance (cost of edge) into the graph object
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
void processWaypoints(unordered_map<int, Point>& points, list<int>& path,
    SerialPort Serial
) {
    Serial.writeline("N ");
    Serial.writeline(to_string(path.size()));
    Serial.writeline("\n\r");

    char input;
    for (int i = 0; i < path.size(); i++) {
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
    SerialPort Serial("/dev/ttyACM0");
    WDigraph graph;
    unordered_map<int, Point> points;
    unordered_map<int, PIL> tree;
    list<int> path;
    string line;
	long long startLon, startLat, endLon, endLat;
	int startIndex, endIndex;

    // builds the weighted directed graph from the yegGraph input file
    const char* yegGraph = "edmonton-roads-2.0.1.txt";
    readGraph(yegGraph, graph, points);

    // finite state machine for server
    enum {START, SEND_WAYPOINTS, END} currState = START;
    while (true) {
        while (currState != END) {
            line = Serial.readline();
            if (line[0] == 'R') cout << "YES" << endl;
        }
    }

    // cin >> input;
    // if (input == 'R') {
    //     cin >> startLat >> startLon >> endLat >> endLon;
    //     Point start = {startLat, startLon};
    //     Point end = {endLat, endLon};

    //     startIndex = findClosestPointOnMap(start, points);
    //     endIndex = findClosestPointOnMap(end, points);

    //     dijkstra(graph, startIndex, tree);

    //     if (!findShortestPath(tree, path, startIndex, endIndex)) {
    //         Serial.writeline("N 0\n\r");
    //     } else {
    //         processWaypoints(points, path, Serial);
    //     }
    // }

	return 0;
}
