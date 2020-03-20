#include "serial_handling.h"

extern shared_vars shared;

int waitForwaypoint(long long &lon, long long &lat) {
  int starttime = millis();
  bool state = false;
  string temp;
  
  while (millis() - starttime < TIMEOUT_NORMAL) {
    if (Serial.available()) {
      string inputLine = Serial.readline();
      stringstream ss(inputLine);
      ss >> temp;
      if (temp == "N") {
        if (!(ss >> lat)) {
          return 0;
        }
        if (!(ss >> lon)) {
          return 0;
        }
        if (!(ss.eof())) {
          return 0;
        }

      }
      else if (temp == "E") {
        return 1;
      }
      else {
        return 0;
      }
    }
  }
  return 0;
}

uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment

  // get lon and lat of end and start point
  long long startLat = start.lat, endLat = end.lat;
  long long startLon = start.lon, endLon = end.lon;

  // sending request
  Serial.write("R " + startLon + startLat + endLon + endLat);

  int startTime = millis();
  bool timeout = true;
  string temp;
  while (millis() - startTime < TIMEOUT_LARGE) {
    if (Serial.available()) {
      string waypointsAmount = Serial.readline();
      stringstream ss(waypointsAmount);
      ss >> temp
      if (temp == 'N') {
        if (!(ss >> shared.waypoints_num)) {
          return 0;
        }
        if (!ss.eof()) {
          return 0;
        }
      }
      else {
        return 0;
      }
    }
  }

  ss.flush();

  if (timeout) {
    return 0;
  }

  if (shared.waypoints_num >= 0 && shared.waypoints_num > max_waypoints) {
    Serial.println("Path invaild");
  }

  int i = 0;
  while (true) {
    Serial.write('A\n');
    long long lat, lon;
    if (waitForwaypoint(lon, lat) == 0) {
      return 0;
    }
    else if (waitForwaypoint(lon, lat) == 1) {
      break;
    }
    else {
      lon_lat_32 waypoint = {lon, lat};
      shared.waypoints[i] = waypoint;
      i++;
    }
  }

  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  return 1;
}
