#include <myDebug.h>
#include <esp_adc_cal.h>

#define PLOT_VALUES 	0
#define DBG_DIODE		(PLOT_VALUES + 1)

#define LED_COMPRESS_ON	2		// "pump" is on when the is on
#define LED_DIODE_ON 	27		// follows the controller "DIODE"

#define PIN_FAN_PLUS	35		
#define PIN_FAN_MINUS	34
#define PIN_DIODE_MINUS	39

#define SAMPLE_PLUS		0
#define SAMPLE_FAN		1
#define SAMPLE_DIODE	2

#define THRESHOLD_PLUS_ON	1600
#define THRESHOLD_FAN_ON	1600
#define THRESHOLD_DIODE_ON	1600

#define FLASH_COUNT_TIME	3600	// ms for counting flashes
#define FLASH_CLEAR_TIME	5200	// no flash indicates a restart attempt after this long


#define NUM_SAMPLES	 5

int circ[3][NUM_SAMPLES];
int ptr;
int sum[3];
int val[3];


#define PWM_CHANNEL		0
#define PWM_FREQ		5000
#define PWM_RESOLUTION	8
#define PWM_PIN			15

#define TEST_SWITCH		16
#define DO_TEST_SEQUENCE	0


void setup(void)
{
    Serial.begin(PLOT_VALUES?115200:921600);
    delay(1000);
    display(PLOT_VALUES,"fridgeController",0);
	pinMode(PIN_FAN_PLUS,INPUT_PULLUP);
	pinMode(PIN_FAN_MINUS,INPUT_PULLUP);
	pinMode(PIN_DIODE_MINUS,INPUT_PULLUP);
	pinMode(LED_COMPRESS_ON,OUTPUT);
	pinMode(LED_DIODE_ON,OUTPUT);
	digitalWrite(LED_COMPRESS_ON,0);
	digitalWrite(LED_DIODE_ON,0);

	pinMode(TEST_SWITCH,INPUT_PULLUP);
	ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	ledcAttachPin(PWM_PIN, PWM_CHANNEL);
	ledcWrite(PWM_CHANNEL, 0);
}


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








void doRead(int i, int pin)
{
	sum[i] -= circ[i][ptr];
	int v = analogRead(pin);
	circ[i][ptr] = v;
	sum[i] += v;
	val[i] = analogRead(pin);	// sum[i] / NUM_SAMPLES;
}



//----------------------------------------
// Compressor Speed
//----------------------------------------

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

	display(0,"rpmToDuty(%d)  desired_ma(%0.3f)  duty(%d)",rpm,desired_ma,duty);
	return duty;
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
		doRead(SAMPLE_PLUS,PIN_FAN_PLUS);
		doRead(SAMPLE_FAN,PIN_FAN_MINUS);
		doRead(SAMPLE_DIODE,PIN_DIODE_MINUS);
		ptr++;
		if (ptr >= NUM_SAMPLES)
			ptr = 0;

		#if PLOT_VALUES
			Serial.print(val[0]);
			Serial.print(",");
			Serial.print(val[1]);
			Serial.print(",");
			Serial.print(val[2]);
			Serial.println();
		#endif
		
		static bool plus_on;
		static bool fan_on;
		static bool diode_on;

		static int error_code;
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
			error_code = 0;
			flash_count = 0;
			cycle_start = 0;
			count_start = 0;
			prev_cycle = 0;
		}

		// power on/off is always noted

		if (plus_on != p_on)
		{
			plus_on = p_on;
			display(PLOT_VALUES,"POWER %s",plus_on?"ON":"OFF");
		}

		// note fan change if power is on

		if (fan_on != f_on)
		{
			fan_on = f_on;
			display(PLOT_VALUES,"FAN %s",fan_on?"ON":"OFF");
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
						display(DBG_DIODE,"CYCLE_START prev_dur=%d",dur);
					}
					prev_cycle = now;
				}

				// increment the flash count

				flash_count++;
				display(DBG_DIODE,"DIODE %s count=%d",diode_on?"ON":"OFF",flash_count);
			}
		}

		// now, within a cycle, if there any flashes,
		// we check if the window for counting flashes has passed,
		// and if so, we move the flash_count to the error_code
		// and note any changes in the error code.

		if (count_start && flash_count && now - count_start > FLASH_COUNT_TIME)
		{
			count_start = 0;
			if (error_code != flash_count)
			{
				error_code = flash_count;
				display(PLOT_VALUES,"ERROR(%d)",error_code);
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
			error_code = 0;
			display(PLOT_VALUES,"RESTART CLEARING ERROR(%d)",error_code);
		}

		// finally, we empirically determine that the
		// compressor is running if the fan is running
		// and there are no errorw.

		static bool compress_on;
		bool c_on = fan_on && !error_code;
		if (compress_on != c_on)
		{
			compress_on = c_on;
			digitalWrite(LED_COMPRESS_ON,compress_on);
		}


		//--------------------------------------
		// TEST COMPRESSOR SPEED
		//--------------------------------------
		
		static bool test_sw;
		bool sw = !digitalRead(TEST_SWITCH);

		static bool test_running;
		static int  desired_rpm;
		static uint32_t last_test_time;

		if (test_sw != sw)
		{
			test_sw = sw;

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

		// Interesting
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
	}
}