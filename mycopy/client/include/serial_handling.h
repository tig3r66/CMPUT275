#ifndef __SERIAL_HANDLING_H
#define __SERIAL_HANDLING_H

#include "consts_and_types.h"
#include "map_drawing.h"


void consumeSerialInput();

bool waitOnSerial(uint8_t nBytes, uint32_t timeout);

void sendRequest(const lon_lat_32& start, const lon_lat_32& end);

int recNumWayAndAck();

int recWayAndAck(uint16_t counter);

/*
    Print the request with the given endpoints and read the replies
    from the server. Store the number of waypoints and the waypoints themselves
    in the appropriate fields of the extern variable shared

    If the number of waypoints is > max_waypoints (from consts_and_types.h),
    print some message using status_message() indicating the path is too long,
    delay for 5 seconds, and then resume having treated this like there is no
    path.
*/
uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end);

#endif
