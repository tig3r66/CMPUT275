#include "include/serial_handling.h"

extern shared_vars shared;

uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment
  
  long long startLat = start.lat, endLat = end.lat;
  long long startLon = start.lon, endLon = end.lon;

  //enum state {NOT_IN_REQUEST, SENDING_REQUEST, PROCESSING_WAYPOINTS}
  Serial.write("R " + startLat + startLon + endLat + endLon);
  int time = millis();
  bool timeout = true;
  do {
    // check input
  } while(millis() - time < 10000);

  if (timeout) {
    return 0;
  }

  


  // for now, nothing is stored
  shared.num_waypoints = 0;

  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
}
