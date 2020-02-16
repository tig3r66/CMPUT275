//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

#include "include/yeg_draw.h"


/*
    Description: redraws the shifted map of Edmonton depending on the cursor
    nudge direction.

    Arguments:
        cursorX0 (uint16_t): the original cursor's X position.
        cursorY0 (uint16_t): the original cursor's Y position.
*/
void redrawMap() {
    // image (not screen) position of cursor on the YEG map
    int icol = YEG_MIDDLE_X + yeg.cursorX + yeg.shiftX;
    int irow = YEG_MIDDLE_Y + yeg.cursorY + yeg.shiftY;
    // PAD accounts for integer division by 2 (i.e., cursor has odd sidelength)
    uint8_t PAD = 0;
    if (CURSOR_SIZE & 1) PAD = 1;

    int leftShift = constrain(icol - MAP_DISP_WIDTH - yeg.cursorX, 0,
        YEG_SIZE - MAP_DISP_WIDTH);
    int upShift = constrain(irow - yeg.cursorY - MAP_DISP_HEIGHT, 0,
        YEG_SIZE - MAP_DISP_HEIGHT);
    int rightShift = constrain(icol + MAP_DISP_WIDTH - yeg.cursorX, 0,
        YEG_SIZE - MAP_DISP_WIDTH);
    int downShift = constrain(irow - yeg.cursorY + MAP_DISP_HEIGHT, 0,
        YEG_SIZE - MAP_DISP_HEIGHT);

    // screen (not image) position of cursor on the YEG map
    int scrnX = YEG_MIDDLE_X + yeg.cursorX + yeg.shiftX - (CURSOR_SIZE >> 1);
    int scrnY = YEG_MIDDLE_Y + yeg.cursorY + yeg.shiftY - (CURSOR_SIZE >> 1);

    // (CURSOR_SIZE << 1) leaves buffer for user at the previous map location
    if (yeg.cursorX == (CURSOR_SIZE >> 1) && scrnX != 0) {
        // left side of screen reached
        lcdYegDraw(leftShift, irow - yeg.cursorY, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        helperMove(&yeg.shiftX, "left");
    } else if (yeg.cursorY == (CURSOR_SIZE >> 1) && scrnY != 0) {
        // top of screen reached
        lcdYegDraw(icol - yeg.cursorX, upShift, 0, 0, MAP_DISP_WIDTH,
            MAP_DISP_HEIGHT);
        helperMove(&yeg.shiftY, "up");
    } else if (yeg.cursorX == MAP_DISP_WIDTH - (CURSOR_SIZE >> 1) - PAD
        && scrnX != SHIFTED_BORDER) {
            // right side of sign reached
            lcdYegDraw(rightShift, irow - yeg.cursorY, 0, 0, MAP_DISP_WIDTH,
                MAP_DISP_HEIGHT);
            helperMove(&yeg.shiftX, "right");
    } else if (yeg.cursorY == MAP_DISP_HEIGHT - (CURSOR_SIZE >> 1) - PAD
        && scrnY != SHIFTED_BORDER) {
            // bottom of screen reached
            lcdYegDraw(icol - yeg.cursorX, downShift, 0, 0,
                MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
            helperMove(&yeg.shiftY, "down");
    }
}


/*
    Description: redraws the map of Edmonton centered as much as possible over
    the selected restaurant.

    Arguments:
        selection (uint16_t): the index of the selected restaurant.
*/
void redrawOverRest(uint16_t selection) {
    // storing latitude and longitude info for the selected restaurant
    restaurant temp;
    getRestaurantFast(cache.REST_DIST[selection].index, &temp);

    // getting and constraining the x and y coordinates of the restaurant to the
    // map dimensions
    int16_t xPos = lon_to_x(temp.lon);
    int16_t yPos = lat_to_y(temp.lat);
    xPos = constrain(xPos, 0, YEG_SIZE);
    yPos = constrain(yPos, 0, YEG_SIZE);

    // x corner of patch
    int16_t xEdge = xPos - (MAP_DISP_WIDTH >> 1);
    // y corner of patch
    int16_t yEdge = yPos - (MAP_DISP_HEIGHT >> 1);
    // keeping corners of patch within bounds of map
    xEdge = constrain(xEdge, 0, YEG_SIZE - MAP_DISP_WIDTH);
    yEdge = constrain(yEdge, 0, YEG_SIZE - MAP_DISP_HEIGHT);

    // drawing the map and resetting cursor and shift positions
    lcdYegDraw(xEdge, yEdge, 0, 0, MAP_DISP_WIDTH, MAP_DISP_HEIGHT);
    yeg.cursorX = xPos - xEdge;
    yeg.cursorY = yPos - yEdge;
    constrainCursor(&yeg.cursorX, &yeg.cursorY);
    redrawCursor(TFT_RED);
    yeg.shiftX = xEdge - YEG_MIDDLE_X;
    yeg.shiftY = yEdge - YEG_MIDDLE_Y;
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
    int diffX = yeg.cursorX - cursorX0;
    int diffY = yeg.cursorY - cursorY0;
    // storing appropriate irow and icol positions
    int icolPos = YEG_MIDDLE_X + cursorX0 + (CURSOR_SIZE >> 1)
        + diffX + yeg.shiftX;
    int icolNeg = YEG_MIDDLE_X + cursorX0 - (CURSOR_SIZE >> 1) + yeg.shiftX;
    int irowPos = YEG_MIDDLE_Y + cursorY0 + (CURSOR_SIZE >> 1)
        + diffY + yeg.shiftY;
    int irowNeg = YEG_MIDDLE_Y + cursorY0 - (CURSOR_SIZE >> 1) + yeg.shiftY;
    // storing appropriate srow and scol positions
    int scolPos = yeg.cursorX + (CURSOR_SIZE >> 1);
    int scolNeg = cursorX0 - (CURSOR_SIZE >> 1);
    int srowPos = yeg.cursorY + (CURSOR_SIZE >> 1);
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
    Description: helper function for redrawMap() which aids with map boundary
    clamping and map shifting.

    Arguments:
        shiftLen (int*): the X or Y shift to store for future map shifts.
        mainDir (const char*): user passes in the parameter of movement.
*/
void helperMove(int* shiftLen, const char* mainDir) {
    if (strcmp(mainDir, "left") == 0) {
        *shiftLen -= MAP_DISP_WIDTH;
        yeg.cursorX += MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "right") == 0) {
        *shiftLen += MAP_DISP_WIDTH;
        yeg.cursorX -= MAP_DISP_WIDTH - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "up") == 0) {
        *shiftLen -= MAP_DISP_HEIGHT;
        yeg.cursorY += MAP_DISP_HEIGHT - (CURSOR_SIZE << 1);
    } else if (strcmp(mainDir, "down") == 0) {
        *shiftLen += MAP_DISP_HEIGHT;
        yeg.cursorY -= MAP_DISP_HEIGHT - (CURSOR_SIZE << 1);
    }
}


/*
    Description: draws a square cursor at the current cursor position.

    Arguments:
        colour (uint16_t): the colour of the cursor.
*/
void redrawCursor(uint16_t colour) {
    tft.fillRect(yeg.cursorX - (CURSOR_SIZE >> 1),
        yeg.cursorY - (CURSOR_SIZE >> 1), CURSOR_SIZE, CURSOR_SIZE, colour);
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
    Description: constrains the global shift in X and Y position to within the
    physical boundaries of the YEG map.

    Arguments:
        shiftX (int*): pointer to the X shift.
        shiftY (int*): pointer to the Y shift.
*/
void constrainMap(int* shiftX, int* shiftY) {
    *shiftX = constrain(*shiftX, - YEG_MIDDLE_X,
        YEG_SIZE - YEG_MIDDLE_X - MAP_DISP_WIDTH);
    *shiftY = constrain(*shiftY, - YEG_MIDDLE_Y,
        YEG_SIZE - YEG_MIDDLE_Y - MAP_DISP_HEIGHT);
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
