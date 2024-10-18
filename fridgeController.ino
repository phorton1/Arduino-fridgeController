
#include "fridge.h"
#include <myIOTLog.h>
#include "tSense.h"
#include "fridgePixels.h"
#if WITH_FAKE_COMPRESSOR
	#include "fakeCompressor.h";
#endif
#include "fridgeToolTips.h"


//------------------------------
// myIOT setup
//------------------------------

#define DEFAULT_LED_BRIGHTNESS	30

static valueIdType dash_items[] = {

	ID_FRIDGE_MODE,
	ID_SETPOINT_HIGH,
	ID_SETPOINT_LOW,
	ID_FRIDGE_TEMP,
	ID_COMP_TEMP,
	ID_INV_ERROR,
	ID_CLEAR_ERROR,
	ID_EXTRA_TEMP,
	ID_MECH_THERM,
	ID_COMP_RPM,
	ID_STATUS,
	ID_INV_PLUS,
	ID_INV_FAN,
	ID_VOLTS_INV,
	ID_VOLTS_5V,

	ID_CHART_LINK,
    0
};


static valueIdType config_items[] = {

	ID_USER_RPM,
	ID_MIN_RPM,
	ID_MAX_RPM,
	ID_FRIDGE_SENSE_ID,
	ID_COMP_SENSE_ID,
	ID_EXTRA_SENSE_ID,
	ID_TEMP_SENSE_SECS,
	ID_INV_SENSE_MS,
	ID_CALIB_VOLTS_INV,
	ID_CALIB_VOLTS_5V,

	ID_BACKLIGHT_SECS,
	ID_LED_BRIGHTNESS,

#if WITH_FAKE_COMPRESSOR
	ID_USE_FAKE,
	ID_RESET_FAKE,
	ID_FAKE_COMP_ON,
	ID_FAKE_PROB_ERROR,
	ID_FAKE_AMBIENT,
	ID_FAKE_PERIOD,
	ID_COOLING_ACCEL,
	ID_WARMING_ACCEL,
	ID_HEATING_ACCEL,
	ID_COOLDOWN_ACCEL,
	ID_MAX_COOL_VEL,
	ID_MAX_WARM_VEL,
	ID_MAX_HEAT_VEL,
	ID_MAX_DOWN_VEL,
#endif
    0
};


enumValue fridgeModes[] = {
	"Off",
	"MIN",
	"MAX",
	"USER",
	"MECH",
	"TEMP",
    0};

#define VALUE_STYLE_RO_TEMP		(VALUE_STYLE_READONLY | VALUE_STYLE_TEMPERATURE)

// default temperatures are in centigrade

const valDescriptor fridge_values[] =
{
    {ID_DEVICE_NAME,		VALUE_TYPE_STRING,  VALUE_STORE_PREF,     	VALUE_STYLE_REQUIRED,   NULL,   	NULL,   FRIDGE_CONTROLLER },        // override base class element

	{ID_FRIDGE_MODE,		VALUE_TYPE_ENUM,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_fridge_mode,     NULL, 	{ .enum_range = { 0, fridgeModes }} },
	{ID_SETPOINT_HIGH,      VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_TEMPERATURE,(void *) &Fridge::_setpoint_high,   (void *) Fridge::onSetPointChanged,	{ .float_range	= {-12,	-200+MIN_SETPOINT_DIF,	200+MIN_SETPOINT_DIF}},	},
	{ID_SETPOINT_LOW,       VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_TEMPERATURE,(void *) &Fridge::_setpoint_low,    (void *) Fridge::onSetPointChanged,	{ .float_range	= {-20,	-200-MIN_SETPOINT_DIF,	200-MIN_SETPOINT_DIF}},	},
	{ID_USER_RPM,           VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_user_rpm,        NULL,	{ .int_range	= {2600, 2000, 3500}}, },
	{ID_MIN_RPM,            VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_min_rpm,         NULL,	{ .int_range	= {2000, 2000, 3500}}, },
	{ID_MAX_RPM,            VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_max_rpm,         NULL,	{ .int_range	= {3200, 2000, 3500}}, },
	{ID_FRIDGE_SENSE_ID,    VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_fridge_sense_id, NULL,	MY_TSENSOR_01 },
	{ID_COMP_SENSE_ID,      VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_comp_sense_id,   NULL,	MY_TSENSOR_02 },
	{ID_EXTRA_SENSE_ID,     VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_extra_sense_id,  NULL,	},
	{ID_TEMP_SENSE_SECS,    VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_temp_sense_secs, NULL,	{ .int_range	= {10,  0,		300}},  },
	{ID_INV_SENSE_MS,   	VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_inv_sense_ms,	NULL,	{ .int_range	= {20,  5,		1000}}, },
	{ID_CALIB_VOLTS_INV,    VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_calib_volts_inv, NULL,	{ .float_range	= {1,	0.5,	1.5}},	},
	{ID_CALIB_VOLTS_5V,     VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_calib_volts_5v,  NULL,	{ .float_range	= {1,	0.5,	1.5}},	},

    {ID_CLEAR_ERROR,        VALUE_TYPE_COMMAND, VALUE_STORE_PROG,       VALUE_STYLE_NONE,    NULL,                       			(void *) Fridge::clearInvError },

	{ID_STATUS,      		VALUE_TYPE_STRING,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_status_str,   	NULL,	},
	{ID_FRIDGE_TEMP,        VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_fridge_temp,     NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_COMP_TEMP,          VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_comp_temp,       NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_EXTRA_TEMP,         VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_extra_temp,      NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_MECH_THERM,         VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_mech_therm,      NULL,	},
	{ID_COMP_RPM,           VALUE_TYPE_INT,		VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_comp_rpm,        NULL,	{ .int_range	= {0,	0,		5000}}, },
	{ID_INV_ERROR,          VALUE_TYPE_INT,		VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_error,       NULL,	{ .int_range	= {0,	0,		7}}, },
	{ID_INV_PLUS,           VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_plus,        NULL,	},
	{ID_INV_FAN,            VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_fan,         NULL,	},
	{ID_VOLTS_INV,			VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_volts_inv,		NULL,	{ .float_range	= {0,	0,	24}},	},
	{ID_VOLTS_5V,			VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_volts_5v,		NULL,	{ .float_range	= {0,	0,	24}},	},

	{ID_BACKLIGHT_SECS,     VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_backlight_secs,  (void *) Fridge::onBacklightChanged, { .int_range	= {30,	15,	BACKLIGHT_ALWAYS_ON}}, },
	{ID_LED_BRIGHTNESS,  	VALUE_TYPE_INT, 	VALUE_STORE_PREF,     	VALUE_STYLE_NONE,		(void *) &Fridge::_led_brightness,	(void *) Fridge::onBrightnessChanged, { .int_range = { DEFAULT_LED_BRIGHTNESS,  0,  254}} },
	{ID_CHART_LINK,			VALUE_TYPE_STRING,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_chart_link, },

#if WITH_FAKE_COMPRESSOR
	{ID_USE_FAKE,			VALUE_TYPE_BOOL,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_use_fake,	NULL,	{ .int_range	= {1,	0,	1}}, },
    {ID_RESET_FAKE,         VALUE_TYPE_COMMAND, VALUE_STORE_PROG,       VALUE_STYLE_NONE,    NULL,                       			(void *) fakeCompressor::init },
	{ID_FAKE_COMP_ON,		VALUE_TYPE_BOOL,    VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_comp_on,		NULL,	{ .int_range	= {0,	0,	1}}, },
	{ID_FAKE_PROB_ERROR,	VALUE_TYPE_INT,     VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_prob_error,	NULL,	{ .int_range	= {3,	0,	100}}, },

	{ID_FAKE_AMBIENT,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_ambient,		NULL,	{ .float_range	= {26.67, 20, 40}},	},
	{ID_FAKE_PERIOD,		VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_fake_period,		NULL,	{ .int_range	= {4, 	1, 10000}},	},
	
	{ID_COOLING_ACCEL,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_cooling_accel,	NULL,	{ .float_range	= {0.1, -1000, 1000}},	},
	{ID_WARMING_ACCEL, 		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_warming_accel,	NULL,	{ .float_range	= {0.02, -1000, 1000}},	},
	{ID_HEATING_ACCEL, 		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_heating_accel,	NULL,	{ .float_range	= {0.5, -1000, 1000}},	},
	{ID_COOLDOWN_ACCEL,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_cooldown_accel,	NULL,	{ .float_range	= {0.4, -1000, 1000}},	},
	{ID_MAX_COOL_VEL,  		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_max_cool_vel, 	NULL,	{ .float_range	= {2, 	-1000, 1000}},	},
	{ID_MAX_WARM_VEL,  		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_max_warm_vel, 	NULL,	{ .float_range	= {0.2, -1000, 1000}},	},
	{ID_MAX_HEAT_VEL,  		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_max_heat_vel, 	NULL,	{ .float_range	= {1.6, -1000, 1000}},	},
	{ID_MAX_DOWN_VEL,  		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_max_down_vel, 	NULL,	{ .float_range	= {1.8, -1000, 1000}},	},
#endif                                          

};


#define NUM_FRIDGE_VALUES (sizeof(fridge_values)/sizeof(valDescriptor))


// static member variable declarations

int 	Fridge::_fridge_mode;
int 	Fridge::_user_rpm;
int		Fridge::_min_rpm;
int		Fridge::_max_rpm;
String	Fridge::_fridge_sense_id;
String	Fridge::_comp_sense_id;
String	Fridge::_extra_sense_id;
int		Fridge::_temp_sense_secs;
int		Fridge::_inv_sense_ms;
float	Fridge::_setpoint_high;
float	Fridge::_setpoint_low;
float	Fridge::_calib_volts_inv;
float	Fridge::_calib_volts_5v;


String  Fridge::_status_str;
float	Fridge::_fridge_temp;
float	Fridge::_comp_temp;
float	Fridge::_extra_temp;
bool 	Fridge::_mech_therm;
int		Fridge::_comp_rpm;
int		Fridge::_inv_error;
bool	Fridge::_inv_plus;
bool	Fridge::_inv_fan;
bool	Fridge::_inv_compress;
float	Fridge::_volts_inv;
float	Fridge::_volts_5v;

int		Fridge::_backlight_secs;
int		Fridge::_led_brightness;
String 	Fridge::_chart_link;


//--------------------------------
// main
//--------------------------------


void setup()
{
	// first indicator

	pinMode(LED_POWER_ON,OUTPUT);
	pinMode(LED_FAN_ON,OUTPUT);
	pinMode(LED_DIODE_ON,OUTPUT);
	pinMode(LED_COMPRESS_ON,OUTPUT);

	digitalWrite(LED_POWER_ON,1);
	digitalWrite(LED_FAN_ON,0);
	digitalWrite(LED_DIODE_ON,0);
	digitalWrite(LED_COMPRESS_ON,0);

	setPixel(PIXEL_SYSTEM,MY_LED_CYAN);
	setPixel(PIXEL_STATE,0);
	setPixel(PIXEL_ERROR,0);
	showPixels();

    Serial.begin(MY_IOT_ESP32_CORE == 3 ? 115200 : 921600);
    delay(1000);

	#if WITH_PWM
		ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
		ledcAttachPin(PIN_PUMP_PWM, PWM_CHANNEL);
		ledcWrite(PWM_CHANNEL, 0);
	#endif
	
    Fridge::setDeviceType(FRIDGE_CONTROLLER);
    Fridge::setDeviceVersion(FRIDGE_CONTROLLER_VERSION);
    Fridge::setDeviceUrl(FRIDGE_CONTROLLER_URL);

    LOGU("");
    fridge = new Fridge();
    fridge->addValues(fridge_values,NUM_FRIDGE_VALUES);
	fridge->setTabLayouts(dash_items,config_items);
	fridge->addDerivedToolTips(fridge_tooltips);
    LOGU("fridgeController.ino setup() started on core(%d)",xPortGetCoreID());

    fridge->setup();

	// clear indicators
	
	digitalWrite(LED_FAN_ON,0);
	digitalWrite(LED_POWER_ON,0);
	digitalWrite(LED_DIODE_ON,0);
	digitalWrite(LED_COMPRESS_ON,0);

	setPixel(PIXEL_SYSTEM,0);
	setPixel(PIXEL_STATE,0);
	setPixel(PIXEL_ERROR,0);
	showPixels();

    LOGU("fridgeController.ino setup() finished",0);
}



void loop()
{
    fridge->loop();
}



