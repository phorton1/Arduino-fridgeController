
#include "fridge.h"
#include <myIOTLog.h>

//----------------------------------------------------
// ESP32 issues - for lack of a better place
//----------------------------------------------------
// In the middle of development I got a bunch of ESP32-S3-ZERO boards,
// and wanted to try them.  Got totally sidetracked into updating
// from ESP32 Core 1_0_6 to most recent 3_0_4 after I discovered that
// my Arduino IDE 1.8.13 did not support the S3 board.
//
// First installing Arduino IDE 2.3.2 on a new laptop, and the 3_0_4
// core, I was able to get an LED to blink.  Lots of work to figure
// out how to get a myIOT test program to run on the little board,
// before finally getting to uploading /data to the SPIFFS.
// The SPIFFS upload tool is not supported in Arduino 2.0, so
// I tried it from IDE 1.18.3, but of course, the old esp32tool
// doesn't know about the S3.  Modified my "upload_spiffs.pm" script
// and got it to work, but not before wiping out my /Users/Patrick
// AppData/Local/Arduino15 esp32 stuff.  So an emergency restore of THAT
// from a hard disk archive, and then installing 3_0_4 over that,
// and jiggling around, I got the test app to run on the S3, BUT
// it is not a reliable little platform and I could never get it
// to fully work.  I digressed to a WebServer only test program
// and had to mess with serial ports, and many things before giving
// up in exhaustion at 4:00am.
//
// Today I started by carefully identifying the changes to myIOT
// and ESP32SSDP that I had to make tot get it to compile, and
// digging up some compile defines that I could drive the build
// with. Got it all nice and then built the fridgeController
// with 3_0_4.  Builds fine; runs not so good.
//
// Apparently there are more Serial Port kludges in 3_0_4, and
// right now I am struggling to even debug fridgeController because
// the serial port output is gobbledy-gook, on top of which I'm
// getting a boot time error:
//
//	E (131) esp_co?fx_dump_flash: Incorrect size of core dump image: 6619237
//
// At least I verified that I can go back to 1_0_6 and the fridgeController
// builds and seems to work as before.  In /Users/Patrick/AppData/Local
// there are two switchable copies of Arduino15 ... one is 1_0_6 and
// the other is 3_0_4 and the one not in use is named appropriately.
// There are also backup copies of each in /junk/maybe_save.
//
// As a result, there are also multiple copies of preferences.txt
// checked in and I have added an Arduino/_esp32  'test' directory
// to localize the many test programs written in this foray.
//
// At this point, I cannot conclusively say it is a good idea to go
// to 3_0_4.  It is definitely a bad idea to go to Arduino IDE 2, or
// I risk not only all the ESP32 stuff (myIOT and FluidNC), but also
// all the teensy stuff, which is already in flux.
//
// Compiling cnc_3018 against 3_0_4 was even worse.  Starting with
// src/StackTrace/debug_helpers.cpp, which failed on, but does not need
//			#    include "soc/cpu.h"
// there are problems with basic ESP32 includes, that exist, but are not
// compiling correctly in-and-around "assert()" and a failure to include
// <cassert>, as well as Strings and flash memory:
//
// - src\spindles\spindle.h:83:38: error: cannot bind non-const lvalue reference of type 'IPAddress&' to an rvalue of type 'IPAddress'
// - src\spindles\spindle.h:83:38: error: invalid conversion from 'int' to 'const __FlashStringHelper*' [-fpermissive]
// - rc/Machine/Homing.h:43:25: error: no matching function for call to 'item(const char [6], int&)
//
// and many, many, other problems.



//--------------------------------
// main
//--------------------------------


void setup()
{
    Serial.begin(MY_IOT_ESP32_CORE == 3 ? 115200 : 921600);
    delay(1000);

    Fridge::setDeviceType(FRIDGE_CONTROLLER);
    Fridge::setDeviceVersion(FRIDGE_CONTROLLER_VERSION);
    Fridge::setDeviceUrl(FRIDGE_CONTROLLER_URL);

    LOGU("");
    LOGU("fridgeController.ino setup() started on core(%d)",xPortGetCoreID());

    fridge = new Fridge();
    fridge->setup();

    LOGU("fridgeController.ino setup() finished",0);
}



void loop()
{
    fridge->loop();
}



//==================================================
// extern'd utilities
//==================================================
// temporary location

int rpmToDuty(int rpm)
	// Great news!  The compressor speed can be more precisely
	// and flexibly controlled by PWM instead of a using bulky
	// relays and fixed resistors or complicated digital pots.
	//
	// Assuming the documentation is accurate and the compressor
	// speed is linear between the given currents, we can now
	// map a compressor speed, in RPM, to a PWM duty cycle.
	//
	// This scaling algorithm gives pretty good results
	// across the 2ma to 5ma range.  Intermediate (rpm)
	// values are presumed.
	//
	// 		     		doc			initial testing		this method
	// 		desired_ma	rpm			duty	actual_ma	duty	ma
	//								255		5.07
	// 		5.0			2000		244		5.00		244		4.99
	// 		4.5			(2250)		218		4.59		217		4.56
	// 		4.0			2500		191		4.08		191		4.08
	// 		3.5			(2750)		164		3.56		164		3.56
	// 		3.0			3000		137		3.04		138		3.05
	// 		2.5			(3250)		110		2.50		111		2.52
	// 		2.0			3500		85		1.99		85		2.01
	//
{
	// empirically determined constants

	#define DUTY_5MA	244.0	// corresponds to 5ma == 2000 rpm
	#define DUTY_2MA	85.0	// corresponds to 2ma == 3500 rpm

	if (rpm > 3500)
		rpm = 3500;
	if (rpm < 2000)
		rpm = 2000;

	float delta_rpm = rpm - 2000;					// 0 .. 1500 rpm faster than 2000 rpm
	float delta_ma = (delta_rpm / 1500) * 3.0;		// 0 .. 3.0ma less than 5ma
	float desired_ma = 5.0 - delta_ma;				// for display only
	float duty_delta = (delta_ma / 3.00) * (DUTY_5MA - DUTY_2MA);
	float duty_float = DUTY_5MA - duty_delta;
	int duty = duty_float;

	LOGD("rpmToDuty(%d)  desired_ma(%0.3f)  duty(%d)",rpm,desired_ma,duty);
	return duty;
}
