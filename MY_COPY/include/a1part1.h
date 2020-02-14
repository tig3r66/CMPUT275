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
extern MCUFRIEND_kbv tft;


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

//number of restuarants listed
#define MAX_LIST 21

// cursor position variable
extern int cursorX, cursorY;
// storing overall map shifts for total map redraws
extern int shiftX, shiftY;

// variables holding SD card read information
extern restaurant TEMP_BLOCK[8];
extern RestDist REST_DIST[NUM_RESTAURANTS];
extern uint32_t PREV_BLOCK_NUM;


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
void modeOne();

/*
    Description: translates joystick inputs to movement of the cursor on the TFT
    screen. Constrains the cursor to within the map portion of the screen, and
    sets variable speed.
*/
void modeZero(uint8_t slow, uint8_t fast);


// =========================== TFT-RELATED FUNCTIONS ===========================
/*
    Description: helper function for drawMapPatch(). Essentially the
    lcd_image_draw() except it assumes the image's memory address is &yegImage
    and the TFT object's memory address is &tft.
*/
void lcdYegDraw(int icol, int irow, int scol, int srow, int width, int height);

/*
    Description: initial drawing of the names of the closest 21 restaurants to
    the cursor. Highlights the first entry.
*/
void printRestList();


// ======================== MEMORY RETRIEVAL FUNCTIONS ========================
/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
    card into the global restaurant struct TEMP_BLOCK then stores this
    information into a smaller global struct REST_DIST. Reads a block once
    for consecutive restaurants on the same block.
*/
void getRestaurantFast(uint16_t restIndex, restaurant* restPtr);

/*
    Description: fast implementation of getRestaurant(). Reads a block once for
    consecutive restaurants on the same block. Does not calculate Manhattan
    distance.
*/
void getRestaurant(uint16_t restIndex, restaurant* restPtr);

/*
    Description: reads all restaurant data from the SD card.
*/
void readRestData();

// ============================ SCROLLING FUNCTIONS ============================
/*
    Description: processes the joystick movements to move the selection
    highlight either up or down.
*/
void menuProcess(uint16_t* selection);

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
void processTouchScreen();

/*
    Description: draws dots over restaurants that are closest to the cursor.
*/
void drawCloseRests(uint8_t radius, uint16_t distance, uint16_t colour);

#endif
