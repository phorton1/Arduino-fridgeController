#include <myDebug.h>
#include "fridge.h"
#include "vSense.h"
#include "tSense.h"


#define PWM_CHANNEL		0
#define PWM_FREQ		5000
#define PWM_RESOLUTION	8


OneWire one_wire(PIN_ONE_WIRE);

tSense t_sense(&one_wire);
vSense v_sense;



void setup(void)
{
    Serial.begin(PLOT_VALUES?115200:921600);
    delay(1000);
    display(PLOT_VALUES,"fridgeController::setup()",0);

	pinMode(PIN_BUTTON1,INPUT_PULLUP);

	ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	ledcAttachPin(PIN_PUMP_PWM, PWM_CHANNEL);
	ledcWrite(PWM_CHANNEL, 0);

	v_sense.init();
	int err = t_sense.init();
		// we ignore errors but they may be reported
}



//----------------------------------
// loop
//----------------------------------

void loop(void)
{
	uint32_t now = millis();

	static uint32_t last_read = 0;
	if (now - last_read >= 50)
	{
		last_read = now;
		v_sense.sense();

		//--------------------------------------
		// TEST COMPRESSOR SPEED
		//--------------------------------------
		
		static bool test_sw;
		bool sw = !digitalRead(PIN_BUTTON1);

		static bool test_running;
		static int  desired_rpm;
		static uint32_t last_test_time;

		if (test_sw != sw)
		{
			test_sw = sw;

			#define DO_TEST_SEQUENCE	0
			#if DO_TEST_SEQUENCE
			
				display(0,"TEST(%s)",test_sw?"STARTING":"OFF");
				desired_rpm = 1750;		// will be incremented at start
				last_test_time = 0;
				test_running = test_sw;
				ledcWrite(PWM_CHANNEL, 0);

			#else	// simply turn the PWM on or off

				display(0,"TURNING_PUMP(%s)",test_sw?"ON":"OFF");
				int duty = test_sw ? rpmToDuty(2000) : 0;
				ledcWrite(PWM_CHANNEL, duty);

			#endif
		}

		if (test_running && now-last_test_time > 10000)
		{
			desired_rpm += 250;
			if (desired_rpm > 3500)
			{
				test_running = 0;
				display(0,"TEST FINISHED!",0);
				ledcWrite(PWM_CHANNEL, 0);
			}
			else
			{
				int duty = rpmToDuty(desired_rpm);
				ledcWrite(PWM_CHANNEL, duty);
				last_test_time = now;
			}
		}
	}



	//---------------------------
	// temperature sensors
	//---------------------------

	#define NUM_ACTIVE  1
	const int ACTIVE_SENSORS[] = {1};
	#define TEMP_READ_INTERVAL	3000
		// SHOULD be well larger than 800ms.
		// MUST be at least larger than 14ms * NUM_ACTIVE
		// or the program will spend all of its time
		// just reading the temperature sensors!
		
	static uint32_t last_sense;
	if (!t_sense.pending() && now - last_sense > TEMP_READ_INTERVAL)
	{
		last_sense = now;
		display(PLOT_VALUES,"temperatures",0);
		proc_entry();

		for (int i=0; i<NUM_ACTIVE; i++)
		{
			int my_num = ACTIVE_SENSORS[i];
			const uint8_t *addr = KNOWN_SENSORS[my_num-1];
				// my KNOWN_SENSOR numbers are ONE based!!

			float degreesF = t_sense.getDegreesF(addr);
				// Takes 13.3 ms with default 12 bit resolution!!
				// Could read be about 2ms if we read just 2 bytes of the scratch pad,
				// and eliminate CRC check (and higher resolution on other models),
				// but I don't care for that approach.
				//
				// Possibly this will run on a separate core, somehow, and/or a task
				// with myIOT and all it's webservers and junk

			if (degreesF < TEMPERATURE_ERROR)
				display(0,"ACTIVE_SENSOR[%d] degreesF=%0.1fF",my_num,degreesF);
				// if TEMPERATURE_ERROR *could* check getLastError();
		}

		int err = t_sense.measure();
			// takes a a little over 2ms
			// error already reported, but might want to know it for some reason

		proc_leave();
	}



}