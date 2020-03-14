#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <list>
#include "include/server.h"
#include "include/wdigraph.h"
#include "include/dijkstra.h"

using namespace std;


long long findClosestPointOnMap(const Point& currPoint,
    const unordered_map<int, Point> points
) {
	long long minDistance;
	int pointID, i = 0;

	for (auto x : points) {
		long long ptDistance = manhattan(x.second, currPoint);
        if (i == 0) {
            pointID = x.first;
            minDistance = ptDistance;
            i++;
        }

        if (ptDistance < minDistance) {
			pointID = x.first;
			minDistance = ptDistance;
		}
	}

	return pointID;
}


// returns true whether path is possible
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


void readGraph(const char* filename, WDigraph& graph,
    unordered_map<int, Point>& points
) {
	char graphID, comma;
    int ID, start, end;
	double lat, lon;
	string name;
    Point point;

	// input validation
	ifstream textIn(filename);
    isValidIfstream(textIn);

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

            // find points and get manhattan distance
            long long distance = manhattan(points[start], points[end]);
            graph.addEdge(start, end, distance);
        }
    }
    textIn.close();
}


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
        cout << "E" << endl;
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

    readGraph(yegGraph, graph, points);

	while (true) {
		cin  >> input;
		if (input == 'R') {
			cin >> startLat >> startLon >> endLat >> endLon; 
			Point start = {startLat, startLon};
			Point end = {endLat, endLon};

			startIndex = findClosestPointOnMap(start, points);
			endIndex = findClosestPointOnMap(end, points);

			dijkstra(graph, startIndex, tree);

			if (!findShortestPath(tree, path, startIndex, endIndex)) {
                // no path
				cout << "N 0" << endl;
			} else {
                processWaypoints(points, path);
                break;
			}
		}
	}

	return 0;
}
