#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include "server.h"
#include "wdigraph.h"

using namespace std;

void hold() {
	char comm;
	//bool commed = true; //timeout implmentation for part 2

	while (true) { //add conditon regrading time for part 2
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

void processRequest(long startLon, long startLat, long endLon, long endLat) {
	// calc and print waypoints in a list or shit
	/*
	if () { // if waypoints = 0
		return;
	}
	*/
	hold();
	/*
	for () { // replace condition with number of waypoints in list
		//print each waypoint
		hold(); //wait for ACK
	}
	*/
	cout << 'E' << endl; // end key, replace
}

void serverComm() {
	char input;
	long startLon, startLat, endLon, endLat;
	while (true) {
		cin >> input; // replace with arduino serial input in part 2
		if (input == 'R') { //process request, server enters request handling
			cin >> startLon >> startLat >> endLon >> endLat; //replace with serial input in part 2
			// do djkastra with these points and print waypoints
			processRequest(startLon, startLat, endLon, endLat);
		}
	}	
}

long long manhattan(const Point& pt1, const Point& pt2) {
	return (abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon));
}

void readGraph(string filename, WDigraph &graph, unordered_map<int, Point> points) {
	char graphID, comma;
    int ID, start, end;
	float lat, lon;
	long long convertedLat, convertedLon;
	string name;
	ifstream textIn(filename);
    if (!textIn.good()) {
        cout << "error: digraph text file not found" << endl;
        cout << "usage: ./graph_concepts <digraph_file_name>.txt" << endl;
        exit(EXIT_FAILURE);
    }
    Point point;
    long long distance;
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
            //find points and get manhattan distance

            //distance = manhattan(startPt, endPt);
            //associate edge with that distance
            graph.addEdge(start, end, 100); 
        }
    }
} 
