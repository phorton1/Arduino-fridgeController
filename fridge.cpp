//-----------------------------------------------------------
// fridge.cpp
//-----------------------------------------------------------

#define WITH_DATA_LOG	1
#define WITH_TSENSE     0
#define WITH_PWM        0

#include "fridge.h"
#include "vSense.h"
#include "uiScreen.h"
#include "uiButtons.h"
#include <myIOTLog.h>
#include <myIOTWebServer.h>


#define DEBUG_TSENSE	0


#define TEMP_INTERVAL		3000
#define FLUSH_INTERVAL		20000
#define NUM_SD_RECORDS		10


#if WITH_TSENSE
	#include "tSense.h"
	#define tsense_pending()	t_sense.pending()
	OneWire one_wire(PIN_ONE_WIRE);
	tSense t_sense(&one_wire);
#else
	#define tsense_pending()	0
#endif


//-----------------------------------------------
// data_log setup
//-----------------------------------------------

#if WITH_DATA_LOG

	#include <myIOTDataLog.h>

	typedef struct
	{
		float		temp1;
		float		temp2;
		uint32_t	mech;
		uint32_t	rpm;
	} fridgeLog_t;
	
	// The tick_intervals are 0 based and will be will be lined up

	logColumn_t  fridge_cols[] = {
		{"temp1",	LOG_COL_TYPE_FLOAT,		10,		},	// -40,	40 },
		{"temp2",	LOG_COL_TYPE_FLOAT,		10,		},	// 60,		220 },
		{"mech",	LOG_COL_TYPE_UINT32,	1,		},	// 0,		2 },
		{"rpm",		LOG_COL_TYPE_UINT32,	500,	},	// 0,		4000 },
	};

	myIOTDataLog data_log("fridgeData",4,fridge_cols);	// ,9
	bool data_log_inited = 0;


	// jqplot.enhancedLegendRenderer.js renamed to jqplot.legendRenderer.js
	// because of ESP32 SPIFFS max filename length

	const char *plot_deps = 
		"/myIOT/jquery.jqplot.min.css?cache=1,"
		"/myIOT/jquery.jqplot.min.js?cache=1,"
		"/myIOT/jqplot.dateAxisRenderer.js?cache=1,"
		"/myIOT/jqplot.cursor.js?cache=1,"
		"/myIOT/jqplot.highlighter.js?cache=1,"
		"/myIOT/jqplot.legendRenderer.js?cache=1,"
		"/myIOT/iotChart.js";

	myIOTWidget_t fridgeWidget = {
		"fridgeWidget",
		plot_deps,
		"doChart('fridgeData')",
		"stopChart('fridgeData')",
		NULL };
		

#endif	// WITH_DATA_LOG



//------------------------------
// myIOT setup
//------------------------------

static valueIdType dash_items[] = {
	ID_TEMPERATURE_1,
	ID_TEMPERATURE_2,
	ID_TEMP_ERROR,
	ID_MECH_THERM,
	ID_COMP_RPM,
	ID_INV_ERROR,
	ID_INV_PLUS,
	ID_INV_FAN,
	ID_INV_COMPRESS,
	ID_CHART_LINK,
    0
};


static valueIdType config_items[] = {
    0
};


const valDescriptor Fridge::m_fridge_values[] =
{
    {ID_DEVICE_NAME,	VALUE_TYPE_STRING,  VALUE_STORE_PREF,     VALUE_STYLE_REQUIRED,   NULL,   	NULL,   FRIDGE_CONTROLLER },        // override base class element
	{ID_TEMPERATURE_1,	VALUE_TYPE_FLOAT,   VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_temperature1,	NULL,	{ .float_range	= {0,	-200,	2000}},	},
	{ID_TEMPERATURE_2,	VALUE_TYPE_FLOAT,   VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_temperature2,	NULL,	{ .float_range	= {0,	-200,	2000}},	},
	{ID_TEMP_ERROR,		VALUE_TYPE_INT,     VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_temp_error,      NULL,	{ .int_range	= {0,	0,		100}},	},

	{ID_MECH_THERM,		VALUE_TYPE_BOOL,	VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_mech_therm,		NULL,	},
	{ID_COMP_RPM,		VALUE_TYPE_INT,		VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_comp_rpm,		NULL,	{ .int_range	= {0,	0,		5000}}, },

	{ID_INV_ERROR,		VALUE_TYPE_INT,     VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_error,       NULL,	{ .int_range	= {0,	0,		100}},	},
	{ID_INV_PLUS,		VALUE_TYPE_INT,     VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_plus,        NULL,	{ .int_range	= {0,	0,		1}},	},
	{ID_INV_FAN,		VALUE_TYPE_INT,     VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_fan,         NULL,	{ .int_range	= {0,	0,		1}},	},
	{ID_INV_COMPRESS,	VALUE_TYPE_INT,     VALUE_STORE_TOPIC,		VALUE_STYLE_READONLY,	(void *) &_inv_compress,    NULL,	{ .int_range	= {0,	0,		1}},	},

   { ID_CHART_LINK,		VALUE_TYPE_STRING,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &_chart_link,     },


};

#define NUM_FRIDGE_VALUES (sizeof(m_fridge_values)/sizeof(valDescriptor))


// static member variable declarations

float	Fridge::_temperature1;
float	Fridge::_temperature2;
int		Fridge::_temp_error;

bool	Fridge::_mech_therm;
int		Fridge::_comp_rpm;

int		Fridge::_inv_error;
bool 	Fridge::_inv_plus;
bool	Fridge::_inv_fan;
bool	Fridge::_inv_compress;

String	Fridge::_chart_link;

Fridge *fridge;



//------------------------------
// Application Vars
//------------------------------

#if WITH_PWM
	#define PWM_CHANNEL				0
	#define PWM_FREQ				5000
	#define PWM_RESOLUTION			8
	#define TEST_COMPRESSOR_SPEED	0
#endif


vSense v_sense;
uiScreen  ui_screen;
uiButtons ui_buttons(PIN_BUTTON1,PIN_BUTTON2,PIN_BUTTON3);

float cur_temperature1;
float cur_temperature2;
bool  cur_mech_therm;
int	  cur_comp_rpm;



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
	ui_buttons.init();
	v_sense.init();

#if WITH_PWM
	ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	ledcAttachPin(PIN_PUMP_PWM, PWM_CHANNEL);
	ledcWrite(PWM_CHANNEL, 0);
#endif

#if WITH_TSENSE
	int err = t_sense.init();
#endif

	myIOTDevice::setup();

	if (!getBool(ID_WIFI))
	{
		ui_screen.displayLine(1,"WIFI OFF");
	}
	else
	{
		iotConnectStatus_t status = getConnectStatus();
		if (status == IOT_CONNECT_ALL)
			ui_screen.displayLine(1,"AP_STA MODE");
		else if (status == IOT_CONNECT_AP)
			ui_screen.displayLine(1,"AP_MODE");
		else if (status == IOT_CONNECT_STA)
		{
			String ip = WiFi.localIP().toString();
			ui_screen.displayLine(1,ip.c_str());
		}
		else
			ui_screen.displayLine(1,"WIFI ERROR");
	}

#if WITH_DATA_LOG
	data_log_inited = data_log.init(NUM_SD_RECORDS);
	if (!data_log_inited)
	{
		delay(2000);
		ui_screen.displayLine(0,"dataLog ERROR!!");
	}
	else
	{
		// on the stack
		String html = data_log.getChartHTML(
			300,		// height
			600,		// width
			3600,		// default period for the chart
			0 );		// default refresh interval

		#if 0
			Serial.print("html=");
			Serial.println(html.c_str());
		#endif

		// move to the heap
		fridgeWidget.html = new String(html);
		setDeviceWidget(&fridgeWidget);

		_chart_link = "<a href='/spiffs/temp_chart.html?uuid=";
		_chart_link += getUUID();
		_chart_link += "' target='_blank'>Chart</a>";
	}
#endif

	LOGI("starting stateTask");
    xTaskCreatePinnedToCore(stateTask,
        "stateTask",
        8192,           // task stack
        NULL,           // param
        5,  	        // note that the priority is higher than one
        NULL,           // returned task handle
        ESP32_CORE_OTHER);

    proc_leave();

	randomSeed(time(NULL));

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
	// need sensing of cur_mech_therm and system to setup cur_comp_rpm

	static uint32_t last_tsense;
	if (!tsense_pending() && now - last_tsense > TEMP_INTERVAL)
	{
		last_tsense = now;

		#if WITH_TSENSE

			// Takes 13.3 ms per sensor with default 12 bit resolution!!
			// Could read just 2 bytes to get it down to about 2 ms
			// if TEMPERATURE_ERROR *could* check getLastError();

			float temp1 = t_sense.getDegreesF(MY_TSENSOR_01);
			if (temp1 < TEMPERATURE_ERROR)
				cur_temperature1 = temp1;
			float temp2 = t_sense.getDegreesF(MY_TSENSOR_02);
			if (temp2 < TEMPERATURE_ERROR)
				cur_temperature2 = temp1;

		#else

			// create dummy numbers for testing logging and chart
			// temp1 starts going down and tends around 0
			// temp2 starts going up and tends around 100
			// the mech_therm and comp_rpms come on for a random
			// amount of time around 30 seconds

			#define ON_TEMPERATURE	 -12
			#define OFF_TEMPERATURE  -20
			#define MECH_TEMPERATURE -16

			static bool started = 0;
			static float temp1_delta;
			static float temp2_delta;

			if (!started)
			{
				started = 1;
				cur_temperature1 = -19;
				temp1_delta = 1;

				cur_temperature2 = 100;
				temp2_delta = -1;
			}
			else
			{
				#if DEBUG_TSENSE>1
					LOGD("   delta1=%0.3fF delta2=%0.3fF",temp1_delta,temp2_delta);
				#endif
				
				cur_temperature1 += temp1_delta;
				if (cur_temperature1 > 80)		// ambient
					cur_temperature1 = 80;

				cur_temperature2 += temp2_delta;
				if (cur_temperature2 < 80)		// ambient
					cur_temperature2 = 80;
				if (cur_temperature2 > 212)		// boiling!
					cur_temperature2 = 212;
			}

			if (!cur_comp_rpm && cur_temperature1>ON_TEMPERATURE)
				cur_comp_rpm = 2400;	// better for graphing
			if (cur_comp_rpm && cur_temperature1<OFF_TEMPERATURE)
				cur_comp_rpm = 0;
			cur_mech_therm = cur_temperature1 > MECH_TEMPERATURE ? 1 : 0;

			float tdelta1 = random(100);
			tdelta1 /= 500;		// 0 - 0.20
			tdelta1 += 0.1;		// 0.1 - 0.30
			if (cur_comp_rpm)
				tdelta1 = -tdelta1;
			temp1_delta += tdelta1;
			if (temp1_delta > 2.0)
				temp1_delta = 2.0;
			if (temp1_delta < -2.0)
				temp1_delta = -2.0;

			if (cur_comp_rpm && temp2_delta < 0)
				temp2_delta = 0;
			if (!cur_comp_rpm && temp2_delta > 0)
				temp2_delta = 0;
				
			float tdelta2 = random(100);
			tdelta2 /= 500;		// 0 - 0.20
			tdelta2 += 0.1;		// 0.1 - 0.30
			if (!cur_comp_rpm)
				tdelta2 = -tdelta2;
			temp2_delta += tdelta2;
			if (temp2_delta > 2.0)
				temp2_delta = 2.0;
			if (temp2_delta < -2.0)
				temp2_delta = -2.0;

		#endif	// !WITH_TSENSE

		#if DEBUG_TSENSE
			LOGD("TSENSE temp1=%0.3fF temp2=%0.3fF  mech=%d  rpm=%d",cur_temperature1,cur_temperature2,cur_mech_therm,cur_comp_rpm);
		#endif

		#if WITH_DATA_LOG
			if (data_log_inited)
			{
				fridgeLog_t log_rec;
				log_rec.temp1 = cur_temperature1;
				log_rec.temp2 = cur_temperature2;
				log_rec.mech  = cur_mech_therm;
				log_rec.rpm   = cur_comp_rpm;
				data_log_inited = data_log.addRecord((logRecord_t) &log_rec);

				// turns out that having flush in loop() does
				// not work well with webserver ... it's the opposite
				// of what I would like as I though of loop() as a good
				// place to do slow stuff.  Perhaps it is only that it
				// does CORE(ARDUINO) STUFF ..

				static uint32_t last_flush = 0;
				if (data_log_inited && now-last_flush > FLUSH_INTERVAL)
				{
					last_flush = now;
					data_log_inited = data_log.flushToSD();
				}
			}
		#endif

		#if WITH_TSENSE
			// takes a a little over 2ms
			int err = t_sense.measure();
		#endif
	}


	//--------------------------------------
	// test compressor speed
	//--------------------------------------

#if WITH_PWM && TEST_COMPRESSOR_SPEED

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
	// round temperatures to 0.1 degrees

	int itemp1 = (cur_temperature1 * 10);
	float ftemp1 = ((float)itemp1) / 10.0;
	if (_temperature1 != ftemp1)
		setFloat(ID_TEMPERATURE_1,ftemp1);

	int itemp2 = (cur_temperature2 * 10);
	float ftemp2 = ((float)itemp2) / 10.0;
	if (_temperature2 != ftemp2)
		setFloat(ID_TEMPERATURE_2,ftemp2);

#if WITH_TSENSE
	int terr = t_sense.getLastError();
	if (_temp_error != terr)
		setInt(ID_TEMP_ERROR,terr);
#endif

	if (_mech_therm != cur_mech_therm)
		setBool(ID_MECH_THERM,cur_mech_therm);
	if (_comp_rpm != cur_comp_rpm)
		setInt(ID_COMP_RPM,cur_comp_rpm);

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
// chart API
//----------------------------------------

int getArg(const char *name, int def_value)
{
	String arg = myiot_web_server->arg(name);
	if (arg != "")
		return arg.toInt();
	return def_value;
}


String Fridge::onCustomLink(const String &path,  const char **mime_type)
    // called from myIOTHTTP.cpp::handleRequest()
	// for any paths that start with /custom/
{
	#if WITH_DATA_LOG

		// LOGI("Fridge::onCustomLink(%s)",path.c_str());
		if (path.startsWith("chart_html/fridgeData"))
		{
			// only used by temp_chart.html inasmuch as the
			// chart html is baked into the myIOT widget
			
			int height = getArg("height",400);
			int width  = getArg("width",800);
			int period = getArg("period",3600);
			int refresh = getArg("refresh",0);
			return data_log.getChartHTML(height,width,period,refresh);
		}
		else if (path.startsWith("chart_header/fridgeData"))
		{
			*mime_type = "application/json";
			return data_log.getChartHeader();
		}
		else if (path.startsWith("chart_data/fridgeData"))
		{
			// query includes secs=N containing the number of seconds of chart data
			// to send, that that only we can convert into a number of records
			// using our known sampling rate.  Not perfect, but close enough.
			
			int num_recs = 0;
			int secs = getArg("secs",0);
			if (secs > 0)
			{
				num_recs = 1 + (secs * 1000) / TEMP_INTERVAL;
				#if 1
					LOGD("    getting secs(%d) == num_recs(%d)",secs,num_recs);
				#endif
			}
			return data_log.sendChartData(num_recs);
		}
	#endif

    return "";
}


