//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>
#include <stdlib.h>

#include "include/lcd_image.h"
#include "include/yeg_rest_coords.h"
#include "include/yeg_sort.h"
#include "include/yeg_draw.h"
#include "include/a1part2.h"


MCUFRIEND_kbv tft;
Sd2Card card;
// initialize with 300 ohms of resistance
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

// variables holding SD card read information
restaurant TEMP_BLOCK[8];
RestDist REST_DIST[NUM_RESTAURANTS];
uint32_t PREV_BLOCK_NUM = 0;

// cursor position variable
int cursorX, cursorY;
// storing overall map shifts for total map redraws
int shiftX = 0, shiftY = 0;


/*
    Description: moves a cursor across a map of Edmonton using a joystick on a
    TFT display controlled by an Arduino. If the user touches the screen, then
    the closest restaurants are drawn as dots. If the user pushes the joystick
    button, then a list of the closest restaurants are drawn. The user can
    select the highlighted restaurant from the list by pushing the joystick
    button, and the map will be redrawn with the restaurant centered as much
    as possible on the TFT display.
*/
int main() {
    setup();
    lcd_setup();

    // for the display mode
    uint8_t MODE = 0;
    while (true) {
        if (MODE == 0) {
            // joystick button pressed
            if (!digitalRead(JOY_SEL)) MODE = 1;
            // if user touches screen, draw closest restaurants
            processTouchScreen();
            // min and max cursor speeds are 0 and CURSOR_SIZE pixels/cycle
            modeZero(0, CURSOR_SIZE);
        } else if (MODE == 1) {
            tft.fillScreen(TFT_BLACK);
            // loops until the joystick button is pressed
            modeOne();
            MODE = 0;
        }
    }

    Serial.end();
    return 0;
}


/*
    Description: initializes important components of Arduino, TFT display, and
    SD card.
*/
void setup() {
    init();
    Serial.begin(9600);
    pinMode(JOY_SEL, INPUT_PULLUP);

    uint16_t ID = tft.readID();
    if (ID == 0xD3D3) ID = 0x9481;
    tft.begin(ID);

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed!");
        while (true) {}
    }

    // SD card initialization for raw reads
    Serial.print("\nInitializing SPI communication...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        Serial.println("failed!");
        while (true) {}
    } else {
        Serial.println("OK!");
    }
}


/*
    Description: intializes TFT display. Portrait display, draws the centre of
    the Edmonton map with the rightmost 60 pixels black, and sets the initial
    cursor position to the middle of the map.
*/
void lcd_setup() {
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextWrap(false);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    lcdYegDraw(YEG_MIDDLE_X, YEG_MIDDLE_Y, 0, 0, MAP_DISP_WIDTH,
        DISPLAY_HEIGHT);
    // setting cursor to middle of YEG map
    cursorX = (MAP_DISP_WIDTH) >> 1;
    cursorY = DISPLAY_HEIGHT >> 1;
    redrawCursor(TFT_RED);
}


/*
    Description: translates joystick inputs to movement of the cursor on the TFT
    screen. Constrains the cursor to within the map portion of the screen, and
    sets variable speed.

    Arguments:
        slow (uint8_t): the minimum cursor speed (pixels/loop).
        fast (uint8_t): the maximum cursor speed (pixels/loop).
*/
void modeZero(uint8_t slow, uint8_t fast) {
    uint16_t xVal = analogRead(JOY_HORIZ);
    uint16_t yVal = analogRead(JOY_VERT);
    uint16_t EPSILON_POS = JOY_CENTER + JOY_DEADZONE;
    uint16_t EPSILON_NEG = JOY_CENTER - JOY_DEADZONE;
    uint16_t MAX_STICK = (1 << 10) - 1;
    // store current cursor position
    int cursorX0 = cursorX, cursorY0 = cursorY;

    // x movement
    if (xVal > EPSILON_POS) {
        cursorX -= map(xVal, EPSILON_POS, MAX_STICK, slow, fast);
    } else if (xVal < EPSILON_NEG) {
        cursorX += map(xVal, 0, EPSILON_NEG, fast, slow);
    }

    // y movement
    if (yVal < EPSILON_NEG) {
        cursorY -= map(yVal, 0, EPSILON_NEG, fast, slow);
    } else if (yVal > EPSILON_POS) {
        cursorY += map(yVal, EPSILON_POS, MAX_STICK, slow, fast);
    }

    if (cursorX0 != cursorX || cursorY != cursorY0) {
        // constrain cursor and display within the map then redraw the map patch
        constrainCursor(&cursorX, &cursorY);
        constrainMap(&shiftX, &shiftY);
        drawMapPatch(cursorX0, cursorY0);
        // screen (not image) position of cursor on the YEG map
        int icol = YEG_MIDDLE_X + cursorX + shiftX - (CURSOR_SIZE >> 1);
        int irow = YEG_MIDDLE_Y + cursorY + shiftY - (CURSOR_SIZE >> 1);

        // PAD accounts for integer division by 2 (i.e., cursor has odd
        // sidelength)
        uint8_t PAD = 0;
        if (CURSOR_SIZE & 1) PAD = 1;

        // draws once cursor reaches border (except at physical map border)
        if ((icol != 0 && irow != 0 && icol != (YEG_SIZE + PAD)
            && irow != YEG_SIZE + PAD)
            || (icol != 0 && irow == 0) || (icol == 0 && irow != 0)
            || (icol != SHIFTED_BORDER && irow == SHIFTED_BORDER)
            || (icol != SHIFTED_BORDER && irow == SHIFTED_BORDER)
        ) {
            redrawMap();
        }
        redrawCursor(TFT_RED);
    }
}


/*
    Description: allows the user to select a restaurant from the 21 closest
    restaurants to the cursor. Once selected, the map of Edmonton is redrawn
    with the restaurant centered as much as possible on the TFT display.
*/
void modeOne() {
    readRestData();
    // insertionSort(REST_DIST, NUM_RESTAURANTS);
    quickSort(REST_DIST, 0, NUM_RESTAURANTS);
    printRestList();

    // processing menu
    uint16_t selection = 0;
    while (true) {
        menuProcess(&selection);
        if (!(digitalRead(JOY_SEL))) {
            tft.fillRect(MAP_DISP_WIDTH, 0, PADX, MAP_DISP_HEIGHT, TFT_BLACK);
            redrawOverRest(selection);
            return;
        }
    }
}


/*
    Description: processes touches on the TFT display. When the user touches the
    map, the closest restaurants to the cursor are drawn as blue dots.
*/
void processTouchScreen() {
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    int screen_x = map(touch.y, TS_MINX, TS_MAXX, tft.width() - 1, 0);
    if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE
        || screen_x > MAP_DISP_WIDTH) {
            return;
    } else if (screen_x < MAP_DISP_WIDTH) {
        readRestData();
        drawCloseRests(3, MAP_DISP_WIDTH + MAP_DISP_HEIGHT, TFT_BLUE);
    }
}


/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
    card into the global restaurant struct TEMP_BLOCK then stores this
    information into a smaller global struct REST_DIST. Reads a block once
    for consecutive restaurants on the same block.

    Arguments:
        restIndex (uint16_t): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurantFast(uint16_t restIndex, restaurant* restPtr) {
    uint32_t blockNum = REST_START_BLOCK + restIndex / 8;
    if (blockNum != PREV_BLOCK_NUM) {
        while (!card.readBlock(blockNum,
            reinterpret_cast<uint8_t*>(TEMP_BLOCK))) {
                Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = TEMP_BLOCK[restIndex % 8];
    PREV_BLOCK_NUM = blockNum;

    // storing TEMP_BLOCK information in a smaller global struct
    REST_DIST[restIndex].index = restIndex;

    // image (not screen) position of cursor on the YEG map
    int icol = YEG_MIDDLE_X + cursorX + shiftX;
    int irow = YEG_MIDDLE_Y + cursorY + shiftY;

    // calculating Manhattan distance and storing into global REST_DIST struct
    int manDist = abs(icol - lon_to_x(restPtr->lon))
        + abs(irow - lat_to_y(restPtr->lat));
    REST_DIST[restIndex].dist = manDist;
}


/*
    Description: fast implementation of getRestaurant(). Reads a block once for
    consecutive restaurants on the same block. Does not calculate Manhattan
    distance.

    Arguments:
        restIndex (uint16_t): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurant(uint16_t restIndex, restaurant* restPtr) {
    uint32_t blockNum = REST_START_BLOCK + restIndex / 8;
    if (blockNum != PREV_BLOCK_NUM) {
        while (!card.readBlock(blockNum,
            reinterpret_cast<uint8_t*>(TEMP_BLOCK))) {
                Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = TEMP_BLOCK[restIndex % 8];
    PREV_BLOCK_NUM = blockNum;
}


/*
    Description: reads all restaurant data from the SD card.
*/
void readRestData() {
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        getRestaurantFast(i, TEMP_BLOCK);
    }
}


/*
    Description: draws dots over restaurants that are closest to the cursor.

    Arguments:
        radius (int): the desired radius of the drawn dot.
        distance (int): the restaurants at a desired distance from the cursor.
        colour (uint16_t): the colour of the dot drawn.
*/
void drawCloseRests(uint8_t radius, uint16_t distance, uint16_t colour) {
    int sidePad = 3;
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        restaurant tempRest;
        getRestaurant(REST_DIST[i].index, &tempRest);
        int16_t scol = lon_to_x(tempRest.lon) - YEG_MIDDLE_X - shiftX;
        int16_t srow = lat_to_y(tempRest.lat) - YEG_MIDDLE_Y - shiftY;

        if (scol < MAP_DISP_WIDTH - sidePad && srow < MAP_DISP_HEIGHT - sidePad
            && srow >= 0 && scol >= 0 && REST_DIST[i].dist <= distance) {
                tft.fillCircle(scol, srow, radius, colour);
        }
    }
}


/*
    Description: initial drawing of the names of the closest 21 restaurants to
    the cursor. Highlights the first entry.
*/
void printRestList() {
    tft.setCursor(0, 0);
    for (uint8_t i = 0; i < MAX_LIST; i++) {
        restaurant rest;
        getRestaurantFast(REST_DIST[i].index, &rest);
        if (i == 0) {
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            tft.print(rest.name);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.print(rest.name);
        }
        tft.setCursor(0, (i+1) * FONT_SIZE);
    }
}


/*
    Description: processes the joystick movements to move the selection
    highlight either up or down.

    Arguments:
        *selection (uint16_t): pointer to the selected restaurant's index.
*/
void menuProcess(uint16_t* selection) {
    uint16_t joyY = analogRead(JOY_VERT);
    if (joyY < (JOY_CENTER - JOY_DEADZONE)) {
        // scroll one up
        if (*selection > 0) {
            (*selection)--;
            redrawText(*selection, *selection + 1);
        }
    } else if (joyY > (JOY_CENTER + JOY_DEADZONE)) {
        // scroll one down
        if (*selection < MAX_LIST - 1) {
            (*selection)++;
            redrawText(*selection, *selection - 1);
        }
    }
    // for better (less sensitive) scrolling user experience
    delay(25);
}


/*
    Description: highlights the selected restaurant and unhighlights the
    previous restaurant.

    Arguments:
        current (int): the currently selected restaurant.
        prev (int): the previously selected restaurant.
*/
void redrawText(int current, int prev) {
    // drawing over prev and reprinting it
    restaurant tempRest;
    getRestaurantFast(REST_DIST[prev].index, &tempRest);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, prev * FONT_SIZE);
    tft.print(tempRest.name);

    // get current name and print it
    getRestaurantFast(REST_DIST[current].index, &tempRest);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setCursor(0, current * FONT_SIZE);
    tft.print(tempRest.name);
}
