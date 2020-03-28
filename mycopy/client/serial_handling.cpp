#include "include/serial_handling.h"

extern shared_vars shared;


void consumeSerialInput() {
    while (Serial.available() > 0) Serial.read();
}


bool waitOnSerial(uint8_t nBytes, uint32_t timeout) {
    uint32_t limit = millis() + timeout;
    while (Serial.available() < nBytes && (timeout < 0 || millis() < limit)) {
        delay(1);
    }
    return Serial.available() >= nBytes;
}


void sendRequest(const lon_lat_32& start, const lon_lat_32& end) {
    int32_t startLat = start.lat, startLon = start.lon;
    int32_t endLat = end.lat, endLon = end.lon;

    Serial.print("R ");
    Serial.print(startLat);
    Serial.print(' ');
    Serial.print(startLon);
    Serial.print(' ');
    Serial.print(endLat);
    Serial.print(' ');
    Serial.print(endLon);
    Serial.print('\n');
}


/*
    Returns:
        -1: invaild input
        0: vaild input but no path
        1: vaild input
*/
int recNumWayAndAck() {
    // reading and validating
    char num = Serial.read();
    char space = Serial.read();
    if (num != 'N' || space != ' ') {
        return -1;
    }

    shared.num_waypoints = atol(Serial.readStringUntil('\n').c_str());
    Serial.print('A');
    Serial.print('\n');
    if (shared.num_waypoints > max_waypoints) {
        status_message("too many waypoints requested");
        return 0;
    }

    if (shared.num_waypoints > 0) return 1;
    else return 0;
}


/*
    Returns:
        0: input is E
        1: input is vaild
        -1: input is invaild
*/
int recWayAndAck(uint32_t counter) {
    char way = Serial.read();
    char space = Serial.read();
    if ((way != 'W' && way != 'E') || space != ' ') return -1;
    else if (way == 'E') return 0;

    int32_t lat = atol(Serial.readStringUntil(' ').c_str());
    int32_t lon = atol(Serial.readStringUntil('\n').c_str());

    shared.waypoints[counter] = {lat, lon};
    Serial.print('A');
    Serial.print('\n');

    return 1;
}


uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
    // simple finite-state machine per assignment description
    enum {START, WAIT_ON_WAYPOINTS, REC_WAYPOINTS, END} currState = START;
    uint16_t oneSecond = 1000, tenSeconds = 10000;
    uint32_t wayCounter = 0;

    consumeSerialInput();
    while (currState != END) {
        if (currState == START) {
            sendRequest(start, end);
            currState = WAIT_ON_WAYPOINTS;
        }

        else if (currState == WAIT_ON_WAYPOINTS) {
            // waiting for >= 3 bytes and timeout of 10 seconds
            if (waitOnSerial(3, tenSeconds)) {
                int state = recNumWayAndAck();
                if (state == 1) currState = REC_WAYPOINTS;
                else if (state == 0) return 1;
                else return 0;
            } else {
                return 0;
            }
        }

        else if (currState == REC_WAYPOINTS) {
            if (waitOnSerial(2, oneSecond)) {
               int state = recWayAndAck(wayCounter);
               if (state == -1) return 0;
               else if (state == 0) currState = END;
               else wayCounter++;
            } else {
                return 0;
            }
        }
    }

    // 1 indicates a successful exchange
    return 1;
}
