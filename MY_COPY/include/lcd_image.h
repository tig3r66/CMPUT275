//  =======================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  Partner: Jason Kim
//  CMPUT 275, Winter 2020
//
//  Assignment 1 Part 2: Restaurant Finder
//  =======================================

/*
    Routine for drawing an image patch from the SD card to the LCD display.
    Please see lcd_image.cpp for full function documentation.
*/

#ifndef _LCD_IMAGE_H_
#define _LCD_IMAGE_H_

typedef struct {
    char file_name[50];
    uint16_t ncols;
    uint16_t nrows;
} lcd_image_t;


/*
    Description: draws the referenced image to the LCD screen.
*/
void lcd_image_draw(const lcd_image_t *img, MCUFRIEND_kbv *tft,
    uint16_t icol, uint16_t irow,
    uint16_t scol, uint16_t srow,
    uint16_t width, uint16_t height);

#endif
