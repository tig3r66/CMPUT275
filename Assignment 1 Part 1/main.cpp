//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: [redacted]
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 1: Restaurant Finder
//  =======================================

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>

// pins to connect
#define SD_CS 10
#define JOY_VERT A9
#define JOY_HORIZ A8
#define JOY_SEL 53

// screen and map dimensions
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define PADX 60
#define YEG_SIZE 2048

// These constants are for the 2048 by 2048 map.
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l

// memory block on SD card where restaurant data is stored
#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

// joystick movement
#define JOY_CENTER 512
#define JOY_DEADZONE 64
// cursor size and position
#define CURSOR_SIZE 9
int cursorX, cursorY;

// touch screen pins, obtained from the documentaion
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// calibration data for the touch screen, obtained from documentation the
// minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// tft display and sd card objects
MCUFRIEND_kbv tft;
Sd2Card card;

// initialize with 300 ohms of resistance (multimeter measured 300 ohms across
// the plate)
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

struct restaurant {
    int32_t lat;
    int32_t lon;
    // from 0 to 10
    uint8_t rating;
    char name[55];
};

// variables holding SD card read information
restaurant TEMP_BLOCK[8];
unsigned long PREV_BLOCK_NUM = 0;

// display padding
const int PADX = 26;
const int SLOW_RUN_PADY = 16, SLOW_RUN_AVG_PADY = 64;
const int FAST_RUN_PADY = 112, FAST_RUN_AVG_PADY = 160;


// These functions convert between x/y map position and lat/lon
// (and vice versa.)
int32_t x_to_lon(int16_t x) {
	return map(x, 0, MAP_WIDTH , LON_WEST , LON_EAST);
}


int32_t y_to_lat(int16_t y) {
	return map(y, 0, MAP_HEIGHT , LAT_NORTH , LAT_SOUTH);
}


int16_t lon_to_x(int32_t lon) {
	return map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH);
}


int16_t lat_to_y(int32_t lat) {
	return map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT);
}


/*
  Description: initializes important components of Arduino and TFT display.
*/
void setup() {
  	init();
  	Serial.begin(9600);
  	pinMode(JOY_SEL, INPUT_PULLUP);

  	uint16_t ID = tft.readID();
  	Serial.print("ID = 0x");
  	Serial.println(ID, HEX);
  	if (ID == 0xD3D3) ID = 0x9481;
  	tft.begin(ID);

  	Serial.print("Initializing SD card...");
  	if (!SD.begin(SD_CS)) {
  		Serial.println("failed! Is it inserted properly?");
  		while (true) {}
  	}
  	Serial.println("OK!");
}


/*
  Description: intializes TFT display. Portrait display, draws the centre of the
    Edmonton map with the rightmost 60 pixels black, and sets the initial cursor
    position to the middle of the map.
*/
void lcd_setup() {
  	tft.setRotation(1);
  	tft.fillScreen(TFT_BLACK);
  	// drawing map
  	int yegMiddleX = (YEG_SIZE - (DISPLAY_WIDTH - PADX)) >> 1;
  	int yegMiddleY = (YEG_SIZE - DISPLAY_HEIGHT) >> 1;
  	lcd_image_draw(&yegImage, &tft, yegMiddleX, yegMiddleY, 0, 0,
  		DISPLAY_WIDTH - PADX, DISPLAY_HEIGHT);
  	// setting cursor to middle of YEG map
  	cursorX = (DISPLAY_WIDTH - PADX) >> 1;
  	cursorY = DISPLAY_HEIGHT >> 1;
  	redrawCursor(TFT_RED);
}

int main() {
	setup();
	lcd_setup();

    // main code here

    Serial.end();
    return 0;
}
