#ifndef _JOY_CURSOR_H
#define _JOY_CURSOR_H

// pins to connect
#define SD_CS 10
#define JOY_VERT A9
#define JOY_HORIZ A8
#define JOY_SEL 53

// screen and map dimensions
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define PADX 60
#define YEG_SIZE 2048

// joystick movement
#define JOY_CENTER 512
#define JOY_DEADZONE 64

// cursor size
#define CURSOR_SIZE 9

void setup();
void lcd_setup();
void redrawCursor(uint16_t);
void processJoystick(uint8_t, uint8_t);
void drawMapPatch(int, int);
void constrainCursor(int*, int*);
void lcdYegDraw(int, int, int, int, int, int);

#endif
