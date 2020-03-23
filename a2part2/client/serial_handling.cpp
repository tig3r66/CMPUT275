#include <Arduino.h>
#include "include/serial_handling.h"

extern shared_vars shared;


// waits for a serial input from server for specified timeout
bool waitForResponse(int timeout) {
    int startTime = millis();
    while (millis() - startTime < timeout) {
        if (Serial.available()) {
        return true;
        }
    }
    return false;
}

// prints a 64 bit int to the serial mon
void writeInt64_t(long long n) {
    // negative
    if (n < 0) {
        Serial.print("-");
        writeInt64_t(-n);
    }
    // printing each digit from most sig to least sig
    else {
        long long test = n;
        int digits = 0;
        while (test > 0) {
            test = test / 10;
            digits++;
        }
        // find a way to get each digit needed, in order of most
        // sig to least sig
        for (int i = digits-1; i >= 0; i--) {
            long long modifier = 0; // should be 10 pow i, use an exp function?
            int digit = n / modifier;
            Serial.print(digit);
            n = n - digit*modifer;
        }
    }
}


uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
    // Currently this does not communicate over the serial port.
    // It just stores a path length of 0. You should make it communicate with
    // the server over Serial using the protocol in the assignment description.

    // TODO: implement the communication protocol from the assignment

    // get lon and lat of end and start point
    long long startLat = start.lat, endLat = end.lat;
    long long startLon = start.lon, endLon = end.lon;

    // sending initial request
    Serial.print("R ");
    writeInt64_t(startLon);
    Serial.print(" ");
    writeInt64_t(startLat);
    Serial.print(" ");
    writeInt64_t(endLon);
    Serial.print(" ");
    writeInt64_t(endLat);
    Serial.println();

    // wait for response, timeout if not in time
    if (!waitForResponse(TIMEOUT_LARGE)) return 0;

    // handle response and get waypoints num
    if (Serial.read() != 'N') {
        return 0;
    } else {
        Serial.read();
        int waypointNum = Serial.read();

        //add seperate prompts for both cases?
        if (waypointNum == 0 || waypointNum > max_waypoints) {
            return 0;
        } else {
            shared.num_waypoints = waypointNum;
        }
    }

    for (int i = 0; i < shared.num_waypoints; i++) {
        // send ACK and wait for response
        Serial.println("A");
        if (!waitForResponse(TIMEOUT_SMALL)) return 0;

        // handle waypoint input here, should be same as before
    }

    Serial.println("A");
    if (!waitForResponse(TIMEOUT_SMALL)) return 0;

    // handle last E


    // 1 indicates a successful exchange, of course you should only output 1
    // in your final solution if the exchange was indeed successful
    // (otherwise, return 0 if the communication failed)
    return 1;
}
