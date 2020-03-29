//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 2: Directions (Server)
//  ========================================

#ifndef SERVER_H
#define SERVER_H

#include "wdigraph.h"
#include "dijkstra.h"
#include "points.h"
#include "serialport.h"
#include <unordered_map>

#define SCALE 100000
#define TIMEOUT 1000


// STRUCT AND ENUM DECLARATIONS ================================================

/*
    Finite state machine representation of the server communication protocol.
*/
typedef enum {
    LISTENING, PROCESSING_REQUEST, SENDING_WAYPOINTS
} ServerState;

/*
    Contains the two map points to query.
*/
struct TwoPoints {
    long long startLon, startLat;
    long long endLon, endLat;
};

/*
    Contains all variables necessary to send the requested waypoints to the
    Arduino.
*/
struct ServerVars {
    // the two coordinates requested by the Arduino 
    TwoPoints twoCoords;
    // the waypoints between the coordinates
    list<int> path;
    // the IDs of all points and their associated latitude and longitude
    unordered_map<int, Point> points;
    // the search tree built by Dijkstra
    unordered_map<int, PIL> tree;
    // the weighted graph object to read into
    WDigraph graph;
    // the server communication state for the communication protocol
    ServerState state;
    // the Serial communications object
    SerialPort Serial;
};


// FUNCTION DECLARATIONS =======================================================

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
void processWaypoints(ServerVars* Vars);

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

/*
    Returns true if an acknowledgement ('A\n') is returned by the Arduino
    through the Serial port. Otherwise the function returns false if a timeout
    occurs (1 second) or there was invalid input.
*/
bool waitForAck(SerialPort *Serial);

/*
    Clears the search tree and path between two given waypoints.
*/
void reset(unordered_map<int, PIL> &tree, list<int> &path);

/*
    Retrieving the two coordinates requested by the Arduino over Serial.
*/
bool getRequest(ServerVars* Vars);

/*
    Prints the two coordinates requested by the Arduino to stdout.
*/
void printRequest(const ServerVars* const Vars);

/*
    Sending the waypoints between and including the closest points to the
    requested start and end coordinates.
*/
void sendWaypoints(ServerVars* Vars);

#endif
