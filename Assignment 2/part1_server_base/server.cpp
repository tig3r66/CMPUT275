#include <iostream>
using namespace std;

void hold() {
	char comm;
	//bool commed = true; //timeout implmentation for part 2

	while (true) { //add conditon regrading time for part 2
		cin >> comm;
		if (comm == 'A') { // wait for ACK key 
			commed = true;
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
	if () { // if waypoints = 0
		return;
	}
	hold();
	for () { // replace condition with number of waypoints in list
		//print each waypoint
		hold(); //wait for ACK
	}
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

int main() {

}