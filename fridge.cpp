//-----------------------------------------------------------
// fridge.cpp
//-----------------------------------------------------------

#include "fridge.h"
#include "vSense.h"
#include "tSense.h"
#include "uiScreen.h"
#include "uiButtons.h"
#include <myIOTLog.h>

#define WITH_TSENSE     0
#define WITH_PWM        0

#define TEST_COMPRESSOR_SPEED	0


#define TEMP_INTERVAL		3000
#define MAX_TEMP_POINTS		(24 * 60 * 2)	// 11.5K = 24 hours at 30 seconds,  2.4 hours at 3 seconds


float plot_temps[MAX_TEMP_POINTS];
int plot_head = 0;
bool plot_circ = 0;

static void addPlotTemp(float temp)
{
	plot_temps[plot_head++] = temp;
	if (plot_head >= MAX_TEMP_POINTS)
	{
		plot_head = 0;
		plot_circ = 1;
	}
}



//------------------------------
// myIOT setup
//------------------------------

static valueIdType dash_items[] = {
	ID_TEMPERATURE_1,
	ID_TEMP_ERROR,
	ID_INV_ERROR,
	ID_INV_PLUS,
	ID_INV_FAN,
	ID_INV_COMPRESS,
    0
};

static valueIdType config_items[] = {
    0
};


const valDescriptor Fridge::m_fridge_values[] =
{
    {ID_DEVICE_NAME,	VALUE_TYPE_STRING,   VALUE_STORE_PREF,     VALUE_STYLE_REQUIRED,   NULL,   	NULL,   FRIDGE_CONTROLLER },        // override base class element

	{ID_TEMPERATURE_1,	VALUE_TYPE_FLOAT,    VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_temperature1,	NULL,	{ .float_range	= {0,	-200,	2000}},	},
	{ID_TEMP_ERROR,		VALUE_TYPE_INT,      VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_temp_error,      NULL,	{ .int_range	= {0,	0,		100}},	},
	{ID_INV_ERROR,		VALUE_TYPE_INT,      VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_error,       NULL,	{ .int_range	= {0,	0,		100}},	},
	{ID_INV_PLUS,		VALUE_TYPE_INT,      VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_plus,        NULL,	{ .int_range	= {0,	0,		1}},	},
	{ID_INV_FAN,		VALUE_TYPE_INT,      VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_fan,         NULL,	{ .int_range	= {0,	0,		1}},	},
	{ID_INV_COMPRESS,	VALUE_TYPE_INT,      VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_compress,    NULL,	{ .int_range	= {0,	0,		1}},	},

};

#define NUM_FRIDGE_VALUES (sizeof(m_fridge_values)/sizeof(valDescriptor))


// static member variable declarations

float	Fridge::_temperature1;
int		Fridge::_temp_error;

int		Fridge::_inv_error;
bool 	Fridge::_inv_plus;
bool	Fridge::_inv_fan;
bool	Fridge::_inv_compress;


Fridge *fridge;


//------------------------------
// Application Vars
//------------------------------

#define PWM_CHANNEL		0
#define PWM_FREQ		5000
#define PWM_RESOLUTION	8

OneWire one_wire(PIN_ONE_WIRE);

tSense t_sense(&one_wire);
vSense v_sense;
uiScreen  ui_screen;
uiButtons ui_buttons(PIN_BUTTON1,PIN_BUTTON2,PIN_BUTTON3);

float degreesF;


//=========================================================
// implementation
//=========================================================

Fridge::Fridge()
{
    fridge = this;
    addValues(m_fridge_values,NUM_FRIDGE_VALUES);
    setTabLayouts(dash_items,config_items);
}


void Fridge::setup()
{
    LOGD("Fridge::setup(%s) started",getVersion());
    proc_entry();

	ui_screen.init();

#if WITH_PWM
	ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	ledcAttachPin(PIN_PUMP_PWM, PWM_CHANNEL);
	ledcWrite(PWM_CHANNEL, 0);
#endif

	v_sense.init();
#if WITH_TSENSE
	int err = t_sense.init();
		// we ignore errors but they may be reported
#endif
	ui_buttons.init();

	myIOTDevice::setup();

	LOGI("starting stateTask");
    xTaskCreatePinnedToCore(stateTask,
        "stateTask",
        8192,           // task stack
        NULL,           // param
        5,  	        // note that the priority is higher than one
        NULL,           // returned task handle
        ESP32_CORE_OTHER);

    proc_leave();
    LOGD("Fridge::setup(%s) completed",getVersion());
}



void Fridge::stateTask(void *param)
{
    delay(1200);
    LOGI("starting stateTask loop on core(%d)",xPortGetCoreID());
    delay(1200);
    while (1)
    {
		// vTaskDelay(1000 / portTICK_PERIOD_MS);
		vTaskDelay(1);	// 10 ms?
        fridge->stateMachine();
    }
}



//=========================================================
// stateMachine()
//=========================================================

void Fridge::stateMachine()
{
	uint32_t now = millis();

	//--------------------------------
	// voltage (inverter) sensors
	//--------------------------------

	static uint32_t last_vsense = 0;
	if (now - last_vsense >= 50)
	{
		last_vsense = now;
		v_sense.sense();

	}

	//---------------------------
	// temperature sensors
	//---------------------------

#if WITH_TSENSE
	static uint32_t last_tsense;
	if (!t_sense.pending() && now - last_tsense > TEMP_INTERVAL)
	{
		last_tsense = now;
		degreesF = t_sense.getDegreesF(MY_TSENSOR_01);
			// Takes 13.3 ms with default 12 bit resolution!!
			// Could read just 2 bytes to get it down to about 2 ms
		if (degreesF < TEMPERATURE_ERROR)
		{
			LOGI("TSENSOR_01 degreesF=%0.3fF",degreesF);
			addPlotTemp(degreesF);
			// if TEMPERATURE_ERROR *could* check getLastError();
		}
		int err = t_sense.measure();
			// takes a a little over 2ms
			// error already reported, but might want to know it for some reason
	}

#endif	// WITH_TSENSE

	//--------------------------------------
	// test compressor speed
	//--------------------------------------

#if TEST_COMPRESSOR_SPEED

	static uint32_t last_test;
	if (now - last_test > 50)
	{
		last_test = now;

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
#endif	// TEST_COMPRESSOR_SPEED

}	// stateMachine()



//=========================================================
// loop()
//=========================================================

void Fridge::loop()
{
	myIOTDevice::loop();

	// handle UI

	ui_buttons.loop();
	

	// publish changed values from sensors

	int i_temp = (degreesF * 10);
	float f_temp = ((float)i_temp) / 10.0;

	if (_temperature1 != f_temp)
		setFloat(ID_TEMPERATURE_1,f_temp);

	int terr = t_sense.getLastError();
	if (_temp_error != terr)
		setInt(ID_TEMP_ERROR,terr);

	if (_inv_error != v_sense._error_code)
		setInt(ID_INV_ERROR,v_sense._error_code);
	if (_inv_plus != v_sense._plus_on)
		setInt(ID_INV_PLUS,v_sense._plus_on);
	if (_inv_fan != v_sense._fan_on)
		setInt(ID_INV_FAN,v_sense._fan_on);
	if (_inv_error != v_sense._compress_on)
		setInt(ID_INV_COMPRESS,v_sense._compress_on);

}



//----------------------------------------
// graphing experiments
//----------------------------------------

// #include <stdio.h>

static String getTemperatureJson()
{
	String rslt = "{";

	struct timeval tv_now;
	gettimeofday(&tv_now, NULL);
	String tm = timeToString(tv_now.tv_sec);	// time(NULL));

	rslt += "\"time\":\"" + tm + "\",";
	rslt += "\"series\":[[";

	int num = 0;
	if (plot_circ)
	{
		int start = plot_head;
		while (start<MAX_TEMP_POINTS)
		{
			if (num)
				rslt += ",";
			rslt += "[" + String(num++) + "," + String(plot_temps[start++]) + "]";
		}
	}

	for (int i=0; i<plot_head; i++)
	{
		if (num)
			rslt += ",";
		rslt += "[" + String(num++) + "," + String(plot_temps[i]) + "]";
	}
	
	rslt += "]]";
	rslt += "}";
	return rslt;
}



String Fridge::onCustomLink(const String &path,  const char **mime_type)
    // called from myIOTHTTP.cpp::handleRequest()
	// for any paths that start with /custom/
{
	if (path.startsWith("temp_data"))
	{
		*mime_type = "application/json";
		return getTemperatureJson();
	}

    return "";
}
