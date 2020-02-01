#ifndef _MAIN_H_
#define _MAIN_H_

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

// more map constants
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// joystick movement
#define JOY_CENTER 512
#define JOY_DEADZONE 64

// cursor size
#define CURSOR_SIZE 9


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
// setup functions
void setup();
void lcd_setup();

void modeOne();

// tft display-related functions
void redrawCursor(uint16_t);
void processJoystick(uint8_t, uint8_t);
void drawMapPatch(int, int);
void constrainCursor(int*, int*);
void lcdYegDraw(int, int, int, int, int, int);

// retrieving restaurant from memeory
void getRestaurantFast(int, restaurant*);

// converts between x/y map position and lat/lon (and vice versa)
int32_t x_to_lon(int16_t);
int32_t y_to_lat(int16_t);
int16_t lon_to_x(int32_t);
int16_t lat_to_y(int32_t);


#endif