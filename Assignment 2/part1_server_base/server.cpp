#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include "include/server.h"
#include "include/wdigraph.h"
#include "include/dijkstra.h"

using namespace std;


void hold() {
	char comm;
	//bool commed = true; // timeout implmentation for part 2

	while (true) { // add conditon regrading time for part 2
		cin >> comm;
		if (comm == 'A') { // wait for ACK key 
			//commed = true;
			break;
		} /*
		else {
			break;
		}
		*/
	}
}


void processRequest(long startLon, long startLat, long endLon, long endLat, WDigraph& graph) {
	// calc and print waypoints in a list or shit
	unordered_map<int, PIL> tree;
	Point minpoint, maxpoints;
	// for (auto x: )
	// dijkstra(graph, )
	/*
	if () { // if waypoints = 0
		return;
	}
	*/
	//hold();
	/*
	for () { // replace condition with number of waypoints in list
		//print each waypoint
		hold(); //wait for ACK
	}
	*/
	// cout << 'E' << endl; // end key, replace
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
            points.second = point;
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
    Point points;

	readGraph(argv[1], graph, points);
	char input;
	long startLon, startLat, endLon, endLat;
	while (true) {
		cin >> input; // replace with arduino serial input in part 2
		if (input == 'R') { //process request, server enters request handling
			cin >> startLon >> startLat >> endLon >> endLat; //replace with serial input in part 2
			processRequest(startLon, startLat, endLon, endLat, graph); //do djks
		}
	}

	return 0;
}
