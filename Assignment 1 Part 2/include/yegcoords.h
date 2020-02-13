//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

#ifndef _YEGCOORDS_H_
#define _YEGCOORDS_H_


// ================================= CONSTANTS =================================
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l


// =========================== FUNCTION DECLARATIONS ===========================
// these functions convert between x/y to lat/lon in in the map of Edmonton and
// vice versa
int32_t x_to_lon(int16_t x);
int32_t x_to_lon(int16_t x);
int16_t lon_to_x(int32_t lon);
int16_t lat_to_y(int32_t lat);


#endif
