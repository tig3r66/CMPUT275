//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

/*
    Converts between x and y positions on the YEG map to latitude and longitude,
    and vice versa. Please see yeg_rest_coords.cpp for full documentation.
*/

#ifndef _YEG_REST_COORDS_H_
#define _YEG_REST_COORDS_H_

#include <Arduino.h>


// ================================= CONSTANTS =================================
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l


// =========================== FUNCTION DECLARATIONS ===========================
/*
    Description: converts x position on YEG map to longitudinal data for the
    real-world coordinates.
*/
int32_t x_to_lon(int16_t x);

/*
    Description: converts x position on YEG map to latitudinal data for the
    real-world coordinates.
*/
int32_t y_to_lat(int16_t y);

/*
    Description: converts longitudinal data for the real-world YEG locations to
    the x position on the YEG map.
*/
int16_t lon_to_x(int32_t lon);

/*
    Description: converts latitudinal data for the real-world YEG locations to
    the x position on the YEG map.
*/
int16_t lat_to_y(int32_t lat);

#endif
