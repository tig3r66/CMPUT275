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
bool HIT_UP = false, HIT_DOWN = false, HIT_LEFT = false, HIT_RIGHT = false;

// for the display mode
uint8_t MODE = 0; 


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
    readRestData();
    lcd_setup();

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

    // position of current cursor on map
    int icol = YEG_MIDDLE_X + cursorX + shiftX;
    int irow = YEG_MIDDLE_Y + cursorY + shiftY;

    // calculating Manhattan distance and storing into global REST_DIST struct
    int manDist = abs(icol - lon_to_x(restPtr->lon))
        + abs(irow - lat_to_y(restPtr->lat));
    REST_DIST[restIndex].dist = manDist;
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


// FIX THIS
void modeOne() {
    readRestData();
    insertion_sort(REST_DIST, NUM_RESTAURANTS);
    printRestList();

    // processing menu
    uint16_t selection = 0;
    while (true) {
        menuProcess(selection);
        if (!(digitalRead(JOY_SEL))) { // FIX THIS
            tft.fillRect(MAP_DISP_WIDTH, 0, PADX, MAP_DISP_HEIGHT, TFT_BLACK);

            // storing latitude and longitude info for the selected restaurant
            restaurant temp;
            getRestaurantFast(selection, &temp);
            uint16_t xPos = lon_to_x(temp.lon); // x pos on map
            uint16_t yPos = lat_to_y(temp.lat); // y pos on map
            // constrain x and y to map
            xPos = constrain(xPos, 0, YEG_SIZE);
            yPos = constrain(yPos, 0, YEG_SIZE);
            // if selected rest within map
            uint16_t xEdge = xPos - MAP_DISP_WIDTH; // x corner of patch
            uint16_t yEdge = yPos - MAP_DISP_HEIGHT; // y corner of patch
            //keeping corners of patch within bounds of map
            xEdge = constrain(xEdge, 0, YEG_SIZE - MAP_DISP_WIDTH); // 0 to 2048 - patch width
            yEdge = constrain(yEdge, 0, YEG_SIZE - MAP_DISP_HEIGHT); // 0 to 2048 - patch height
            lcdYegDraw(xEdge, yEdge, 0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT); // draw patch
            // reset cursor postions
            cursorX = xPos - xEdge;
            cursorY = yPos - yEdge;
            constrainCursor(cursorX, cursorY); // ensuring cursor isnt in illegal pos
            redrawCursor(TFT_RED);
            // reset shifts
            shiftX = xEdge - YEG_MIDDLE_X;
            shiftY = yEdge - YEG_MIDDLE_Y; 
            break;
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
    // int screen_y = map(touch.x, TS_MINY, TS_MAXY, tft.height() - 1, 0);
    if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE
        || screen_x > MAP_DISP_WIDTH) {
            return;
    } else if (screen_x < MAP_DISP_WIDTH) {
        drawCloseRests(3, MAP_DISP_WIDTH + MAP_DISP_HEIGHT, TFT_BLUE);
    }
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
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        restaurant tempRest;
        getRestaurantFast(REST_DIST[i].index, &tempRest);
        int16_t scol = lon_to_x(tempRest.lon) - (YEG_MIDDLE_X + shiftX);
        int16_t srow = lat_to_y(tempRest.lat) - (YEG_MIDDLE_Y + shiftY);

        if (scol < MAP_DISP_WIDTH && srow < MAP_DISP_HEIGHT && srow >= 0
            && scol >= 0) {
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
        &selection (uint16_t): the memory address of the selected restaurant's
            index.
*/
void menuProcess(uint16_t& selection) {
    uint16_t joyY = analogRead(JOY_VERT);
    if (joyY < (JOY_CENTER - JOY_DEADZONE)) {
        // scroll one up
        if (selection > 0) {
            selection--;
            redrawText(selection, selection+1); 
        }
    } else if (joyY > (JOY_CENTER + JOY_DEADZONE)) {
        // scroll one down
        if (selection < MAX_LIST - 1) {
            selection++;
            redrawText(selection, selection-1);
        }
    }
    // for better (less sensitive) scrolling user experience
    delay(50);
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
        // redraw
        drawMapPatch(cursorX0, cursorY0);
        // draws once cursor reaches border (except at physical map border)
        redrawMap();
        redrawCursor(TFT_RED);
    }
}


/*
    Description: constrains the cursor within the boundaries of the image
    displayed on the TFT display. This function assumes the cursor is a square.

    Arguments:
        cursorX (int*): pointer to the cursor's X position.
        cursorY (int*): pointer to the cursor's Y position.
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
    // storing change in cursor position
    int diffX = cursorX - cursorX0;
    int diffY = cursorY - cursorY0;
    // storing appropriate irow and icol positions
    int icolPos = YEG_MIDDLE_X + cursorX0 + (CURSOR_SIZE >> 1) + diffX + shiftX;
    int icolNeg = YEG_MIDDLE_X + cursorX0 - (CURSOR_SIZE >> 1) + shiftX;
    int irowPos = YEG_MIDDLE_Y + cursorY0 + (CURSOR_SIZE >> 1) + diffY + shiftY;
    int irowNeg = YEG_MIDDLE_Y + cursorY0 - (CURSOR_SIZE >> 1) + shiftY;
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


/*
    Description: constrains the global shift in X and Y position to within the
    physical boundaries of the YEG map.

    Arguments:
        shiftX (int*): pointer to the X shift.
        shiftY (int*): pointer to the Y shift.
*/
void constrainMap(int* shiftX, int* shiftY) {
    *shiftX = constrain(*shiftX, -YEG_MIDDLE_X,
        YEG_SIZE - YEG_MIDDLE_X - MAP_DISP_WIDTH);
    *shiftY = constrain(*shiftY, -YEG_MIDDLE_Y,
        YEG_SIZE - YEG_MIDDLE_Y - MAP_DISP_HEIGHT);
}


/*
    Description: redraws the shifted map of Edmonton depending on the cursor
    nudge direction.

    Arguments:
        cursorX0 (uint16_t): the original cursor's X position.
        cursorY0 (uint16_t): the original cursor's Y position.
*/
void redrawMap() {
    // storing appropriate irow and icol positions
    int icol = YEG_MIDDLE_X + cursorX + shiftX;
    int irow = YEG_MIDDLE_Y + cursorY + shiftY;
    // PAD accounts for integer division by 2 (i.e., cursor has odd sidelength)
    uint8_t PAD = 0;
    if (CURSOR_SIZE & 1) PAD = 1;

    int leftShift = constrain(icol - MAP_DISP_WIDTH - cursorX, 0,
        YEG_SIZE - MAP_DISP_WIDTH);
    int upShift = constrain(irow - cursorY - MAP_DISP_HEIGHT, 0,
        YEG_SIZE - MAP_DISP_HEIGHT);
    int rightShift = constrain(icol + MAP_DISP_WIDTH - cursorX, 0,
        YEG_SIZE - MAP_DISP_WIDTH);
    int downShift = constrain(irow - cursorY + MAP_DISP_HEIGHT, 0,
        YEG_SIZE - MAP_DISP_HEIGHT);

    // (CURSOR_SIZE << 1) leaves buffer for user at the previous map location
    if (cursorX == (CURSOR_SIZE >> 1) && !HIT_LEFT) {
        // left side of screen reached
        lcdYegDraw(leftShift, irow - cursorY, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        if (leftShift == 0) HIT_LEFT = true;
        helperMove(&HIT_RIGHT, &shiftX, "left");
    } else if (cursorY == (CURSOR_SIZE >> 1) && !HIT_UP) {
        // top of screen reached
        lcdYegDraw(icol - cursorX, upShift, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        if (upShift == 0) HIT_UP = true;
        helperMove(&HIT_DOWN, &shiftY, "up");
    } else if (cursorX == MAP_DISP_WIDTH - (CURSOR_SIZE >> 1) - PAD
        && !HIT_RIGHT) {
            // right side of sign reached
            lcdYegDraw(rightShift, irow - cursorY, 0, 0, MAP_DISP_WIDTH,
                MAP_DISP_HEIGHT);
            if (rightShift == YEG_SIZE - MAP_DISP_WIDTH) HIT_RIGHT = true;
            helperMove(&HIT_LEFT, &shiftX, "right");
    } else if (cursorY == MAP_DISP_HEIGHT - (CURSOR_SIZE >> 1) - PAD
        && !HIT_DOWN) {
            // bottom of screen reached
            lcdYegDraw(icol - cursorX, downShift, 0, 0,
                MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
            if (downShift == YEG_SIZE - MAP_DISP_HEIGHT) HIT_DOWN = true;
            helperMove(&HIT_UP, &shiftY, "down");
    }
}


/*
    Description: helper function for redrawMap() which aids with map boundary
    clamping and map shifting.

    Arguments:
        oppDir (bool*): pointer to the opposite direction of map shift.
        shiftLen (int*): the X or Y shift to store for future map shifts.
        mainDir (const char*): user passes in the parameter of movement.
*/
void helperMove(bool* oppDir, int* shiftLen, const char* mainDir) {
    *oppDir = false;
    if (strcmp(mainDir, "left") == 0) {
        *shiftLen -= MAP_DISP_WIDTH;
        cursorX += MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "right") == 0) {
        *shiftLen += MAP_DISP_WIDTH;
        cursorX -= MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "up") == 0) {
        *shiftLen -= MAP_DISP_HEIGHT;
        cursorY += MAP_DISP_HEIGHT - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "down") == 0) {
        *shiftLen += MAP_DISP_HEIGHT;
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
