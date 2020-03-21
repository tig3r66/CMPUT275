#include "serial_handling.h"

extern shared_vars shared;

bool waitForResponse(int timeout) {
  int startTime = millis();
  while (millis() - startTime <  timeout) {
    if (Serial.available()) {
      return true;
    }
  }
}
return false;

uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment

  // get lon and lat of end and start point
  long long startLat = start.lat, endLat = end.lat;
  long long startLon = start.lon, endLon = end.lon;

  // sending request
  Serial.write("R " + startLon + " " + startLat + " " +  endLon 
  " " + endLat + "\n");

  // wait for response
  if (!waitForResponse(TIMEOUT_LARGE)) {return 0;}

  if (Serial.read() != 'N') {
    return 0;
  }
  else {
    Serial.read();
    string readline = Serial.readline();
    


  }


  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
}
