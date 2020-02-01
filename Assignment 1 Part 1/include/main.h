#ifndef _MAIN_H_
#define _MAIN_H_

// ================================= CONSTANTS =================================
// pins to connect
#define SD_CS 10
#define JOY_VERT A9
#define JOY_HORIZ A8
#define JOY_SEL 53

// screen and map dimensions
#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

// joystick movement
#define JOY_CENTER 512
#define JOY_DEADZONE 64

// cursor size
#define CURSOR_SIZE 9


// =========================== FUNCTION DECLARATIONS ===========================
void setup();
void lcd_setup();
void redrawCursor(uint16_t);
void processJoystick(uint8_t, uint8_t);
void drawMapPatch(int, int);
void constrainCursor(int*, int*);
void lcdYegDraw(int, int, int, int, int, int);

#endif
