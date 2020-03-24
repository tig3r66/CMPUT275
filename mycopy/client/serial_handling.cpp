#include "include/serial_handling.h"

extern shared_vars shared;


void consumeSerialInput() {
    while (Serial.available() > 0) Serial.read();
}


bool waitOnSerial(uint8_t nBytes, uint32_t timeout) {
    uint32_t limit = millis() + timeout;
    while (Serial3.available() < nBytes && (timeout < 0 || millis() < limit)) {
        delay(1);
    }
    return Serial.available() >= nBytes;
}


void sendRequest(const lon_lat_32& start, const lon_lat_32& end) {
    int32_t startLat = start.lat, startLon = start.lon;
    int32_t endLat = end.lat, endLon = end.lon;

    Serial.write("R ");
    Serial.print(startLat);
    Serial.write(' ');
    Serial.print(startLon);
    Serial.write(' ');
    Serial.print(endLat);
    Serial.write(' ');
    Serial.print(endLon);
    Serial.print('\n');
}


/*
    Returns:
        -1: failed input.
        0: wait for the next waypoint.
        1: transition to the next state.
*/
int recNumWayAndAck() {
    // reading and validating
    char num = Serial.read();
    char space = Serial.read();
    if (num != 'N' || space != ' ') {
        return -1;
    }

    shared.num_waypoints = static_cast<int32_t>(Serial.read());
    Serial.print('A\n');
    if (shared.num_waypoints > max_waypoints) {
        status_message("too many waypoints requested");
        return 0;
    }

    if (shared.num_waypoints > 0) return 1;
    else if (shared.num_waypoints == 0) return 0;
    else return -1;
}


/*
    Returns:
        -1: failed input.
        0: wait for the next waypoint.
        1: transition to the next state.
*/
int recWayAndAck(uint16_t counter) {
    char way = Serial.read();
    char space = Serial.read();
    if (way != 'W' || space != ' ') return -1;

    int32_t lat = Serial.readStringUntil(' ').toInt();
    int32_t lon = Serial.readStringUntil('\n').toInt();
    shared.waypoints[counter] = {lat, lon};
    if (shared.num_waypoints-1 > counter) Serial.print('A\n');

    if (way == 'W') return 0;
    else if (way == '1') return 1;
    else return -1;
}


uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
    // simple finite-state machine per assignment description
    enum {START, WAIT_ON_WAYPOINTS, REC_WAYPOINTS, END} currState = START;
    uint16_t oneSecond = 1000, tenSeconds = 10000;
    int32_t wayCounter = 0;

    consumeSerialInput();
    while (currState != END) {
        if (currState == START) {
            sendRequest(start, end);
            currState = WAIT_ON_WAYPOINTS;
        }

        else if (currState == WAIT_ON_WAYPOINTS) {
            // waiting for >= 3 bytes and timeout of 10 seconds
            if (waitOnSerial(3, tenSeconds)) {
                int success = recNumWayAndAck();
                if (success == -1) return 0;
                else if (success == 0) currState = END;
                else if (success == 1) currState = REC_WAYPOINTS;
            } else {
                currState = START;
            }
        }

        else if (currState == REC_WAYPOINTS) {
            // waiting for >= 3 bytes and timeout of 1 second
            if (waitOnSerial(3, oneSecond)) {
                int transitionTo = recWayAndAck(wayCounter);
                if (transitionTo == -1
                    || wayCounter > shared.num_waypoints) return 0;
                else if (transitionTo == 0) wayCounter++;
                else if (transitionTo == 1) currState = END;
            } else {
                currState = START;
            }
        }
    }

    // 1 indicates a successful exchange
    return 1;
}
