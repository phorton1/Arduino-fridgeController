#pragma once

#include "fridge.h"

#define PIXEL_STATE     1
#define PIXEL_SYSTEM    0

#define NUM_PIXELS		2


//----------------------------------
// available LED colors
//----------------------------------

#define MY_LED_BLACK    0x000000
#define MY_LED_RED      0x440000
#define MY_LED_GREEN    0x003300
#define MY_LED_BLUE     0x000044
#define MY_LED_CYAN     0x003322
#define MY_LED_YELLOW   0x333300
#define MY_LED_MAGENTA  0x440044
#define MY_LED_WHITE    0x444444
#define MY_LED_ORANGE   0x402200
#define MY_LED_REDMAG   0x400022
#define MY_LED_PURPLE   0x220022
#define MY_LED_BLUECYAN 0x002240

// externs

extern void showPixels();
extern void clearPixels();
extern void setPixelsBrightness(int val);
extern void setPixel(int num, uint32_t color);
