//-----------------------------------------------------------
// fridge.cpp
//-----------------------------------------------------------
// TODO:
//
//  Show found TSense devices
//  Set voltages when change > 0.1 volts
//	Log with correct names
//	Log more stuff
//  Chart options for initial shown columns
//  Emulate a whole working compressor?

#include "fridge.h"
#include "uiScreen.h"
#include "uiButtons.h"
#include "vSense.h"
#include "tSense.h"
#include <myIOTLog.h>
#include <myIOTWebServer.h>

#if WITH_FAKE_COMPRESSOR
	#include "fakeCompressor.h";
#endif


#define DEBUG_TSENSE	1

#define WITH_DATA_LOG	1


//-----------------------------------------------
// data_log setup
//-----------------------------------------------

#if WITH_DATA_LOG

	#include <myIOTDataLog.h>

	typedef struct
	{
		uint32_t	dt;	// filled in by dataLog
		float		temp1;
		float		temp2;
		uint32_t	mech;
		uint32_t	rpm;
	} fridgeLog_t;
	
	// The tick_intervals are 0 based and will be will be lined up

	logColumn_t  fridge_cols[] = {
		{"temp1",	LOG_COL_TYPE_TEMPERATURE,	10,		},
		{"temp2",	LOG_COL_TYPE_TEMPERATURE,	10,		},
		{"mech",	LOG_COL_TYPE_UINT32,		1,		},
		{"rpm",		LOG_COL_TYPE_UINT32,		500,	},
	};

	myIOTDataLog data_log("fridgeData",4,fridge_cols);

	myIOTWidget_t fridgeWidget = {
		"fridgeWidget",
        "/myIOT/jquery.jqplot.min.css?cache=1,"
            "/myIOT/jquery.jqplot.min.js?cache=1,"
            "/myIOT/jqplot.dateAxisRenderer.js?cache=1,"
            "/myIOT/jqplot.cursor.js?cache=1,"
            "/myIOT/jqplot.highlighter.js?cache=1,"
            "/myIOT/jqplot.legendRenderer.js?cache=1,"
			"/myIOT/iotChart.js",
		"doChart('fridgeData')",
		"stopChart('fridgeData')",
		NULL };
		

#endif	// WITH_DATA_LOG


Fridge *fridge;

OneWire one_wire(PIN_ONE_WIRE);
tSense t_sense(&one_wire);


//------------------------------
// Application Vars
//------------------------------



vSense v_sense;
uiScreen  ui_screen;
uiButtons ui_buttons(PIN_BUTTON1,PIN_BUTTON2,PIN_BUTTON3);

float cur_fridge_temp;
float cur_comp_temp;
float cur_extra_temp;
bool  cur_mech_therm;
int	  cur_rpm;



//=========================================================
// implementation
//=========================================================

Fridge::Fridge()
{
    fridge = this;
}


void Fridge::setup()
{
    LOGD("Fridge::setup(%s) started",getVersion());
    proc_entry();

	pinMode(PIN_MECH_THERM,INPUT_PULLDOWN);
	
	ui_screen.init();
	ui_buttons.init();

	myIOTDevice::setup();
	LOGI("initial FRIDGE_MODE=%d",_fridge_mode);
	
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
	// on the stack
	String html = data_log.getChartHTML(
		300,		// height
		600,		// width
		86400,		// default period for the chart
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
#endif

	setPlotLegend("batt,fan,diode");
		// see vSense.cpp

	// ACTIVE INITIALIZATION

#if WITH_FAKE_COMPRESSOR
	fakeCompressor::init();
#endif

	v_sense.init();
	t_sense.init();

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



//==================================================
// setRPM
//==================================================
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

void Fridge::setRPM(int rpm)
{
	int pwm_duty = 0;

	if (rpm > 0)
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
		pwm_duty = duty_float;
		LOGI("setRPM(%d)  desired_ma(%0.3f)  duty(%d)",rpm,desired_ma,pwm_duty);
	}
	else
	{
		LOGI("setRPM(0)");
	}

	ledcWrite(PWM_CHANNEL, pwm_duty);
	cur_rpm = rpm;
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
	// which is also how fast we run the fake compressor

	static uint32_t last_vsense = 0;
	if (now - last_vsense >= _inv_sense_ms)
	{
		last_vsense = now;
		#if WITH_FAKE_COMPRESSOR
			fakeCompressor::run();
		#endif

		v_sense.sense();

	}

	//---------------------------
	// temperature sensors
	//---------------------------

	static uint32_t last_tsense;
	if (now - last_tsense > (_temp_sense_secs * 1000))
	{
		last_tsense = now;

		if (_fridge_sense_id != "")
		{
			float temp = t_sense.getDegreesC(_fridge_sense_id);
			if (temp < TEMPERATURE_ERROR)
				cur_fridge_temp = temp;
		}
		if (_comp_sense_id != "")
		{
			float temp = t_sense.getDegreesC(_comp_sense_id);
			if (temp < TEMPERATURE_ERROR)
				cur_comp_temp = temp;
		}
		if (_extra_sense_id != "")
		{
			float temp = t_sense.getDegreesC(_extra_sense_id);
			if (temp < TEMPERATURE_ERROR)
				cur_extra_temp = temp;
		}

		// odd place for this

		#if WITH_FAKE_COMPRESSOR
			if (fakeCompressor::_use_fake)
			{
				if (fakeCompressor::g_fridge_temp > _setpoint_high)
					cur_mech_therm = 1;
				if (fakeCompressor::g_fridge_temp < _setpoint_low)
					cur_mech_therm = 0;
			}
			else
		#endif
			cur_mech_therm = digitalRead(PIN_MECH_THERM);


		#if DEBUG_TSENSE
			LOGD("TSENSE fridge=%0.3fC comp=%0.3fC  mech=%d  rpm=%d",
				 cur_fridge_temp,
				 cur_comp_temp,
				 cur_mech_therm,
				 cur_rpm);
		#endif

		#if WITH_DATA_LOG
			fridgeLog_t log_rec;
			log_rec.temp1 = cur_fridge_temp;
			log_rec.temp2 = cur_comp_temp;
			log_rec.mech  = cur_mech_therm;
			log_rec.rpm   = cur_rpm;
			data_log.addRecord((logRecord_t) &log_rec);
		#endif

		// takes a a little over 2ms

		t_sense.measure();

	}



	// determine whether to run or stop the refrigerator

	static int last_mode = _fridge_mode;
	if (last_mode != _fridge_mode)
	{
		last_mode = _fridge_mode;
		LOGW("FridgeMode(%d)",_fridge_mode);
	}

	int rpm = 0;
	if (_fridge_mode == FRIDGE_MODE_RUN_MIN)
		rpm = _min_rpm;
	else if (_fridge_mode == FRIDGE_MODE_RUN_MAX)
		rpm = _max_rpm;
	else if (_fridge_mode == FRIDGE_MODE_RUN_USER)
		rpm = _user_rpm;
	else if (_fridge_mode == FRIDGE_MODE_RUN_MECH)
		rpm = cur_mech_therm ? _user_rpm : 0;
	else if (_fridge_mode == FRIDGE_MODE_RUN_TEMP)
	{
		if (cur_fridge_temp > _setpoint_high)
			rpm = _user_rpm;
		else if (cur_fridge_temp < _setpoint_low)
			rpm = 0;
		else
			rpm = cur_rpm;
	}

	if (rpm != cur_rpm)
		setRPM(rpm);

	
}	// stateMachine()



//=========================================================
// loop()
//=========================================================
// We log the full floating point Centigrade temperature,
// but we only publish values in Centigrade rounded to 0.1C.
// Centigrade is more granular than Farenheit, but in the UI
// we will round those again to the nearest 10't in F, and
// precision is lost, so the values shown in the UI may not
// agree with the (dummy) values that are shown in DEBUG_TSENSE

float round1(float val)
{
	val +=
		val > 0 ? 0.05 :
		val < 0 ? -0.05 :
		0.0;

	int ival = val * 10;
	return ((float)ival) / 10.0;
}

void publishTemp(const char *id, float cur_value)
{
	float cur = round1(cur_value);
	float set = fridge->getFloat(id);
	if (set != cur)
	{
		LOGD("   setting %s(%0.3fC)=%0.3fF",id,set,centigradeToFarenheit(set));
		fridge->setFloat(id, cur);
	}
}


void Fridge::loop()
{
	myIOTDevice::loop();

	// handle UI

	ui_buttons.loop();

	// publish temperatures
	
	publishTemp(ID_FRIDGE_TEMP,cur_fridge_temp);
	publishTemp(ID_COMP_TEMP,cur_comp_temp);
	publishTemp(ID_EXTRA_TEMP,cur_extra_temp);

	// publish other states

	int terr = t_sense.getLastError();
	if (_temp_error != terr)
		setInt(ID_TEMP_ERROR,terr);
	if (_mech_therm != cur_mech_therm)
		setBool(ID_MECH_THERM,cur_mech_therm);
	if (_comp_rpm != cur_rpm)
		setInt(ID_COMP_RPM,cur_rpm);
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
			
			int height = myiot_web_server->getArg("height",400);
			int width  = myiot_web_server->getArg("width",800);
			int period = myiot_web_server->getArg("period",86400);	// day default
			int refresh = myiot_web_server->getArg("refresh",0);
			return data_log.getChartHTML(height,width,period,refresh);
		}
		else if (path.startsWith("chart_header/fridgeData"))
		{
			*mime_type = "application/json";
			return data_log.getChartHeader();
		}
		else if (path.startsWith("chart_data/fridgeData"))
		{
			int secs = myiot_web_server->getArg("secs",0);
			return data_log.sendChartData(secs);
		}
	#endif

    return "";
}


