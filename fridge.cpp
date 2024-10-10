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
#include "vSense.h"
#include "tSense.h"
#include "fridgePixels.h"
#include <myIOTLog.h>
#include <myIOTWebServer.h>

#if WITH_FAKE_COMPRESSOR
	#include "fakeCompressor.h";
#endif


#define WITH_DATA_LOG	1


#define DEBUG_TSENSE	0
#define DEBUG_SETVALUE	0
#define DEBUG_CHART_DATA_HTTP  0
	// 0 turns off myIOTHTTP display of /chart_data request headers


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
		{"fridge",	LOG_COL_TYPE_TEMPERATURE,	10,		},
		{"comp",	LOG_COL_TYPE_TEMPERATURE,	10,		},
		{"mech",	LOG_COL_TYPE_UINT32,		1,		},
		{"rpm",		LOG_COL_TYPE_UINT32,		500,	},
	};

	myIOTDataLog data_log("fridgeData",4,fridge_cols,0);
		// 0 = debug_send_data LEVEL

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

// non myIOTDevice public static member variable initializations go here

bool Fridge::m_log_error;

int Fridge::m_fridge_temp_error;
int Fridge::m_comp_temp_error;
int Fridge::m_extra_temp_error;
bool Fridge::m_force_pixels;

volatile bool in_clear_error;



//------------------------------
// Application Vars
//------------------------------

float cur_fridge_temp;
float cur_comp_temp;
float cur_extra_temp;
bool  cur_mech_therm;
int cur_rpm;


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
	
	setPixel(PIXEL_SYSTEM,0);
	setPixel(PIXEL_STATE,0);
	showPixels();

	ui_screen.init();

	myIOTDevice::setup();
	randomSeed(time(NULL) + millis() + micros());

	setPixelsBrightness(_led_brightness);
	showPixels();

	//----------------------------------------
	// continuing generic myIOTDevice setup
	//----------------------------------------
	// set device to have a plotter tab
	// the plot output is generated in vSense.cpp

	setPlotLegend("batt,fan,diode");

	// initialize the dataLog
	// from a string on the stack

#if WITH_DATA_LOG
	String html = data_log.getChartHTML(
		300,		// height
		600,		// width
		86400,		// default period for the chart
		0 );		// default refresh interval

	#if 0
		Serial.print("html=");
		Serial.println(html.c_str());
	#endif

	// create the iotWidget String for the chart
	// and store it persistently on the heap

	fridgeWidget.html = new String(html);
	setDeviceWidget(&fridgeWidget);

	// add the chart link value

	_chart_link = "<a href='/spiffs/temp_chart.html?uuid=";
	_chart_link += getUUID();
	_chart_link += "' target='_blank'>Chart</a>";
#endif

	//-------------------------------------
	// Fridge specific intialization
	//-------------------------------------

	LOGI("initial FRIDGE_MODE=%d",_fridge_mode);

#if WITH_FAKE_COMPRESSOR
	fakeCompressor::init();
#endif

	v_sense.init();
	t_sense.init();		// returns 0 or an error_number
		// We ignore errors in initialization, but will notice
		// errors on individual measurements.

	LOGI("starting stateTask");
    xTaskCreatePinnedToCore(stateTask,
        "stateTask",
        8192,           // task stack
        NULL,           // param
        5,  	        // note that the priority is higher than one
        NULL,           // returned task handle
        ESP32_CORE_OTHER);

    proc_leave();

    ui_screen.backlight(1);
		// reset the activity timeout so the screen
		// doesn't immediately go blank due to long
		// myIOTDevice::setup() 
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


void Fridge::onBacklightChanged(const myIOTValue *value, int val)
{
	LOGU("onBacklightChanged(%d)",val);
	ui_screen.backlight(1);
}

void Fridge::onBrightnessChanged(const myIOTValue *desc, uint32_t val)
{
	LOGU("onBrightnessChanged(%d)",val);
	setPixelsBrightness(val);
	m_force_pixels = 1;

}


void Fridge::onSetPointChanged(const myIOTValue *value, float val)
{
	if (!strcmp(value->getId(),ID_SETPOINT_HIGH))
	{
		float other_val = fridge->getFloat(ID_SETPOINT_LOW);
		if (other_val > val - MIN_SETPOINT_DIF)
			fridge->setFloat(ID_SETPOINT_LOW,val-MIN_SETPOINT_DIF);
	}
	else
	{
		float other_val = fridge->getFloat(ID_SETPOINT_HIGH);
		if (other_val <  val + MIN_SETPOINT_DIF)
			fridge->setFloat(ID_SETPOINT_HIGH,val+MIN_SETPOINT_DIF);
	}
}

//==================================================
// setRPM
//==================================================
// Great news!  The compressor speed can be more precisely
// and flexibly controlled by PWM instead of using bulky
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

	// finally, we empirically determine that the
	// compressor is running if the fan is running
	// and there are no errorw.

	digitalWrite(LED_COMPRESS_ON,rpm);
	#if WITH_PWM
		ledcWrite(PWM_CHANNEL, pwm_duty);
	#endif
	cur_rpm = rpm;
}



void Fridge::clearInvError()
	// I noticed during reverse engineering that I can clear the
	// DIAG flashing LED by briefly turning the inverter on and
	// off quick enough so that it resets its state, but does not
	// have enough time to do an actual start attempt.
	//
	// This method is aware of the RPM and will clear the error
	// and initiate an immediate restart attempt if the RPM is
	// not zero at the call.
{
	if (in_clear_error)
	{
		LOGE("clearInvError() re-entered");
	}
	else if (_inv_error && WITH_PWM)
	{
		LOGD("clearInvError(%d) rpm=%d",_inv_error,cur_rpm);
		
		in_clear_error = true;

		if (cur_rpm)		// turn the compressor off first
		{
			LOGD("    setting rpm(0)");
			cur_rpm = 0;
			ledcWrite(PWM_CHANNEL, 0);
			delay(250);
		}
		ledcWrite(PWM_CHANNEL, DUTY_5MA);
		delay(250);
		ledcWrite(PWM_CHANNEL, 0);
		delay(250);

		_inv_error = 0;
		in_clear_error = false;
	}
	else
	{
		LOGE("Attempt to clearInvError(%d) WITH_PWM(%d)",
			 _inv_error,WITH_PWM);
	}
}



//=========================================================
// stateMachine()
//=========================================================

void measureTemperature(String id, float *rslt, int *err)
{
	if (id != "")
	{
		float temp = t_sense.getDegreesC(id);
		if (temp < TEMPERATURE_ERROR)
		{
			*rslt = temp;
			*err = 0;
		}
		else
			*err = t_sense.getLastError();
	}
}


void Fridge::stateMachine()
{
	uint32_t now = millis();

	//--------------------------------
	// voltage (inverter) sensors
	//--------------------------------
	// which is also how fast we run the fake compressor

	static uint32_t last_vsense = 0;
	if (!last_vsense || now - last_vsense >= _inv_sense_ms)
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
	if (!last_tsense || now - last_tsense > (_temp_sense_secs * 1000))
	{
		last_tsense = now;

		measureTemperature(_fridge_sense_id,&cur_fridge_temp,&m_fridge_temp_error);
		measureTemperature(_comp_sense_id,&cur_comp_temp,&m_comp_temp_error);
		measureTemperature(_extra_sense_id,&cur_extra_temp,&m_extra_temp_error);
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
			m_log_error = !data_log.addRecord((logRecord_t) &log_rec);
		#endif

		// takes a a little over 2ms
		// we ignore any errors returned by this

		t_sense.measure();

	}

	// determine whether to run or stop the refrigerator
	// We only set rpms if v_sense._plus_on, if the conmpressor has power,
	// otherwise we turn off the rpms.

	static int last_mode = _fridge_mode;
	if (last_mode != _fridge_mode)
	{
		last_mode = _fridge_mode;
		LOGW("FridgeMode(%d)",_fridge_mode);
	}

	if (!in_clear_error)	// dont change rpm while in a clearError() call
	{
		int rpm = 0;
		if (v_sense._plus_on)
		{
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
		}

		if (rpm != cur_rpm)
			setRPM(rpm);
	}

	//---------------------------
	// pixels
	//---------------------------

	#define SYS_CYCLE_TIME	4000
	#define SYS_FLASH_TIME  250
	#define SYS_FLASH_COLOR MY_LED_RED

	bool pixels_changed = 0;

	static uint32_t cur_sys_error;		// current system error
	static uint32_t sys_cycle_time;		// start of current system error flash cycle
	static uint32_t	last_color_system;	// base solid color for system pixel
	static uint32_t	last_color_state;	// solid (though possibly flashing) color for state pixel

	//-------------------------
	// system pixel shows Wifi status as solid color.
	// We flash the system pixel Red to highlight certain error conditions.
	//
	//  - temperature sense errors (m_fridge_temp_error || m_comp_temp_error) = 1 flash
	//	- any m_log_error = 2 flashes
	//  - both = 3 flashes
	//
	// We report errors writing to the logfile as flashing, which could result from
	// a bad/missing SD card, OR a failure to get NTP time as a STATION.  However,
	// note that in loss of Station Connection, the system clock may still be
	// accurate, and logging may continue to work even in AP mode, so we report
	// the flashes separately.

	uint32_t sys_error = 0;
	if (m_fridge_temp_error || m_comp_temp_error)
		sys_error += 1;
	if (m_log_error)
		sys_error += 2;

	// start a new cycle on sys_error change

	if (cur_sys_error != sys_error)
	{
		LOGD("SYS_ERROR(%d)",sys_error);
		cur_sys_error = sys_error;
		sys_cycle_time = now;
	}

	// start a new cycle every four seconds

	if (cur_sys_error && now - sys_cycle_time > SYS_CYCLE_TIME)
		sys_cycle_time = now;

	// the default system LED color is CYAN == no wifi.
	// we override it to red at appropriate points in the cycle
	// if there's any system error by calculating 'since' as
	// the millis since the cycle start, and if it is less
	// than 'cur_sys_error' full on/off flash times ...

	uint32_t color_system = MY_LED_CYAN;
		// default color == WIFI_OFF
	if (getBool(ID_WIFI))
	{
		iotConnectStatus_t wifi_mode = fridge->getConnectStatus();
		if (wifi_mode == IOT_CONNECT_ALL)
			color_system = MY_LED_ORANGE;
		else if (wifi_mode == IOT_CONNECT_AP)
			color_system = MY_LED_MAGENTA;
		else if (wifi_mode == IOT_CONNECT_STA)
			color_system = MY_LED_GREEN;
		else
			color_system = MY_LED_RED;
	}

	if (cur_sys_error)
	{
		uint32_t since = now - sys_cycle_time;
		if (since < cur_sys_error * 2 * SYS_FLASH_TIME)
		{
			// develop a number where odd = ON and even == OFF
			int num = since / SYS_FLASH_TIME;
			if (num % 2)
				color_system = SYS_FLASH_COLOR;
		}
	}



	//--------------
	// state pixel

	uint32_t color_state = MY_LED_RED;
	if (v_sense._diag_on)
		color_state = MY_LED_RED;
	else if (cur_rpm)
		color_state = MY_LED_BLUE;
	else if (v_sense._fan_on)
		color_state = MY_LED_YELLOW;
	else if (v_sense._plus_on)
		color_state = MY_LED_GREEN;

	//-------------
	// show if changed

	if (m_force_pixels || last_color_system != color_system)
	{
		last_color_system = color_system;
		setPixel(PIXEL_SYSTEM,color_system);
		pixels_changed = 1;
	}
	if (m_force_pixels || last_color_state != color_state)
	{
		last_color_state = color_state;
		setPixel(PIXEL_STATE,color_state);
		pixels_changed = 1;
	}
	if (pixels_changed)
		showPixels();
	m_force_pixels = 0;

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
		#if DEBUG_SETVALUE
			LOGD("   setting %s(%0.3fC)=%0.3fF",id,set,centigradeToFarenheit(set));
		#endif
		fridge->setFloat(id, cur);
	}
}


void addStatusInt(bool with_zeros, String &status, char *what, int value, bool cr_after=false)
{
	if (with_zeros || value)
	{
		status += what;
		status += String(value);
		status += cr_after ? "\n" : " ";
	}
}



void Fridge::loop()
{
	myIOTDevice::loop();

	// handle UI

	ui_screen.loop();

	// publish changes every couple of seconds

	#define PUBLISH_INTERVAL 	2000

	uint32_t now = millis();
	static uint32_t last_publish;
	if (now - last_publish > PUBLISH_INTERVAL)
	{
		last_publish = now;

		// publish temperatures

		publishTemp(ID_FRIDGE_TEMP,cur_fridge_temp);
		publishTemp(ID_COMP_TEMP,cur_comp_temp);
		publishTemp(ID_EXTRA_TEMP,cur_extra_temp);

		// publishs status

		String status = "";
		addStatusInt(0,status,"FTEMP_ERROR:",m_fridge_temp_error);
		addStatusInt(0,status,"CTEMP_ERROR:",m_comp_temp_error);
		addStatusInt(0,status,"ETEMP_ERROR:",m_extra_temp_error);

		if (!v_sense._plus_on && _fridge_mode)
		{
			if (status != "")
				status += "\n";
			status += "FRIDGE_MODE(";
			status += String(_fridge_mode);
			status += ") while INV_PLUS (power) OFF!!";
		}
		if (_status_str != status)
			setString(ID_STATUS,status);

		// publish other states

		if (_mech_therm != cur_mech_therm)
			setBool(ID_MECH_THERM,cur_mech_therm);
		if (_comp_rpm != cur_rpm)
			setInt(ID_COMP_RPM,cur_rpm);
		if (_inv_error != v_sense._error_code)
			setInt(ID_INV_ERROR,v_sense._error_code);
		if (_inv_plus != v_sense._plus_on)
			setBool(ID_INV_PLUS,v_sense._plus_on);
		if (_inv_fan != v_sense._fan_on)
			setBool(ID_INV_FAN,v_sense._fan_on);

		if (_volts_inv != v_sense._volts_inv)
			setFloat(ID_VOLTS_INV,v_sense._volts_inv);
		if (_volts_5v != v_sense._volts_5V)
			setFloat(ID_VOLTS_5V,v_sense._volts_5V);
	}
}



//----------------------------------------
// chart API
//----------------------------------------

// virtual
bool Fridge::showDebug(String path)	// override;
{
	// called by myIOTHttp while path still has /custom at front
	#if !DEBUG_CHART_DATA_HTTP
		if (path.startsWith("/custom/chart_data/fridgeData"))
			return 0;
	#endif
	return 1;
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


