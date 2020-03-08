#ifndef _GET_RESTAURANT_H
#define _GET_RESTAURANT_H

#include "rest_struct.h"

#define SD_CS 10

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

// touch screen pins, obtained from the documentaion
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// memory block on SD card where restaurant data is stored
#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation the
// minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// display padding
const int PADX = 26;
const int SLOW_RUN_PADY = 16, SLOW_RUN_AVG_PADY = 64;
const int FAST_RUN_PADY = 112, FAST_RUN_AVG_PADY = 160;

// function declarations
void setup();
void printRunLabels();
void drawOptionButtons();
void printWord(const char*);
void eraseRuns(uint8_t, uint8_t);
void printSlowRun(unsigned int&, unsigned int&);
void printFastRun(unsigned int&, unsigned int&);
void getRestaurant(int, restaurant*);
void getRestaurantFast(int, restaurant*);
void processTouchScreen(unsigned int&, unsigned int&, unsigned int&,
    unsigned int&);
unsigned long timeSlowRun(restaurant*);
unsigned long getRunningTime(void (*)(int, restaurant*), restaurant*);
unsigned long runAvg(unsigned int, unsigned int*, unsigned int*);

#endif
