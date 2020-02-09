//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 1: Restaurant Finder
//  =======================================

/*
    Routine for drawing an image patch from the SD card to the LCD display.
*/

#ifndef _LCD_IMAGE_H
#define _LCD_IMAGE_H


typedef struct {
    char file_name[50];
    uint16_t ncols;
    uint16_t nrows;
} lcd_image_t;


/*
    Description: draws the referenced image to the LCD screen.

    Arguments:
        img (const lcd_image_t): the image to draw
        tft (MCUFRIEND_kbv*): the initialized tft struct
        icol, irow (uint16_t): the upper-left corner of the image patch to draw
        scol, srow (uint16_t): the upper-left corner of the screen to draw to
        width, height (uint16_t): controls the size of the patch drawn.
*/
void lcd_image_draw(const lcd_image_t *img, MCUFRIEND_kbv *tft,
    uint16_t icol, uint16_t irow,
    uint16_t scol, uint16_t srow,
    uint16_t width, uint16_t height);


#endif
