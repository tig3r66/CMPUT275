#include "include/yegcoords.h"


/*
    Description: converts x position on YEG map to longitudinal data for the
    real-world coordinates.

    Arguments:
        x (int16_t): the x position.
    Returns:
        map(x, 0, MAP_WIDTH, LON_WEST, LON_EAST) (int32_t): the longitude.
*/
int32_t x_to_lon(int16_t x) {
    return map(x, 0, MAP_WIDTH, LON_WEST, LON_EAST);
}


/*
    Description: converts x position on YEG map to latitudinal data for the
    real-world coordinates.

    Arguments:
        y (int16_t): the y position.
    Returns:
        map(y, 0, MAP_WIDTH, LON_WEST, LON_EAST) (int32_t): the latitude.
*/
int32_t y_to_lat(int16_t y) {
    return map(y, 0, MAP_HEIGHT, LAT_NORTH, LAT_SOUTH);
}


/*
    Description: converts longitudinal data for the real-world YEG locations to
    the x position on the YEG map.

    Arguments:
        lon (int32_t): the longitude.
    Returns:
        map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH) (int16_t): the x position.
*/
int16_t lon_to_x(int32_t lon) {
    return map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH);
}


/*
    Description: converts latitudinal data for the real-world YEG locations to
    the x position on the YEG map.

    Arguments:
        lat (int32_t): the latitude.
    Returns:
        map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT) (int16_t): the y position.
*/
int16_t lat_to_y(int32_t lat) {
    return map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT);
}