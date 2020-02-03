//  ========================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Weekly Exercise 1: Display and Joystick
//  ========================================

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>
#include <stdlib.h>
#include "../include/lcd_image.h"
#include "../include/main.h"

MCUFRIEND_kbv tft;
Sd2Card card;
// initialize with 300 ohms of resistance
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };


// variables holding SD card read information
restaurant TEMP_BLOCK[8];
RestDist REST_DIST[NUM_RESTAURANTS];
unsigned long PREV_BLOCK_NUM = 0;

// cursor position variable
int cursorX, cursorY;

// storing overall map shifts for total map redraws
int shiftX = 0, shiftY = 0;

// for the display mode
uint8_t MODE = 0;


/*
    Description: moves a cursor across a partial map of Edmonton using a
    joystick on a TFT display controlled by an Arduino.
*/
int main() {
    setup();
    lcd_setup();

    while (true) {
        if (MODE == 0) {
            // joystick button pressed
            if (!digitalRead(JOY_SEL)) MODE = 1;
            // min and max cursor speeds are 0 and CURSOR_SIZE pixels/cycle
            modeZero(0, CURSOR_SIZE);
        } else if (MODE == 1) {
            tft.fillRect(0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT, TFT_BLACK);
            if (!digitalRead(JOY_SEL)) {
                MODE = 0;
                // redraw map over the selected restaurant
            };

            // errors in modeOne()
            // modeOne();
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

    // SD card initialization for raw reads
    Serial.print("Initializing SPI communication for raw reads...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        Serial.println("failed! Is the card inserted properly?");
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
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // drawing map
    int yegMiddleX = (YEG_SIZE - (MAP_DISP_WIDTH)) >> 1;
    int yegMiddleY = (YEG_SIZE - DISPLAY_HEIGHT) >> 1;
    lcdYegDraw(yegMiddleX, yegMiddleY, 0, 0, MAP_DISP_WIDTH, DISPLAY_HEIGHT);
    // setting cursor to middle of YEG map
    cursorX = (MAP_DISP_WIDTH) >> 1;
    cursorY = DISPLAY_HEIGHT >> 1;
    redrawCursor(TFT_RED);
}


void modeOne() {
    // // joystick pressed
    // if (!digitalRead(53)) {
    //     MODE = 0;
    // }

    // // to implement
    // listResturants();
    // int currentSelect = 0;
    // int restX, restY; // use for dot
    // bool hold = true;
    // while (hold) {
    //     processScroll(currentSelect, hold, restX, restY);
    // }
    // // redrawing map patch around selected resturant
    // lcdYegDraw(restX-(MAP_DISP_WIDTH >> 1), 
    //     restY-(DISPLAY_HEIGHT >> 1),0, 0 MAP_DISP_WIDTH, DISPLAY_HEIGHT);
    // set cursor to pos restX, restY, remeber to convert!
}


void restDistFill() {
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        restaurant rest;
        getRestaurantFast(i, &rest);
        int xDiff = abs(cursorX - rest.lat); //fix
        int yDiff = abs(cursorY - rest.lon); //fix
        RestDist rd = {i, xDiff + yDiff};
    }
    insertion_sort(REST_DIST, NUM_RESTAURANTS); // sorting by dist to location
}


void listResturants() {
    tft.fillScreen(0);
    tft.setCursor(0, 0);
    tft.setTextWrap(false);

    for (int i = 0; i < 21; i++) {
        char restName[100];
        getRestaurantName(restName, i);
        if (i == 0) {
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
        } else {
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        tft.println(restName);
    }
    tft.println();
}


void processScroll(int &currentSelect, bool &hold, int& restX, int& restY) {
    uint16_t yVal = analogRead(JOY_VERT);
    if (yVal > JOY_DEADZONE + JOY_CENTER) {
        redrawText(currentSelect, currentSelect+1);
        currentSelect++;
    } else if (yVal < JOY_CENTER - JOY_DEADZONE) {
        redrawText(currentSelect, currentSelect-1);
        currentSelect--;
    }

    int select = constrain(select, 0, 21);
    // you sure this is correct?
    // you sure this is correct?
    // you sure this is correct?
    if (!(digitalRead(53))) {
        hold = false;
        restaurant selectedRest;
        getRestaurantFast(currentSelect, &selectedRest);
        restX = selectedRest.lat; //convert to map
        restY = selectedRest.lon; //convert to map
    }
}


void redrawText(int prev, int current) { // implement
    // redrawing and reprinting old selection
    tft.setTextColor(TFT_WHITE);
    tft.drawRect(0,FONT_SIZE*prev, MAP_DISP_WIDTH, FONT_SIZE, 
        TFT_BLACK);
    tft.setCursor(0, prev*FONT_SIZE);
    char name[100]; 
    getRestaurantName(name, prev);
    tft.println(name);
    // redrawing and reprinting new selection
    tft.setTextColor(TFT_BLACK);
    tft.drawRect(0, FONT_SIZE*current, MAP_DISP_WIDTH, FONT_SIZE, 
        TFT_WHITE);
    tft.setCursor(0, current*FONT_SIZE);
    getRestaurantName(name, current);
    tft.println(name);
}


void drawDot(int x, int y) {
    // for dots on maps
}


void getRestaurantName(char name[], int index) {
    restaurant rest;
    getRestaurantFast(REST_DIST[index].index, &rest);
    name = rest.name;
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
        while (!card.readBlock(blockNum, (uint8_t*) TEMP_BLOCK)) {
            Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = TEMP_BLOCK[restIndex % 8];
    PREV_BLOCK_NUM = blockNum;

    // storing TEMP_BLOCK information in a smaller global struct
    REST_DIST[restIndex].index = restIndex;
    // need to calculate Manhattan distance here
    // need to correlate cursor position on map with actual position on map
}


/*
    Description: implementation of insertion sort.

    Arguments:
        array[] (int): pointer to an array.
        n (int): the number of items in the array.
    Notes:
        Need to change this function so it sorts based on the RestDist struct.
*/
void insertion_sort(RestDist array[], int n) {
    for (int i = 1; i < n; i++) {
        for (int j = i-1; j >= 0 && array[j].dist > array[j+1].dist; j--) {
            custom_swap(array[j], array[j+1]);
        }
    }
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
        // constrain cursor within the map
        constrainCursor(&cursorX, &cursorY);
        // constrain the display to the YEG map
        constrainMap(&shiftX, &shiftY);

        drawMapPatch(cursorX0, cursorY0);
        redrawMap(cursorX0, cursorY0);
        redrawCursor(TFT_RED);
    }
}


/*
    Description: constrains the cursor within the boundaries of the image
    displayed on the TFT display. This function assumes the cursor is a square.

    Arguments:
        cursorX (int*): the value stored at the memory location of the cursor's
        X position.
        cursorY (int*): the value stored at the memory location of the cursor's
        Y position.
*/
void constrainCursor(int* cursorX, int* cursorY) {
    // PAD accounts for integer division by 2 (i.e., cursor has odd sidelength)
    uint8_t PAD = 0;
    if (CURSOR_SIZE & 1) PAD = 1;

    *cursorX = constrain(*cursorX, (CURSOR_SIZE >> 1),
        MAP_DISP_WIDTH - (CURSOR_SIZE >> 1) - PAD);
    *cursorY = constrain(*cursorY, (CURSOR_SIZE >> 1),
        DISPLAY_HEIGHT - (CURSOR_SIZE >> 1) - PAD);
}


/*
    Description: draws a square cursor at the current cursor position.

    Arguments:
        colour (uint16_t): the colour of the cursor.
*/
void redrawCursor(uint16_t colour) {
    tft.fillRect(cursorX - (CURSOR_SIZE >> 1), cursorY - (CURSOR_SIZE >> 1),
        CURSOR_SIZE, CURSOR_SIZE, colour);
}


/*
    Description: draws a small map patch from the old cursor position up to the
    new cursor position (i.e., only redraws a rectangle if the cursor did not
    move too much).

    Arguments:
        cursorX0 (uint16_t): the original cursor's X position.
        cursorY0 (uint16_t): the original cursor's Y position.
*/
void drawMapPatch(int cursorX0, int cursorY0) {
    // middle of the YEG map
    int yegMiddleX = (YEG_SIZE - (MAP_DISP_WIDTH)) >> 1;
    int yegMiddleY = (YEG_SIZE - DISPLAY_HEIGHT) >> 1;
    // storing change in cursor position
    int diffX = cursorX - cursorX0;
    int diffY = cursorY - cursorY0;
    // storing appropriate irow and icol positions
    int icolPos = yegMiddleX + cursorX0 + (CURSOR_SIZE >> 1) + diffX + shiftX;
    int icolNeg = yegMiddleX + cursorX0 - (CURSOR_SIZE >> 1) + shiftX;
    int irowPos = yegMiddleY + cursorY0 + (CURSOR_SIZE >> 1) + diffY + shiftY;
    int irowNeg = yegMiddleY + cursorY0 - (CURSOR_SIZE >> 1) + shiftY;
    // storing appropriate srow and scol positions
    int scolPos = cursorX + (CURSOR_SIZE >> 1);
    int scolNeg = cursorX0 - (CURSOR_SIZE >> 1);
    int srowPos = cursorY + (CURSOR_SIZE >> 1);
    int srowNeg = cursorY0 - (CURSOR_SIZE >> 1);
    // PAD accounts for integer division by 2 (i.e., cursor has odd sidelength)
    uint8_t PAD = 0;
    if (CURSOR_SIZE & 1) PAD = 1;

    // draw the map
    if (diffX == 0 && diffY < 0) {
        // up
        lcdYegDraw(icolNeg, irowPos, scolNeg, srowPos, CURSOR_SIZE, -diffY+PAD);
    } else if (diffX == 0 && diffY > 0) {
        // down
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, CURSOR_SIZE, diffY);
    } else if (diffX < 0 && diffY == 0) {
        // left
        lcdYegDraw(icolPos, irowNeg, scolPos, srowNeg, -diffX+PAD, CURSOR_SIZE);
    } else if (diffX > 0 && diffY == 0) {
        // right
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, diffX, CURSOR_SIZE);
    } else if (diffY < 0 && diffX < 0) {
        // up and left
        lcdYegDraw(icolPos, irowNeg, scolPos, srowNeg, -diffX+PAD, CURSOR_SIZE);
        lcdYegDraw(icolNeg, irowPos, scolNeg, srowPos, CURSOR_SIZE, -diffY+PAD);
    } else if (diffY < 0 && diffX > 0) {
        // up and right
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, diffX, CURSOR_SIZE);
        lcdYegDraw(icolNeg, irowPos, scolNeg, srowPos, CURSOR_SIZE, -diffY+PAD);
    } else if (diffY > 0 && diffX < 0) {
        // down and left
        lcdYegDraw(icolPos, irowNeg, scolPos, srowNeg, -diffX+PAD, CURSOR_SIZE);
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, CURSOR_SIZE, diffY);
    } else {
        // down and right
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, diffX, CURSOR_SIZE);
        lcdYegDraw(icolNeg, irowNeg, scolNeg, srowNeg, CURSOR_SIZE, diffY);
    }
}


void constrainMap(int* shiftX, int* shiftY) {
    int yegMiddleX = (YEG_SIZE - (MAP_DISP_WIDTH)) >> 1;
    int yegMiddleY = (YEG_SIZE - DISPLAY_HEIGHT) >> 1;

    *shiftX = constrain(*shiftX, -yegMiddleX, YEG_SIZE - yegMiddleX);
    *shiftY = constrain(*shiftY, -yegMiddleY, YEG_SIZE - yegMiddleY);
}


void redrawMap(int cursorX0, int cursorY0) {
    // middle of the YEG map
    int yegMiddleX = (YEG_SIZE - (MAP_DISP_WIDTH)) >> 1;
    int yegMiddleY = (YEG_SIZE - DISPLAY_HEIGHT) >> 1;
    // storing change in cursor position
    int diffX = cursorX - cursorX0;
    int diffY = cursorY - cursorY0;
    // storing appropriate irow and icol positions
    int icolPos = yegMiddleX + cursorX0 + (CURSOR_SIZE >> 1) + diffX + shiftX;
    int icolNeg = yegMiddleX + cursorX0 - (CURSOR_SIZE >> 1) + shiftX;
    int irowPos = yegMiddleY + cursorY0 + (CURSOR_SIZE >> 1) + diffY + shiftY;
    int irowNeg = yegMiddleY + cursorY0 - (CURSOR_SIZE >> 1) + shiftY;
    // storing appropriate srow and scol positions
    int scolPos = cursorX + (CURSOR_SIZE >> 1);
    int scolNeg = cursorX0 - (CURSOR_SIZE >> 1);
    int srowPos = cursorY + (CURSOR_SIZE >> 1);
    int srowNeg = cursorY0 - (CURSOR_SIZE >> 1);
    // PAD accounts for integer division by 2 (i.e., cursor has odd sidelength)
    uint8_t PAD = 0;
    if (CURSOR_SIZE & 1) PAD = 1;

    int leftShift = constrain(icolPos - MAP_DISP_WIDTH - scolPos, 0,
        YEG_SIZE-MAP_DISP_WIDTH);
    int rightShift = constrain(icolPos + MAP_DISP_WIDTH - scolPos, 0,
        YEG_SIZE-MAP_DISP_WIDTH);
    int upShift = constrain(irowNeg - srowNeg - MAP_DISP_HEIGHT, 0,
        YEG_SIZE-MAP_DISP_HEIGHT);
    int downShift = constrain(irowNeg - srowNeg + MAP_DISP_HEIGHT, 0,
        YEG_SIZE-MAP_DISP_HEIGHT);

    bool moveLeft = true, moveRight, moveUp = true, moveDown = true;
    // implement no redraw at map boundaries

    // (CURSOR_SIZE << 1) leaves buffer at the map location that the cursor
    // was previously just in case the user wants to reverse his/her action
    // buffer exists because drawing the map takes a long time
    if (cursorX == (CURSOR_SIZE >> 1) && moveLeft) {
        // left side of screen reached
        lcdYegDraw(leftShift, irowNeg - srowNeg, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        shiftX -= MAP_DISP_WIDTH;
        cursorX += MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (cursorY == (CURSOR_SIZE >> 1) && moveUp) {
        // top of screen reached
        lcdYegDraw(icolPos - scolPos, upShift, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        shiftY -= MAP_DISP_HEIGHT;
        cursorY += MAP_DISP_HEIGHT - (CURSOR_SIZE << 1);
    } else if (cursorX == MAP_DISP_WIDTH - (CURSOR_SIZE >> 1) - PAD && moveRight) {
        // right side of sign reached
        lcdYegDraw(rightShift, irowNeg - srowNeg, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        shiftX += MAP_DISP_WIDTH;
        cursorX -= MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (cursorY == MAP_DISP_HEIGHT - (CURSOR_SIZE >> 1) - PAD && moveDown) {
        // bottom of screen reached
        lcdYegDraw(icolPos - scolPos, downShift, 0, 0,
            MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
        shiftY += MAP_DISP_HEIGHT;
        cursorY -= MAP_DISP_HEIGHT - (CURSOR_SIZE << 1);
    }
}


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
void lcdYegDraw(int icol, int irow, int scol, int srow, int width, int height) {
    lcd_image_draw(&yegImage, &tft, icol, irow, scol, srow, width, height);
}


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
