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

// struct holding SD card read information
RestCache cache;
// cursor position variable
MapView yeg;


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

    // for main display mode and restaurant rating filter
    uint8_t MODE = 0, RATING = 1;
    // 0 = quicksort, 1 = insertionsort, 2 = both
    uint8_t SORT_MODE = 0;
    // drawing initial options
    drawSideBar(RATING, SORT_MODE, 3, ONE_PURPLE);

    while (true) {
        if (MODE == 0) {
            // joystick button pressed
            if (!digitalRead(JOY_SEL)) MODE = 1;
            // if user touches screen, draw closest restaurants
            processTouchScreen(&RATING, &SORT_MODE);
            // min and max cursor speeds are 0 and CURSOR_SIZE pixels/cycle
            modeZero(0, CURSOR_SIZE);
        } else if (MODE == 1) {
            tft.fillScreen(TFT_BLACK);
            // loops until the joystick button is pressed
            modeOne(SORT_MODE, RATING);
            drawSideBar(RATING, SORT_MODE, 3, ONE_PURPLE);
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
    } else {
        Serial.println("OK!");
    }

    // SD card initialization for raw reads
    Serial.print("Initializing SPI communication...");
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
    yeg.cursorX = MAP_DISP_WIDTH >> 1;
    yeg.cursorY = DISPLAY_HEIGHT >> 1;
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
    int cursorX0 = yeg.cursorX, cursorY0 = yeg.cursorY;

    // x movement
    if (xVal > EPSILON_POS) {
        yeg.cursorX -= map(xVal, EPSILON_POS, MAX_STICK, slow, fast);
    } else if (xVal < EPSILON_NEG) {
        yeg.cursorX += map(xVal, 0, EPSILON_NEG, fast, slow);
    }
    // y movement
    if (yVal < EPSILON_NEG) {
        yeg.cursorY -= map(yVal, 0, EPSILON_NEG, fast, slow);
    } else if (yVal > EPSILON_POS) {
        yeg.cursorY += map(yVal, EPSILON_POS, MAX_STICK, slow, fast);
    }

    // if the cursor moved
    if (cursorX0 != yeg.cursorX || yeg.cursorY != cursorY0) {
        // constrain cursor and display within the map then redraw the map patch
        constrainCursor(&yeg.cursorX, &yeg.cursorY);
        constrainMap(&yeg.shiftX, &yeg.shiftY);
        drawMapPatch(cursorX0, cursorY0);
        // screen (not image) position of cursor on the YEG map
        int icol = YEG_MIDDLE_X + yeg.cursorX + yeg.shiftX - (CURSOR_SIZE >> 1);
        int irow = YEG_MIDDLE_Y + yeg.cursorY + yeg.shiftY - (CURSOR_SIZE >> 1);

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

    Arguments:
        sortMode (uint8_t): the selected sorting algorithm.
        rating (uint8_t): the restaurant rating filter.
*/
void modeOne(uint8_t sortMode, uint8_t rating) {
    sortTimer(sortMode, rating);
    printRestList(0, 0);

    // processing menu
    uint16_t selection = 0;
    uint16_t pageNum = 0;
    while (true) {
        menuProcess(&selection, &pageNum);
        if (!(digitalRead(JOY_SEL))) {
            tft.fillRect(MAP_DISP_WIDTH, 0, PADX, MAP_DISP_HEIGHT, TFT_BLACK);
            redrawOverRest(selection + pageNum * MAX_LIST);
            return;
        }
    }
}


/*
    Description: processes touches on the TFT display. When the user touches the
    map, the closest restaurants to the cursor are drawn as blue dots.

    Arguments:
        rating (uint8_t*): pointer to the restaurant rating filter.
        sortMode (uint8_t*): pointer to the desired sorting algorithm.
*/
void processTouchScreen(uint8_t* rating, uint8_t* sortMode) {
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    // touch coordinates
    int screen_x = map(touch.y, TS_MINX, TS_MAXX, tft.width() - 1, 0);
    int screen_y = map(touch.x, TS_MINY, TS_MAXY, tft.height() - 1, 0);

    if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
        return;
    } else if (screen_x < MAP_DISP_WIDTH) {
        readRestData(*rating);
        drawCloseRests(*rating, 3, MAP_DISP_WIDTH + MAP_DISP_HEIGHT, TFT_BLUE);
    } else if (screen_y < (DISPLAY_HEIGHT >> 1)) {
        // restaurant rating mapped from a range of [1, 10] to [1, 5]
        *rating = constrain((*rating + 1) % 6, 1, 5);
        drawRating(*rating);
    } else if (screen_y > (DISPLAY_HEIGHT >> 1)) {
        // cycles through QSORT (0), ISORT (1), and BOTH (2)
        *sortMode = (*sortMode + 1) % 3;
        drawSortMode(*sortMode);
    }
    delay(200);
}


/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
    card into the global cache struct member TEMP_BLOCK then stores this
    information into a smaller global cache member REST_DIST. Reads a block once
    for consecutive restaurants on the same block.

    Arguments:
        restIndex (uint16_t): the restaurant to be read.
        rating (uint8_t): the restaurant rating filter.
        counter (uint16_t*): the number of restaurants to retrieve.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurantFast(
    uint16_t restIndex, uint8_t rating, uint16_t* counter, restaurant* restPtr
) {
    uint32_t blockNum = REST_START_BLOCK + restIndex / 8;
    if (blockNum != cache.PREV_BLOCK_NUM) {
        while (!card.readBlock(blockNum,
            reinterpret_cast<uint8_t*>(cache.TEMP_BLOCK))) {
                Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = cache.TEMP_BLOCK[restIndex % 8];
    cache.PREV_BLOCK_NUM = blockNum;
    // storing TEMP_BLOCK information in a smaller struct
    // taking the max of mapped restaurant rating, [1, 10] -> [1, 5], and 1
    if (max((((*restPtr).rating + 1) >> 1), 1) >= rating) {
        cache.REST_DIST[*counter].index = restIndex;
        // image (not screen) position of cursor on the YEG map
        int icol = YEG_MIDDLE_X + yeg.cursorX + yeg.shiftX;
        int irow = YEG_MIDDLE_Y + yeg.cursorY + yeg.shiftY;
        // storing Manhattan distance into cache member REST_DIST
        int manDist = abs(icol - lon_to_x(restPtr->lon))
                        + abs(irow - lat_to_y(restPtr->lat));
        cache.REST_DIST[*counter].dist = manDist;
        (*counter)++;
    }
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
    if (blockNum != cache.PREV_BLOCK_NUM) {
        while (!card.readBlock(blockNum,
            reinterpret_cast<uint8_t*>(cache.TEMP_BLOCK))) {
                Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = cache.TEMP_BLOCK[restIndex % 8];
    cache.PREV_BLOCK_NUM = blockNum;
}


/*
    Description: reads all restaurant data from the SD card.

    Arguments:
        rating (uint8_t): the restaurant rating filter.
*/
void readRestData(uint8_t rating) {
    uint16_t counter = 0;
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        getRestaurantFast(i, rating, &counter, cache.TEMP_BLOCK);
    }
    cache.READ_SIZE = counter;
}


/*
    Description: initial drawing of the names of the closest 21 restaurants to
    the cursor. Highlights the first entry.

    Arguments:
        pageNum (uint16_t): the page of restaurants to display.
        selectedRest (uint16_t): the currently selected restaurant index.
*/
void printRestList(uint16_t pageNum, uint16_t selectedRest) {
    tft.setCursor(0, 0);
    for (uint8_t i = 0; i < MAX_LIST; i++) {
        if (pageNum * MAX_LIST + i >= cache.READ_SIZE) {
            return;
        }

        restaurant rest;
        getRestaurant(cache.REST_DIST[i + (pageNum * MAX_LIST)].index, &rest);
        if (i == selectedRest) {
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
        selection (uint16_t*): pointer to the selected restaurant's index.
        pageNum (uint16_t*): pointer to the appropriate restaurant page. 
*/
void menuProcess(uint16_t* selection, uint16_t* pageNum) {
    uint16_t joyY = analogRead(JOY_VERT);
    if (joyY < (JOY_CENTER - JOY_DEADZONE)) {
        scrollUp(selection, pageNum);
    } else if (joyY > (JOY_CENTER + JOY_DEADZONE)) {
        scrollDown(selection, pageNum);
    }
    // for better (less sensitive) scrolling user experience
    delay(25);
}


/*
    Description: helper function for highlighting the entry if scrolling up and
    page scroll if the user tries to nudge select a restaurant past the top of
    the screen.

    Arguments:
        selection (uint16_t*): pointer to the selected restaurant's index.
        pageNum (uint16_t*): pointer to the appropriate restaurant page. 
*/
void scrollUp(uint16_t* selection, uint16_t* pageNum) {
    if (*selection > 0) {
        // highlight the entry above
        (*selection)--;
        redrawText((*pageNum) * MAX_LIST + (*selection),
            (*pageNum) * MAX_LIST + (*selection) + 1);
    } else if (*selection == 0 && *pageNum > 0) {
        // scrolling for pages that are not the first or last
        (*pageNum)--;
        *selection = MAX_LIST - 1;
        tft.fillScreen(TFT_BLACK);
        printRestList(*pageNum, *selection);
    } else {
        // wrapping around to last page (originally on first page)
        *pageNum = cache.READ_SIZE / MAX_LIST;
        if ((*pageNum) * MAX_LIST == cache.READ_SIZE) {
            (*pageNum)--;
            *selection = MAX_LIST - 1;
        } else {
            *selection = (cache.READ_SIZE - 1) - ((*pageNum) * MAX_LIST);
        }
        tft.fillScreen(TFT_BLACK);
        printRestList(*pageNum, *selection);
    }
}


/*
    Description: helper function for highlighting the entry if scrolling down
    and page scroll if the user tries to nudge select a restaurant past the
    bottom of the screen.

    Arguments:
        selection (uint16_t*): pointer to the selected restaurant's index.
        pageNum (uint16_t*): pointer to the appropriate restaurant page. 
*/
void scrollDown(uint16_t* selection, uint16_t* pageNum) {
    if (*selection < MAX_LIST - 1
        && (*selection) + (*pageNum) * MAX_LIST < cache.READ_SIZE - 1) {
            // highlight the entry below
            (*selection)++;
            redrawText((*pageNum) * MAX_LIST + (*selection),
                (*pageNum) * MAX_LIST + *selection - 1);
    } else if (*selection == MAX_LIST - 1
        && (*selection) + (*pageNum) * MAX_LIST < cache.READ_SIZE - 1) {
            // scrolling down a page that is not the last page
            (*pageNum)++;
            *selection = 0;
            tft.fillScreen(TFT_BLACK);
            printRestList(*pageNum, *selection);
    } else {
        // wrapping around to the first page (originally on last page)
        *pageNum = 0;
        *selection = 0;
        tft.fillScreen(TFT_BLACK);
        printRestList(*pageNum, *selection);
    }
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
    getRestaurant(cache.REST_DIST[prev].index, &tempRest);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(0, (prev % MAX_LIST) * FONT_SIZE);
    tft.print(tempRest.name);

    // get current name and print it
    getRestaurant(cache.REST_DIST[current].index, &tempRest);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setCursor(0, (current % MAX_LIST) * FONT_SIZE);
    tft.print(tempRest.name);
}
