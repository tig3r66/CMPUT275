//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

/*
    Implements an interactive map of Edmonton where the user can move around
    and select restaurants based off of rating and distance to the cursor.
    Please see a1part1.cpp for full function documentation.
*/

#ifndef _A1PART1_H_
#define _A1PART1_H_

#include <MCUFRIEND_kbv.h>
#include "lcd_image.h"


// ================================= CONSTANTS =================================
// joystick pins to connect
#define SD_CS 10
#define JOY_VERT A9
#define JOY_HORIZ A8
#define JOY_SEL 53

// touch screen pins, obtained from the documentaion
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// calibration data for the touch screen, obtained from documentation the
// minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// memory block on SD card where restaurant data is stored
#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// screen and map dimensions
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048
#define YEG_MIDDLE_X ((YEG_SIZE - (MAP_DISP_WIDTH)) >> 1)
#define YEG_MIDDLE_Y ((YEG_SIZE - DISPLAY_HEIGHT) >> 1)
#define PADX 60
#define SHIFTED_BORDER 2039
#define TEXT_PAD 26

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// joystick movement
#define JOY_CENTER 512
#define JOY_DEADZONE 64

// cursor size
#define CURSOR_SIZE 9

// font size
#define FONT_SIZE 15

// number of restuarants listed
#define MAX_LIST 21

// nice blue colour for buttons
#define ONE_BLUE 0x55B8


// ================================== STRUCTS ==================================
// holds restaurant data read from SD card
struct restaurant {
    int32_t lat;
    int32_t lon;
    // from 0 to 10
    uint8_t rating;
    char name[55];
};

// holds index of restaurant and Manhattan distance of restaurant data from
// data from the SD card
struct RestDist {
    uint16_t index;
    uint16_t dist;
};

// temporary cache for reading restaurants from the SD card
struct RestCache {
    restaurant TEMP_BLOCK[8];
    RestDist REST_DIST[NUM_RESTAURANTS];
    uint32_t PREV_BLOCK_NUM = 0;
    uint16_t READ_SIZE = 1066;
};

// stores the cursor's position on the map
struct MapView {
    int cursorX;
    int cursorY;
    int shiftX = 0;
    int shiftY = 0;
};


// ======================= EXTERN VARIABLES AND OBJECTS =======================
// for TFT display-related methods
extern MCUFRIEND_kbv tft;

// for SD card information
extern RestCache cache;

// variables holding SD card read information
extern RestDist REST_DIST[NUM_RESTAURANTS];

// for map redraws
extern lcd_image_t yegImage;

// for map cursor position
extern MapView yeg;


// =========================== FUNCTION DECLARATIONS ===========================
// ============================== SETUP FUNCTIONS ==============================
/*
    Description: initializes important components of Arduino, TFT display, and
    SD card.
*/
void setup();

/*
    Description: intializes TFT display. Portrait display, draws the centre of
    the Edmonton map with the rightmost 60 pixels black, and sets the initial
    cursor position to the middle of the map.
*/
void lcd_setup();


// ================================ MAIN MODES ================================
/*
    Description: allows the user to select a restaurant from the 21 closest
    restaurants to the cursor. Once selected, the map of Edmonton is redrawn
    with the restaurant centered as much as possible on the TFT display.
*/
void modeOne(uint8_t sortMode, uint8_t rating);

/*
    Description: translates joystick inputs to movement of the cursor on the TFT
    screen. Constrains the cursor to within the map portion of the screen, and
    sets variable speed.
*/
void modeZero(uint8_t slow, uint8_t fast);


// =========================== TFT-RELATED FUNCTIONS ===========================
/*
    Description: initial drawing of the names of the closest 21 restaurants to
    the cursor. Highlights the first entry.
*/
void printRestList(uint16_t pageNum, uint16_t selectedRest);


/*
    Description: times quicksort and insertion sort algorithms as they sort the
    restaurants based on Manhattan distance from the cursor position.
*/
void sortTimer(uint8_t sortMode, uint8_t rating);


// ======================== MEMORY RETRIEVAL FUNCTIONS ========================
/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
    card into the global cache struct member TEMP_BLOCK then stores this
    information into a smaller global struct REST_DIST. Reads a block once
    for consecutive restaurants on the same block.
*/
void getRestaurantFast(uint16_t restIndex, uint8_t rating, uint16_t* counter,
    restaurant* restPtr);

/*
    Description: fast implementation of getRestaurant(). Reads a block once for
    consecutive restaurants on the same block. Does not calculate Manhattan
    distance.
*/
void getRestaurant(uint16_t restIndex, restaurant* restPtr);

/*
    Description: reads all restaurant data from the SD card.
*/
void readRestData(uint8_t rating);

// ============================ SCROLLING FUNCTIONS ============================
/*
    Description: processes the joystick movements to move the selection
    highlight either up or down.
*/
void menuProcess(uint16_t* selection, uint16_t* pageNum);

/*
    Description: highlights the selected restaurant and unhighlights the
    previous restaurant.
*/
void redrawText(int current, int prev);

// ======================= TOUCH SCREEN INPUT FUNCTIONS =======================
/*
    Description: processes touches on the TFT display. When the user touches the
    map, the closest restaurants to the cursor are drawn as blue dots.
*/
void processTouchScreen(uint8_t* rating, uint8_t* sortMode);

#endif
