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
#if WITH_FAKE_COMPRESSOR
	#include "fakeCompressor.h"
#endif

// structure

#define SAMPLE_PLUS		0
#define SAMPLE_FAN		1
#define SAMPLE_DIODE	2
#define SAMPLE_5V		3

#define THRESHOLD_PLUS_ON	1600
#define THRESHOLD_FAN_ON	1600
#define THRESHOLD_DIODE_ON	1600

#define FLASH_COUNT_TIME	3600	// ms for counting flashes
#define FLASH_CLEAR_TIME	5200	// no flash indicates a restart attempt after this long

#define FLASH_LEDS_AT_STARTUP	1

//---------------------------------------------
// implementation
//---------------------------------------------

#define NUM_PINS			4
#define NUM_PIN_SAMPLES	 	5

static int sample_circ_buffer[NUM_PINS][NUM_PIN_SAMPLES];
static int cur_sample_circ_idx;
static int sample_sum[NUM_PINS];
static int sample_val[NUM_PINS];

// the voltages are only updated every 100,s
// through a separate 10 sample circular buffer

#define VOLTAGE_INV			0
#define VOLTAGE_5V			1

#define VOLTAGE_INTERVAL	100
#define NUM_VOLTAGES		2
#define NUM_VOLT_SAMPLES	10

bool volt_circ_started = 0;
static float volt_circ_buf[NUM_VOLTAGES][NUM_VOLT_SAMPLES];
static int cur_volt_circ_idx;


vSense v_sense;


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



#if 1
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
	// sample a pin and add its value to the circular buffer
{
	sample_sum[i] -= sample_circ_buffer[i][cur_sample_circ_idx];
	int v = analogRead(pin);
	sample_circ_buffer[i][cur_sample_circ_idx] = v;
	sample_sum[i] += v;

	// YIKES - I'm not even using the circular buffer and don't remember
	// where I left it vis-a-vis testing with real inverter!
	sample_val[i] = analogRead(pin);	// sample_sum[i] / NUM_PIN_SAMPLES;
}


static void addVoltSample(int idx, float volts)
	// add a calculataed voltage to the voltage circular buffer
	// the first time we will the buffer with whatever value
{
	if (!volt_circ_started)
	{
		for (int i=0; i<NUM_VOLT_SAMPLES; i++)
		{
			volt_circ_buf[idx][i] = volts;
		}
	}
	else
	{
		volt_circ_buf[idx][cur_volt_circ_idx] = volts;
	}
}


static float getVoltValue(int idx)
	// return the average of the voltage circular buffer
	// to 1 decimal place
{
	float val = 0;
	for (int i=0; i<NUM_VOLT_SAMPLES; i++)
	{
		val += volt_circ_buf[idx][i];
	}
	val /= NUM_VOLT_SAMPLES;
	val *= 10;
	int i_val = val;
	return ((float) i_val)/10;
}




void vSense::sense()
	// important to only call every 50ms or so
	// because this smooths the sensing of inputs
{

	uint32_t now = millis();
	// static uint32_t last_read = 0;
	// if (now - last_read < 50) return;
	// last_read = now;

	#define TEST_VALUES 0

	#if TEST_VALUES

		if (1)
		{
			static int test_values[NUM_PINS] = {20,20,20};
			for (int i=0; i<NUM_PINS; i++)
			{
				// for testing plotter we allow negative numbers
				// and can try really small numbers
				test_values[i] += random(5) - 2;

				if (test_values[i] >= 4095)
					test_values[i] = 4095;
				if (test_values[i] < -4096)
					test_values[i] = -4096;
				sample_val[i] = test_values[i];
			}
		}
		else

	#elif WITH_FAKE_COMPRESSOR

		if (fakeCompressor::_use_fake)
		{
			sample_val[SAMPLE_PLUS] = fakeCompressor::g_sample_plus;
			sample_val[SAMPLE_FAN] = fakeCompressor::g_sample_fan;
			sample_val[SAMPLE_DIODE] = fakeCompressor::g_sample_diode;
			sample_val[SAMPLE_5V] = fakeCompressor::g_sample_5V;
		}
		else
		
	#endif
	{
		doRead(SAMPLE_PLUS,PIN_S_PLUS);
		doRead(SAMPLE_FAN,PIN_S_FAN);
		doRead(SAMPLE_DIODE,PIN_S_DIODE);
		doRead(SAMPLE_5V,PIN_S_5V);
		
		cur_sample_circ_idx++;
		if (cur_sample_circ_idx >= NUM_PIN_SAMPLES)
			cur_sample_circ_idx = 0;
	}


	// every 100 ms add the voltages calculated from
	// the samples to the voltage circular buffer and
	// set the public variables to the average
	// which is rounded to 1 decimal place by getVoltValue
	// and adjusted herein by the calibration values

	static uint32_t last_volt_sample;
	if (now - last_volt_sample > VOLTAGE_INTERVAL)
	{
		last_volt_sample = now;

		#define CALIB_12V		(12.0 / 2.2)
		#define CALIB_5V		(5.0 / 1.6)
			// initial calibration constants determined
			// by measurments after readToVolts() returned
			// 2.2 and 1.6 respectivly

		// LOGD("sample 12V(%d) 5V(%d)",sample_val[SAMPLE_PLUS],sample_val[SAMPLE_5V]);
		float volt_inv = readToVolts(sample_val[SAMPLE_PLUS]) * CALIB_12V;
			// VOLTS_PLUS(sample_val[SAMPLE_PLUS]);
		float volt_5V = readToVolts(sample_val[SAMPLE_5V]) * CALIB_5V;
			// VOLTS_5V(sample_val[SAMPLE_5V]);

		addVoltSample(VOLTAGE_INV,volt_inv);
		addVoltSample(VOLTAGE_5V,volt_5V);
		_volts_inv = getVoltValue(VOLTAGE_INV) * fridge->_calib_volts_inv;
		_volts_5V = getVoltValue(VOLTAGE_5V) * fridge->_calib_volts_5v;

		cur_volt_circ_idx = (cur_volt_circ_idx+1) % NUM_VOLT_SAMPLES;
		volt_circ_started = 1;
	}


	#if WITH_WS

		if (fridge->_plot_data)
		{
			// we plot the inverter instantaneous voltages,
			// but not the 5V power supply
			
			static char plot_buf[120];
			sprintf(plot_buf,"{\"plot_data\":[%d,%d,%d]}",
				sample_val[0],
				sample_val[1],
				sample_val[2]);

			// LOGD("Sending plot_buf(%s)",plot_buf);
			// I am concerned with broadcasting to WS
			// in the time-critical Fridge::stateMachine(),
			// much as I am concerned about data logging
			// from that task.
			fridge->wsBroadcast(plot_buf);
		}

	#endif	// WITH_WS
	
	static int flash_count;

	static uint32_t cycle_start;
	static uint32_t count_start;
	static uint32_t prev_cycle;

	bool p_on = sample_val[SAMPLE_PLUS] > THRESHOLD_PLUS_ON;
	bool f_on = sample_val[SAMPLE_FAN] < THRESHOLD_FAN_ON;
	bool d_on = sample_val[SAMPLE_DIODE] > THRESHOLD_DIODE_ON;

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

	if (_diag_on != d_on)
	{
		_diag_on = d_on;
		digitalWrite(LED_DIODE_ON,_diag_on);

		// if the diode goes on, see if a count already started
		// and if not, start one

		if (_diag_on)
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
			LOGV("DIODE %s count=%d",_diag_on?"ON":"OFF",flash_count);
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

	// The blue diode is controlled by Fridge::setRPM()

}