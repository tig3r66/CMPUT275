#include <Arduino.h>
#include "include/serial_handling.h"

extern shared_vars shared;

bool waitForResponse(int timeout) {
    int startTime = millis();
    while (millis() - startTime < timeout) {
        if (Serial.available()) {
        return true;
        }
    }
    return false;
}


void writeInt64_t(long long n) {
    //
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
    Serial.print("R ");
    writeInt64_t(startLon);
    Serial.print(" ");
    writeInt64_t(startLat);
    Serial.print(" ");
    writeInt64_t(endLon);
    Serial.write(" ");
    writeInt64_t(endLat);
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();

    // wait for response
    if (!waitForResponse(TIMEOUT_LARGE)) return 0;

    if (Serial.read() != 'N') {
        return 0;
    } else {
        Serial.read();
        int waypointNum = Serial.read();

        if (waypointNum == 0 || waypointNum > max_waypoints) {
            Serial.println("good");
            return 0;
        } else {
            shared.num_waypoints = waypointNum;
            Serial.println("good");
        }
    }


    // 1 indicates a successful exchange, of course you should only output 1
    // in your final solution if the exchange was indeed successful
    // (otherwise, return 0 if the communication failed)
    return 1;
}
