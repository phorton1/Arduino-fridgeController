
#include "fridge.h"
#include <myIOTLog.h>


//--------------------------------
// main
//--------------------------------


void setup()
{
    Serial.begin(921600);
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



//---------------------------------------
// temporary location
//---------------------------------------




//==================================================
// extern'd utilities
//==================================================

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
