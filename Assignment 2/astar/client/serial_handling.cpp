//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 2: Directions (Client)
//  ========================================

#include "include/serial_handling.h"

extern shared_vars shared;

/*
    Consumes all Serial content to prevent garbage communication.
*/
void consumeSerialInput() {
    while (Serial.available() > 0) Serial.read();
}


/*
    Description: waits for a certain number of bytes on Serial or timeout.

    Arguments:
        nbytes (uint8_t): the number of bytes we want.
        timeout (long): timeout period (ms); specifying a negative number
            turns off timeouts (the function waits indefinitely
            if timeouts are turned off).
    Returns:
        true: if the required number of bytes have arrived.
        false: if timeout occurs.
*/
bool waitOnSerial(uint8_t nBytes, uint32_t timeout) {
    uint32_t limit = millis() + timeout;
    while (Serial.available() < nBytes && (timeout < 0 || millis() < limit)) {
        delay(1);
    }
    return Serial.available() >= nBytes;
}


/*
    Sends the start and end coordinates from the YEG map.

    Arguments:
        start (const lon_lat_32&): the start coordinate.
        end (const lon_lat_32&): the end coordinate.
*/
void sendRequest(const lon_lat_32& start, const lon_lat_32& end) {
    // retrieving the points
    int32_t startLat = start.lat, startLon = start.lon;
    int32_t endLat = end.lat, endLon = end.lon;
    // communicating with server
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
    Receives the waypoints from the server and acknowledges.

    Returns:
        -1: invalid input.
        0: valod input but no path or too many waypoints.
        1: valid input.
*/
int recNumWayAndAck() {
    // reading and validating
    char num = Serial.read();
    char space = Serial.read();
    if (num != 'N' || space != ' ') {
        return -1;
    }
    // casting the Serial String to long
    shared.num_waypoints = atol(Serial.readStringUntil('\n').c_str());
    Serial.print('A');
    Serial.print('\n');
    // >500 waypoints received
    if (shared.num_waypoints > max_waypoints) {
        status_message("too many waypoints requested");
        delay(5);
        return 0;
    }

    if (shared.num_waypoints > 0) return 1;
    else return 0;
}


/*
    Receives the waypoints from the server and stores them.

    Arguments:
        counter (uint32_t): the number of waypoints.
    Returns:
        0: input is "E" (end of the request).
        1: input is valid.
        -1: input is invalid.
*/
int recWayAndAck(uint32_t counter) {
    char way = Serial.read();
    char space = Serial.read();
    if ((way != 'W' && way != 'E') || (space != ' ' && space != '\n')) {
        return -1;
    } else if (way == 'E' && space == '\n') {
        return 0;
    }

    int32_t lat = atol(Serial.readStringUntil(' ').c_str());
    int32_t lon = atol(Serial.readStringUntil('\n').c_str());

    lon_lat_32 waypoint = {lon, lat};
    shared.waypoints[counter] = waypoint;
    Serial.print('A');
    Serial.print('\n');

    return 1;
}


/*
    Implements the client communication protocol and receives and stores
    all waypoints from the server.

    Arguments:
        start (const lon_lat_32&): the start coordinate.
        end (const lon_lat_32&): the end coordinate.
    Returns:
        1: if successful.
        0: otherwise.
*/
uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
    // simple finite-state machine per assignment description
    enum {START, WAIT_ON_WAYPOINTS, REC_WAYPOINTS, END} currState = START;
    uint16_t oneSecond = 1000, tenSeconds = 10000;
    uint32_t wayCounter = 0;

    consumeSerialInput();
    status_message("Receiving path...");
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
                // timeout
                return 0;
            }
        }

        else if (currState == REC_WAYPOINTS) {
            // waiting for >= 2 bytes and timeout of 1 second
            if (waitOnSerial(2, oneSecond)) {
                int state = recWayAndAck(wayCounter);
                if (state == -1) return 0;
                else if (state == 0) return 1;
                else wayCounter++;
            } else {
                // timeout
                return 0;
            }
        }
    }

    // 1 indicates a successful exchange
    return 1;
}
