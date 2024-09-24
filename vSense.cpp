//-----------------------------------------------
// vSense.cpp
//-----------------------------------------------
//
// The compressor will try to start, I think, three times before
// entering an error state with the flashing LED.  If it were that
// the PWM was turned off before the first flash, no error would
// be reported, which would be kind of pointless since we really
// don't know what's happening.
//
// However, normally, the inverter will try to restart the compressor,
// clearing the error and turning the fan off after about 90 seconds
// after a failed startup.
//
// I noticed that if you turn the PWM off after an error, and then
// briefely turn it back on, and then off again, it will clear the
// error state of the compressor, theoretically allowing for a quicker
// restart attempt.


#include "vSense.h"
#include "fridge.h"
#include <myIOTLog.h>

#define PLOT_VALUES  1

// structure

#define SAMPLE_PLUS		0
#define SAMPLE_FAN		1
#define SAMPLE_DIODE	2

#define THRESHOLD_PLUS_ON	1600
#define THRESHOLD_FAN_ON	1600
#define THRESHOLD_DIODE_ON	1600

#define FLASH_COUNT_TIME	3600	// ms for counting flashes
#define FLASH_CLEAR_TIME	5200	// no flash indicates a restart attempt after this long

#define FLASH_LEDS_AT_STARTUP	1

//---------------------------------------------
// implementation
//---------------------------------------------

#define NUM_SAMPLES	 5

static int circ[3][NUM_SAMPLES];
static int ptr;
static int sum[3];
static int val[3];


void vSense::init()
{
	pinMode(PIN_S_PLUS,INPUT_PULLUP);
	pinMode(PIN_S_FAN,INPUT_PULLUP);
	pinMode(PIN_S_DIODE,INPUT_PULLUP);
	pinMode(PIN_S_5V,INPUT_PULLUP);

	pinMode(LED_DIODE_ON,OUTPUT);
	pinMode(LED_POWER_ON,OUTPUT);
	pinMode(LED_COMPRESS_ON,OUTPUT);
	pinMode(LED_FAN_ON,OUTPUT);

#if FLASH_LEDS_AT_STARTUP
	for (int i=0; i<10; i++)
	{
		digitalWrite(LED_POWER_ON, (i%5) == 1);
		digitalWrite(LED_FAN_ON, (i%5) == 2);
		digitalWrite(LED_DIODE_ON, (i%5) == 3);
		digitalWrite(LED_COMPRESS_ON,(i%5) == 4);
		delay(200);
	}
#endif

	digitalWrite(LED_DIODE_ON,0);
	digitalWrite(LED_POWER_ON,0);
	digitalWrite(LED_COMPRESS_ON,0);
	digitalWrite(LED_FAN_ON,0);
}



#if 0
	#include <esp_adc_cal.h>

	float readToVolts(int val)
	{
		esp_adc_cal_characteristics_t adc_chars;
		esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1200, &adc_chars);
			// prh dunno what it does, but it's way better than analogRead()/4096 * 3.3V
			// The 1100 is from https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/
			// The constant does not seem to do anything
		uint32_t raw_millis = esp_adc_cal_raw_to_voltage(val, &adc_chars);
		float raw_volts = (float) raw_millis/1000.0;
		return raw_volts;
	}
#endif





static void doRead(int i, int pin)
{
	sum[i] -= circ[i][ptr];
	int v = analogRead(pin);
	circ[i][ptr] = v;
	sum[i] += v;
	val[i] = analogRead(pin);	// sum[i] / NUM_SAMPLES;
}


static void normalize400(int value, bool comma)
{
	float normal = (value * 400) / 4096;
	String rslt = String(normal);
	if (comma) rslt += ",";

}



void vSense::sense()
	// important to only call every 50ms or so
	// because this smooths the sensing of inputs
{
	uint32_t now = millis();
	// static uint32_t last_read = 0;
	// if (now - last_read < 50) return;
	// last_read = now;

	doRead(SAMPLE_PLUS,PIN_S_PLUS);
	doRead(SAMPLE_FAN,PIN_S_FAN);
	doRead(SAMPLE_DIODE,PIN_S_DIODE);
	ptr++;
	if (ptr >= NUM_SAMPLES)
		ptr = 0;

	#if PLOT_VALUES

		#define TEST_VALUES 1
		#define SMALL_TEST  0

		#if TEST_VALUES
			static float test_values[3] = {20,20,20};
			float use_values[3];

			for (int i=0; i<3; i++)
			{
				// for testing plotter we allow negative numbers
				// and can try really small numbers
				test_values[i] += random(5) - 2;
				
				if (test_values[i] >= 4095)
					test_values[i] = 4095;
				if (test_values[i] < -4096)
					test_values[i] = -4096;
				use_values[i] = test_values[i];
				#if SMALL_TEST
					use_values[i] /= 100000;
				#endif

			}
		#endif

		if (fridge->_plot_data)
		{
			static char plot_buf[120];

			#if TEST_VALUES	// test floats
				sprintf(plot_buf,"{\"plot_data\":[%0.5f,%0.5f,%0.5f]}",
					use_values[0],
					use_values[1],
					use_values[2]);

			#else	// real integers
				sprintf(plot_buf,"{\"plot_data\":[%d,%d,%d]}",
					val[0],
					val[1],
					val[2]);
			#endif

			// LOGD("Sending plot_buf(%s)",plot_buf);

			// I am concerned with broadcasting to WS
			// in the time-critical Fridge::stateMachine(),
			// much as I am concerned about data logging
			// from that task.
			
			fridge->wsBroadcast(plot_buf);
		}

	#endif
	

	static bool diode_on;
	static int flash_count;

	static uint32_t cycle_start;
	static uint32_t count_start;
	static uint32_t prev_cycle;

	bool p_on = val[SAMPLE_PLUS] > THRESHOLD_PLUS_ON;
	bool f_on = val[SAMPLE_FAN] < THRESHOLD_FAN_ON;
	bool d_on = val[SAMPLE_DIODE] > THRESHOLD_DIODE_ON;

	// reset everything if the power is off

	if (!p_on)
	{

		f_on = false;
		d_on = false;
		_error_code = 0;
		flash_count = 0;
		cycle_start = 0;
		count_start = 0;
		prev_cycle = 0;
	}

	// power on/off is always noted

	if (_plus_on != p_on)
	{
		_plus_on = p_on;
		digitalWrite(LED_POWER_ON,_plus_on);
		LOGI("POWER %s",_plus_on?"ON":"OFF");
	}

	// note fan change if power is on

	if (_fan_on != f_on)
	{
		_fan_on = f_on;
		digitalWrite(LED_FAN_ON,_fan_on);
		LOGI("FAN %s",_fan_on?"ON":"OFF");
	}

	// note diode change if power is on

	if (diode_on != d_on)
	{
		diode_on = d_on;
		digitalWrite(LED_DIODE_ON,diode_on);

		// if the diode goes on, see if a count already started
		// and if not, start one

		if (diode_on)
		{
			if (!count_start)
			{
				// the cycle start will count the number of flashes

				cycle_start = now;
				count_start = now;
				flash_count = 0;

				// show the duration since the last cycle if there was one

				if (prev_cycle)
				{
					uint32_t dur = now - prev_cycle;
					LOGV("CYCLE_START prev_dur=%d",dur);
				}
				prev_cycle = now;
			}

			// increment the flash count

			flash_count++;
			LOGV("DIODE %s count=%d",diode_on?"ON":"OFF",flash_count);
		}
	}

	// now, within a cycle, if there any flashes,
	// we check if the window for counting flashes has passed,
	// and if so, we move the flash_count to the error_code
	// and note any changes in the error code.

	if (count_start && flash_count && now - count_start > FLASH_COUNT_TIME)
	{
		count_start = 0;
		if (_error_code != flash_count)
		{
			_error_code = flash_count;
			LOGI("ERROR(%d)",_error_code);
		}
	}

	// if no flashes have occurred, and it has been
	// longer than a certain amoount of time, it indicates a
	// restart attempt, so we clear everything and start over

	else if (flash_count && now - cycle_start > FLASH_CLEAR_TIME)
	{
		cycle_start = 0;
		count_start = 0;
		flash_count = 0;
		_error_code = 0;
		LOGI("RESTART CLEARING ERROR(%d)",_error_code);
	}

	// finally, we empirically determine that the
	// compressor is running if the fan is running
	// and there are no errorw.


	bool c_on = _fan_on && !_error_code;
	if (_compress_on != c_on)
	{
		_compress_on = c_on;
		digitalWrite(LED_COMPRESS_ON,_compress_on);
	}


}