//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

#ifndef _A1PART1_H_
#define _A1PART1_H_

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


// MAIN MODES
/*
    Description: allows the user to select a restaurant from the 21 closest
    restaurants to the cursor. Once selected, the map of Edmonton is redrawn
    with the restaurant centered as much as possible on the TFT display.
*/
void modeOne(int);

/*
    Description: translates joystick inputs to movement of the cursor on the TFT
    screen. Constrains the cursor to within the map portion of the screen, and
    sets variable speed.

    Arguments:
        slow (uint8_t): the minimum cursor speed (pixels/loop).
        fast (uint8_t): the maximum cursor speed (pixels/loop).
*/
void modeZero(uint8_t, uint8_t);


// =========================== TFT-RELATED FUNCTIONS ===========================
/*
    Description: draws a square cursor at the current cursor position.

    Arguments:
        colour (uint16_t): the colour of the cursor.
*/
void redrawCursor(uint16_t);

/*
    Description: redraws the shifted map of Edmonton depending on the cursor
    nudge direction.

    Arguments:
        cursorX0 (uint16_t): the original cursor's X position.
        cursorY0 (uint16_t): the original cursor's Y position.
*/
void redrawMap();

/*
    Description: draws a small map patch from the old cursor position up to the
    new cursor position (i.e., only redraws a rectangle if the cursor did not
    move too much).

    Arguments:
        cursorX0 (uint16_t): the original cursor's X position.
        cursorY0 (uint16_t): the original cursor's Y position.
*/
void drawMapPatch(int, int);

/*
    Description: helper function for drawMapPatch(). Essentially the
    lcd_image_draw() except it assumes the image's memory address is &yegImage
    and the TFT object's memory address is &tft.

    Arguments:
        icol (int): image column.
        irow (int): image row.
        scol (int): screen column.
        srow (int): screen row.
        width (int): width of the patch to draw.
        height (int): height of the patch to draw.
*/
void lcdYegDraw(int, int, int, int, int, int);

/*
    Description: constrains the cursor within the boundaries of the image
    displayed on the TFT display. This function assumes the cursor is a square.

    Arguments:
        cursorX (int*): pointer to the cursor's X position.
        cursorY (int*): pointer to the cursor's Y position.
*/
void constrainCursor(int*, int*);

/*
    Description: constrains the global shift in X and Y position to within the
    physical boundaries of the YEG map.

    Arguments:
        shiftX (int*): pointer to the X shift.
        shiftY (int*): pointer to the Y shift.
*/
void constrainMap(int*, int*);

/*
    Description: helper function for redrawMap() which aids with map boundary
    clamping and map shifting.

    Arguments:
        shiftLen (int*): the X or Y shift to store for future map shifts.
        mainDir (const char*): user passes in the parameter of movement.
*/
void helperMove(int*, const char*);

/*
    Description: initial drawing of the names of the closest 21 restaurants to
    the cursor. Highlights the first entry.
*/
void printRestList(int, int, int, int[]);

/*
    Description: redraws the map of Edmonton centered as much as possible over
    the selected restaurant.

    Arguments:
        selection (uint16_t): the index of the selected restaurant.
*/
void redrawOverRest(uint16_t);


// ======================== MEMORY RETRIEVAL FUNCTIONS ========================
/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
    card into the global restaurant struct TEMP_BLOCK then stores this
    information into a smaller global struct REST_DIST. Reads a block once
    for consecutive restaurants on the same block.

    Arguments:
        restIndex (uint16_t): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurantFast(uint16_t, restaurant*);

/*
    Description: fast implementation of getRestaurant(). Reads a block once for
    consecutive restaurants on the same block.

    Arguments:
        restIndex (uint16_t): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurant(uint16_t, restaurant*);

/*
    Description: reads all restaurant data from the SD card.
*/
void readRestData();


// ============================ SORTING ALGORITHMS ============================
/*
    Description: implementation of insertion sort.

    Arguments:
        array[] (int): pointer to an array.
        n (int): the number of items in the array.
    Notes:
        Need to change this function so it sorts based on the RestDist struct.
*/
void insertionSort(RestDist[], int);


// ============================ SCROLLING FUNCTIONS ============================
/*
    Description: processes the joystick movements to move the selection
    highlight either up or down.

    Arguments:
        *selection (uint16_t): pointre to the selected restaurant's index.
*/
void menuProcess(uint16_t*, int*, int[], int*, int);

/*
    Description: highlights the selected restaurant and unhighlights the
    previous restaurant.

    Arguments:
        current (int): the currently selected restaurant.
        prev (int): the previously selected restaurant.
*/
void redrawText(int, int);

// ======================= TOUCH SCREEN INPUT FUNCTIONS =======================
/*
    Description: processes touches on the TFT display. When the user touches the
    map, the closest restaurants to the cursor are drawn as blue dots.
*/
void processTouchScreen(int);

/*
    Description: draws dots over restaurants that are closest to the cursor.

    Arguments:
        radius (int): the desired radius of the drawn dot.
        distance (int): the restaurants at a desired distance from the cursor.
        colour (uint16_t): the colour of the dot drawn.
*/
void drawCloseRests(uint8_t, uint16_t, uint16_t, int);


// =============================== TEMPLATE CODE ===============================
/*
    Description: swaps two items x and y.

    Arguments:
        x (T&): the first item to swap.
        y (T&): the second item to swap.
*/
template <typename T>
void custom_swap(T &x, T &y) {
    T temp = x;
    x = y;
    y = temp;
}

#endif
