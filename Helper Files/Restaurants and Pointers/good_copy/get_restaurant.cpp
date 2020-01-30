//  ============================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Weekly Exercise 2: Restuarants and Pointers
//  ============================================

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>
#include "get_restaurant.h"

// tft display and sd card objects
MCUFRIEND_kbv tft;
Sd2Card card;

// initialize with 300 ohms of resistance (multimeter measured 300 ohms across
// the plate)
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// variables holding SD card read information
restaurant TEMP_BLOCK[8];
unsigned long PREV_BLOCK_NUM = 0;


/*
    Description: displays running time of two restaurant-retrieving functions
        using a touchscreen UI.
*/
int main() {
    setup();

    // variables relating to runs data and temporary block storage
    unsigned int slowRuns = 0, fastRuns = 0, slowTimeSum = 0, fastTimeSum = 0;
    // formatting default text size and colour
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    printRunLabels();
    drawOptionButtons();

    while (true) {
        processTouchScreen(slowRuns, fastRuns, slowTimeSum, fastTimeSum);
    }

    Serial.end();
    return 0;
}


/*
    Description: initializes appropriate Arduino and TFT display components.
*/
void setup() {
    init();
    Serial.begin(9600);

    // tft display initialization
    uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);

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
    Description: slow implementation of getRestaurant(). Reads data from an SD
        into a restaurant struct. Reads each an entire block each time the
        function is called.

    Arguments:
        restIndex (int): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurant(int restIndex, restaurant* restPtr) {
    uint32_t blockNum = REST_START_BLOCK + restIndex/8;
    restaurant restBlock[8];
    while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
        Serial.println("Read block failed, trying again.");
    }
    *restPtr = restBlock[restIndex % 8];
}


/*
    Description: fast implementation of getRestaurant(). Reads data from an SD
        card into the global restaurant struct TEMP_BLOCK. Reads a block once
        for consecutive restaurants on the same block.

    Arguments:
        restIndex (int): the restaurant to be read.
        restPtr (restaurant*): pointer to the restaurant struct.
*/
void getRestaurantFast(int restIndex, restaurant* restPtr) {
    uint32_t blockNum = REST_START_BLOCK + restIndex / 8;
    if (blockNum != PREV_BLOCK_NUM) {
        while (!card.readBlock(blockNum, (uint8_t*) TEMP_BLOCK)) {
            Serial.println("Read block failed, trying again.");
        }
    }
    *restPtr = TEMP_BLOCK[restIndex % 8];
    PREV_BLOCK_NUM = blockNum;
}


/*
    Description: tests the running time of getRestaurant() functions.

    Arguments:
        (*getRestFunc)(int, restaurant*): pointer to the supplied
            getRestaurant() function.
        rest (restaurant*): pointer to a restaurant struct.
    Returns:
        millis() - start (unsigned long): running time of the function of
            interest.
*/
unsigned long getRunningTime(void (*getRestFunc)(int, restaurant*),
    restaurant* rest
) {
    unsigned long start = millis();
    for (int i = 0; i < NUM_RESTAURANTS; i++) {
        (*getRestFunc)(i, rest);
    }
    return (millis() - start);
}


/*
    Description: processes touches on the TFT display.

    Arguments:
        &slowRuns (unsigned int): memory address of number of slow run trials.
        &fastRubs (unsigned int): memory address of number of fast run trials.
        &slowTimeSum (unsigned int): memory address of sum  of slow times.
        &fastTimeSum (unsigned int): memory address of sum  of fast times.
*/
void processTouchScreen(unsigned int& slowRuns, unsigned int& fastRuns,
    unsigned int& slowTimeSum, unsigned int& fastTimeSum
) {
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    int screen_x = map(touch.y, TS_MINX, TS_MAXX, tft.width() - 1, 0);
    int screen_y = map(touch.x, TS_MINY, TS_MAXY, tft.height() - 1, 0);
    if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE
        || screen_x < MAP_DISP_WIDTH) {
            return;
    }

    if (screen_y < DISPLAY_HEIGHT / 3) {
        eraseRuns(0, SLOW_RUN_PADY);
        eraseRuns(0, SLOW_RUN_AVG_PADY);
        printSlowRun(slowRuns, slowTimeSum);
    } else if (DISPLAY_HEIGHT / 3 < screen_y &&
        screen_y < DISPLAY_HEIGHT * 2 / 3) {
            eraseRuns(0, FAST_RUN_PADY);
            eraseRuns(0, FAST_RUN_AVG_PADY);
            printFastRun(fastRuns, fastTimeSum);
    } else {
        eraseRuns(0, SLOW_RUN_PADY);
        eraseRuns(0, SLOW_RUN_AVG_PADY);
        eraseRuns(0, FAST_RUN_PADY);
        eraseRuns(0, FAST_RUN_AVG_PADY);
        printFastRun(fastRuns, fastTimeSum);
        printSlowRun(slowRuns, slowTimeSum);
    }

    delay(200);
}


/*
    Description: prints the running time of the recent slow run and recent slow
        run average to the tft display.

    Arguments:
        &slowRuns (unsigned int): memory address of the number of slow run
            trials.
        &slowTimeSum (unsigned int): memory address of sum of slow run time
            trials.
*/
void printSlowRun(unsigned int& slowRuns, unsigned int& slowTimeSum) {
    // getting elapsed time
    restaurant rest;
    unsigned long timeElapsed = getRunningTime(getRestaurant, &rest);
    // printing
    tft.setCursor(0, SLOW_RUN_PADY);
    tft.print(timeElapsed);
    tft.print(" ms");

    // displaying average slow run time
    unsigned long slowAverage = runAvg(timeElapsed, &slowRuns, &slowTimeSum);
    tft.setCursor(0, SLOW_RUN_AVG_PADY);
    tft.print(slowAverage);
    tft.print(" ms");
}


/*
    Description: prints the running time of the recent fast run and recent fast
        run average to the tft display.

    Arguments:
        &fastRuns (unsigned int): memory address of the number of fast run
            trials.
        &fastTimeSum (unsigned int): memory address of sum of fast run time
            trials.
*/
void printFastRun(unsigned int& fastRuns, unsigned int& fastTimeSum) {
    // getting elapsed time
    unsigned long timeElapsed = getRunningTime(getRestaurantFast, TEMP_BLOCK);
    // printing
    tft.setCursor(0, FAST_RUN_PADY);
    tft.print(timeElapsed);
    tft.print(" ms");

    // displaying average fast run time
    unsigned long fastAverage = runAvg(timeElapsed, &fastRuns, &fastTimeSum);
    tft.setCursor(0, FAST_RUN_AVG_PADY);
    tft.print(fastAverage);
    tft.print(" ms");
}


/*
    Description: calculates the average time needed for a function to run.
        Updates the number of runs and average variables.

    Arguments:
        runningTime (unsigned int): the running time of the current trial.
        nRuns (unsigned int*): pointer to the number of runs.
        sum (unsigned int*): pointer to the sum time for all runs.
    Returns:
        average (unsigned long): the average running time.
    Note:
        Performs integer division. Assumes that the program does not overflow an
            unsigned long.
*/
unsigned long runAvg(unsigned int runningTime, unsigned int* nRuns,
    unsigned int* sum
) {
    ++(*nRuns);
    *sum += runningTime;
    unsigned long average = (*sum) / (*nRuns);
    return average;
}


/*
    Description: given a string, prints the string characters vertically on the
        TFT display.

    Arguments:
        word (const char*): pointer to an immutable character/string.
*/
void printWord(const char* word) {
    // getting string length
    int word_length = 0;
    while (word[word_length] != '\0') {
        word_length++;
    }
    // printing text on tft display
    for (int i = 0; i < word_length; i++) {
        tft.setCursor(MAP_DISP_WIDTH + PADX, tft.getCursorY());
        tft.println(word[i]);
    }
}


/*
    Description: draws three buttons (SLOW, FAST, BOTH) on the TFT display.
*/
void drawOptionButtons() {
    uint8_t PAD = 60;
    tft.drawRect(MAP_DISP_WIDTH, 0, PAD, DISPLAY_HEIGHT, TFT_RED);
    tft.drawLine(MAP_DISP_WIDTH, DISPLAY_HEIGHT / 3, DISPLAY_WIDTH,
        DISPLAY_HEIGHT / 3, TFT_RED);
    tft.drawLine(MAP_DISP_WIDTH, DISPLAY_HEIGHT * 2 / 3, DISPLAY_WIDTH,
        DISPLAY_HEIGHT * 2 / 3, TFT_RED);

    // divide the display height into 15 rows -- words spaced by 5 rows each
    tft.setCursor(MAP_DISP_WIDTH + PADX, DISPLAY_HEIGHT / 15);
    printWord("SLOW");
    tft.setCursor(MAP_DISP_WIDTH + PADX, DISPLAY_HEIGHT * 6 / 15);
    printWord("FAST");
    tft.setCursor(MAP_DISP_WIDTH + PADX, DISPLAY_HEIGHT * 11 / 15);
    printWord("BOTH");
}


/*
    Description: prints initial runs labels on the TFT display.
*/
void printRunLabels() {
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_CYAN);
	tft.println("RECENT SLOW RUN:");
    tft.setTextColor(TFT_WHITE);
	tft.println("Not yet run\n");

    tft.setTextColor(TFT_CYAN);
	tft.println("SLOW RUN AVG:");
    tft.setTextColor(TFT_WHITE);
	tft.println("Not yet run\n");

    tft.setTextColor(TFT_CYAN);
	tft.println("RECENT FAST RUN:");
    tft.setTextColor(TFT_WHITE);
	tft.println("Not yet run\n");

    tft.setTextColor(TFT_CYAN);
	tft.println("FAST RUN AVG:");
    tft.setTextColor(TFT_WHITE);
	tft.println("Not yet run");
}


/*
    Description: draws a black rectangle at (130, 20) on the TFT display of
        a desired width and height.

    Arguments:
        width (uint8_t): the desired rectangle width.
        height (uint8_t): the desired rectangle height.
*/
void eraseRuns(uint8_t width, uint8_t height) {
    uint8_t ERASE_WIDTH = 130, ERASE_HEIGHT = 20;
    tft.fillRect(width, height, ERASE_WIDTH, ERASE_HEIGHT, TFT_BLACK);
    tft.fillRect(width, height, ERASE_WIDTH, ERASE_HEIGHT, TFT_BLACK);
}
