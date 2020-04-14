//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 2 Part 2: Directions (Client)
//  ========================================

#ifndef __SERIAL_HANDLING_H
#define __SERIAL_HANDLING_H

#include "consts_and_types.h"
#include "map_drawing.h"


/*
    Consumes all Serial content to prevent garbage communication.
*/
void consumeSerialInput();

/*
    Description: waits for a certain number of bytes on Serial or timeout.
*/
bool waitOnSerial(uint8_t nBytes, uint32_t timeout);

/*
    Sends the start and end coordinates from the YEG map.
*/
void sendRequest(const lon_lat_32& start, const lon_lat_32& end);

/*
    Receives the waypoints from the server and acknowledges.
*/
int recNumWayAndAck();

/*
    Receives the waypoints from the server and stores them.
*/
int recWayAndAck(uint32_t counter);

/*
    Print the request with the given endpoints and read the replies
    from the server. Store the number of waypoints and the waypoints themselves
    in the appropriate fields of the extern variable shared.

    If the number of waypoints is > max_waypoints (from consts_and_types.h),
    print some message using status_message() indicating the path is too long,
    delay for 5 seconds, and then resume having treated this like there is no
    path.
*/
uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end);

#endif
