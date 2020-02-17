//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

/*
    Implements map redrawing and helper functions. Please see yeg_draw.cpp for
    full function documentation.
*/

#ifndef _YEG_DRAW_H_
#define _YEG_DRAW_H_

#include <Arduino.h>
#include <MCUFRIEND_kbv.h>

#include "a1part2.h"
#include "yeg_rest_coords.h"
#include "lcd_image.h"


// =========================== FUNCTION DECLARATIONS ===========================
/*
    Description: redraws the shifted map of Edmonton depending on the cursor
    nudge direction.
*/
void redrawMap();

/*
    Description: redraws the map of Edmonton centered as much as possible over
    the selected restaurant.
*/
void redrawOverRest(uint16_t selection);

/*
    Description: draws a small map patch from the old cursor position up to the
    new cursor position (i.e., only redraws a rectangle if the cursor did not
    move too much).
*/
void drawMapPatch(int cursorX0, int cursorY0);

/*
    Description: draws dots over restaurants that are closest to the cursor.
*/
void drawCloseRests(uint8_t rating, uint8_t radius, uint16_t distance,
    uint16_t colour);

/*
    Description: helper function for redrawMap() which aids with map boundary
    clamping and map shifting.
*/
void helperMove(int* shiftLen, const char* mainDir);

/*
    Description: draws a square cursor at the current cursor position.
*/
void redrawCursor(uint16_t colour);

/*
    Description: constrains the cursor within the boundaries of the image
    displayed on the TFT display. This function assumes the cursor is a square.
*/
void constrainCursor(int* cursorX, int* cursorY);

/*
    Description: constrains the global shift in X and Y position to within the
    physical boundaries of the YEG map.
*/
void constrainMap(int* shiftX, int* shiftY);

/*
    Description: helper function for drawMapPatch(). Essentially the
    lcd_image_draw() except it assumes the image's memory address is &yegImage
    and the TFT object's memory address is &tft.
*/
void lcdYegDraw(int icol, int irow, int scol, int srow, int width, int height);

/*
    Description: given a string, prints the string characters vertically on the
        TFT display.
*/
void printWord(const char* word);

/*
    Description: draws two buttons on the TFT display.
*/
void drawOptionButtons(uint8_t rating, uint8_t sortMode, uint8_t thickness,
    uint16_t colour);

#endif
