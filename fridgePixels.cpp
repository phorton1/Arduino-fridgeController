// fridgePixels.cpp
//
// Native pixel brightness is weird!!  0==max, 1=min, 255=max-1
// I change it so that 0=min, and 254=max-1 and never send 0

#include "fridgePixels.h"
#include <Adafruit_NeoPixel.h>


static Adafruit_NeoPixel pixels(NUM_PIXELS,PIN_EXT_LED);


//-----------------------------
// pass thrus
//-----------------------------


void showPixels()
{
	pixels.show();
}

void clearPixels()
{
	pixels.clear();
}


void setPixelsBrightness(int val)
{
	pixels.setBrightness(val);
}

void setPixel(int num, uint32_t color)
{
	pixels.setPixelColor(num,color);
}
