//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 2: Directions (Server)
//  ========================================

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
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
    Clears the search tree and path between two given waypoints.

    Parameters:
        tree (unordered_map<int, PIL>&): the search tree to obtain the path.
        path (list<int>&): the path of waypoints to build.
*/
void reset(unordered_map<int, PIL> &tree, list<int> &path) {
    tree.clear();
    path.clear();
}


/*
    Returns true if an acknowledgement ('A\n') is returned by the Arduino
    through the Serial port. Otherwise the function returns false if a timeout
    occurs (1 second) or there was invalid input.

    Parameters:
        Serial (SerialPort*): the instantiated SerialPort object.
    Returns:
        true: "A\n" returned by Arduino within a timeout of 1 second.
        false: invalid input or timeout.
*/
bool waitForAck(SerialPort *Serial) {
    string readline = (*Serial).readline(TIMEOUT);
    if (readline == "A\n") {
        return true;
    } else {
        return false;
    }
}


/*
    Retrieving the two coordinates requested by the Arduino over Serial.

    Parameters:
        Vars (ServerVars*): the struct containing a struct that stores the
            coordinates requested from the Arduino.
*/
bool getRequest(ServerVars* Vars) {
    string request = Vars->Serial.readline(1000);
    stringstream ss(request);
    string temp;
    ss >> temp;
    // checking whether the request was valid
    if (temp == "R") {
        // retrieving the coordinates
        ss >> Vars->twoCoords.startLat >> Vars->twoCoords.startLon
            >> Vars->twoCoords.endLat >> Vars->twoCoords.endLon;
        // moving to the next state
        Vars->state = PROCESSING_REQUEST;
    }
    ss.flush();
    return (Vars->state == PROCESSING_REQUEST);
}


/*
    Prints the two coordinates requested by the Arduino to stdout.

    Parameters:
        Vars (const ServerVars* const): the struct containing a struct that
            stores the coordinates requested from the Arduino.
*/
void printRequest(const ServerVars* const Vars) {
    cout << "START REQUEST\nR ";
    cout << Vars->twoCoords.startLat << ' ' << Vars->twoCoords.startLon;
    cout << ' ' << Vars->twoCoords.endLat << ' '
        << Vars->twoCoords.endLon << endl;
}


/*
    Helper function that processes valid requests sent to the server via stdin
    and stdout. This function will print waypoints to the command-line from the
    requested start and end points.

    Parameters:
        Vars (ServerVars*): struct that contains the graph to search and
            the path to build.
*/
void processWaypoints(ServerVars* Vars) {
    // querying the pont and building the search tree
    Point start = {Vars->twoCoords.startLat, Vars->twoCoords.startLon};
    Point end = {Vars->twoCoords.endLat, Vars->twoCoords.endLon};
    int startIndex = findClosestPointOnMap(start, Vars->points);
    int endIndex = findClosestPointOnMap(end, Vars->points);
    dijkstra(Vars->graph, startIndex, Vars->tree);

    // retrieving the path from the search tree
    if (!findShortestPath(Vars->tree, Vars->path, startIndex, endIndex)) {
        Vars->Serial.writeline("N 0\n");
        cout << "PATH NOT FOUND" << endl;
        reset(Vars->tree, Vars->path);
        Vars->state = LISTENING;
    } else {
        // send path length
        cout << "N " << Vars->path.size() << endl;
        string length = to_string(Vars->path.size());
        // communicating with the Arduino
        Vars->Serial.writeline("N ");
        Vars->Serial.writeline(length);
        Vars->Serial.writeline("\n");
        // wait for ack, if ack recivced in time (1 second), move to next state
        if (!waitForAck(&(Vars->Serial))) {
            reset(Vars->tree, Vars->path);
            Vars->state = LISTENING;
        } else {
            Vars->state = SENDING_WAYPOINTS;
        }
    }
}


/*
    Sending the waypoints between and including the closest points to the
    requested start and end coordinates.

    Parameters:
        Vars (ServerVars*): struct which contains the path to query and
            send to the Arduino.
    Notes:
        This function writes all waypoints to stdout.
*/
void sendWaypoints(ServerVars* Vars) {
    long numWaypoints = Vars->path.size();
    for (int i = 0; i < numWaypoints; i++) {
        // retrieving the coordinate information for a given point
        int waypointID = Vars->path.front();
        Vars->path.pop_front();
        Point waypoint = Vars->points[waypointID];
        string lat = to_string(waypoint.lat);
        string lon = to_string(waypoint.lon);
        cout << "W " << lat << ' ' << lon << endl;

        // sending waypoint
        Vars->Serial.writeline("W ");
        Vars->Serial.writeline(lat);
        Vars->Serial.writeline(" ");
        Vars->Serial.writeline(lon);
        Vars->Serial.writeline("\n");

        // timeout
        if (!waitForAck(&(Vars->Serial))) {
            Vars->state = LISTENING;
            reset(Vars->tree, Vars->path);
            break;
        }
    }

    // end of request
    cout << "E" << endl;
    Vars->Serial.writeline("E\n");
    reset(Vars->tree, Vars->path);
    Vars->state = LISTENING;
}


int main() {
    SerialPort Serial("/dev/ttyACM0");
    ServerVars mainVars;
    mainVars.Serial = Serial;
    mainVars.state = LISTENING;

    // building the weighted directed graph from the yegGraph input file
    const char* yegGraph = "edmonton-roads-2.0.1.txt";
    readGraph(yegGraph, mainVars.graph, mainVars.points);

    // finite state machine for server
    while (true) {
        if (mainVars.state == LISTENING) {
            // LISTENING
            bool isSuccess = getRequest(&mainVars);
            if (isSuccess) printRequest(&mainVars);
        } else if (mainVars.state == PROCESSING_REQUEST) {
            // DIJKSTRA TO BUILD PATH
            processWaypoints(&mainVars);
        } else if (mainVars.state == SENDING_WAYPOINTS) {
            // SENDING PATH AND ENDING COMMUNICATIONS
            sendWaypoints(&mainVars);
            cout << "END REQUEST" << endl;
        }
    }

	return 0;
}
