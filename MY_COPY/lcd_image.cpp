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
*/

#include "Adafruit_GFX.h"   // Core graphics library
#include "MCUFRIEND_kbv.h"  // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "include/lcd_image.h"


/*
    Description: draws the referenced image to the LCD screen.

    Arguments:
        img (const lcd_image_t): the image to draw
        tft (MCUFRIEND_kbv*): the initialized tft struct
        icol, irow (uint16_t): the upper-left corner of the image patch to draw
        scol, srow (uint16_t): the upper-left corner of the screen to draw to
        width, height (uint16_t): controls the size of the patch drawn.
*/
void lcd_image_draw(
    const lcd_image_t *img, MCUFRIEND_kbv *tft,
    uint16_t icol, uint16_t irow,
    uint16_t scol, uint16_t srow,
    uint16_t width, uint16_t height
) {
    File file;

    // Open requested file on SD card if not already open
    if ((file = SD.open(img->file_name)) == NULL) {
        Serial.print("File not found:'");
        Serial.print(img->file_name);
        Serial.println('\'');
        return;
    }

    for (uint16_t row=0; row < height; row++) {
        uint16_t pixels[width];
        // Seek to start of pixels to read from, need 32 bit arithmetic for big
        // images
        uint32_t pos = ( (uint32_t) irow +  (uint32_t) row)
            * (2 *  (uint32_t) img->ncols) +  (uint32_t) icol * 2;
        file.seek(pos);

        // Read row of pixels
        if (file.read(reinterpret_cast<uint8_t *>(pixels),
            2 * width) != 2 * width) {
                Serial.println("SD Card Read Error!");
                file.close();
                return;
        }

        tft->startWrite();
        // Setup display to receive window of pixels
        tft->setAddrWindow(scol, srow+row, scol+width-1, srow+row);

        // Send pixels to display
        for (uint16_t col=0; col < width; col++) {
            uint16_t pixel = pixels[col];

            // pixel bytes in reverse order on card (swap the most
            // significant byte with the least significant byte)
            pixel = (pixel << 8) | (pixel >> 8);

            // tft->writePixel(scol+col, srow+row, pixel);
            pixels[col] = pixel;
        }
        tft->pushColors(pixels, width, true);
        tft->endWrite();
    }
    file.close();
}
