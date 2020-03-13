#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <list>
#include "include/server.h"
#include "include/wdigraph.h"
#include "include/dijkstra.h"

using namespace std;


long long findClosestPointOnMap(const Point& point, unordered_map<long long, Point> points) {
	long long minDistance = 0;
	long long pointID;
	for (auto x: points) {
		long long ptDis = manhattan(x.second, point);
		if (ptDis < minDistance && minDistance != 0) {
			pointID = x.first;
			minDistance = ptDis;
		}
		else if (minDistance == 0) {
			pointID = x.first;
			minDistance = ptDis;
		}
	}
	return pointID;
}

// returns true if path is possible false if not
bool findShortestPath(unordered_map<int, PIL> tree, list<int> path, long long start, long long end) {
	if (tree.find(end) == tree.end()) {
		return false;
	}
	else {
		int currentNode = end;
		while (currentNode != end) {
			path.push_front(end);
			currentNode = tree[currentNode].first;
		}
		path.push_front(start);
		return true;
	}
}



long long manhattan(const Point& pt1, const Point& pt2) {
	return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}


void isValidIfstream(const ifstream& filename) {
    if (!filename.good()) {
	    cout << "error: digraph text file not found" << endl;
	    cout << "usage: ./graph_concepts <digraph_file_name>.txt" << endl;
	    exit(EXIT_FAILURE);
    }
}


void readGraph(string filename, WDigraph &graph, unordered_map<long long, Point> points) {
	char graphID, comma;
    int ID, start, end;
	float lat, lon;
	long long convertedLat, convertedLon, distance;
	string name;
    Point point;

	// input validation
	ifstream textIn(filename);
    isValidIfstream(textIn);

	while (textIn >> graphID >> comma) {
        if (graphID == 'V') {
            // vertex format: V,ID,Lat,Lon
            textIn >> ID >> comma >> lat >> comma >> lon;
            convertedLon = static_cast<long long>(lon * SCALE);
            convertedLat = static_cast<long long>(lat * SCALE);
            point = {convertedLat, convertedLon};
            points[ID] = point;
            graph.addVertex(ID);
        } else if (graphID == 'E') {
            // edge format: E,start,end,name
            textIn >> start >> comma >> end >> comma;
            getline(textIn, name);
            // find points and get manhattan distance
            distance = manhattan(points[start], points[end]);
            graph.addEdge(start, end, distance); 
        }
    }
} 


int main(int argc, char* argv[]) {
    WDigraph graph;
    unordered_map<long long, Point> points;
    unordered_map<int, PIL> tree;
    list<int> path;

	readGraph(argv[1], graph, points);

	char input;
	long startLon, startLat, endLon, endLat;
	long long startIndex, endIndex;
	Point start, end; // might not be needed
	// recall that cins and couts are serial inputs and outputs for pt2
	while (true) {
		cin >> input; 
		if (input == 'R') { 
			cin >> startLon >> startLat >> endLon >> endLat; 

			// move into functions?
			start = {startLat, startLon};
			end = {endLat, endLon};

			startIndex = findClosestPointOnMap(start, points);
			cout << startIndex << endl;
			endIndex = findClosestPointOnMap(end, points);
			cout << endIndex << endl;
			dijkstra(graph, startIndex, tree);
			if (!findShortestPath(tree, path, startIndex, endIndex)) {
				cout << '0' << endl;
			}
			else {
				cout << (path.size() - 8) << endl;
			}
			break;
		}
	}

	return 0;
}
