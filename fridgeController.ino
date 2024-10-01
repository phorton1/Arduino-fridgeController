
#include "fridge.h"
#include <myIOTLog.h>
#include "tSense.h"
#if WITH_FAKE_COMPRESSOR
	#include "fakeCompressor.h";
#endif


//------------------------------
// myIOT setup
//------------------------------

static valueIdType dash_items[] = {

	ID_FRIDGE_MODE,
	ID_SETPOINT_HIGH,
	ID_SETPOINT_LOW,

	ID_FRIDGE_TEMP,
	ID_COMP_TEMP,
	ID_EXTRA_TEMP,
	ID_MECH_THERM,
	ID_COMP_RPM,
	ID_INV_ERROR,
	ID_INV_PLUS,
	ID_INV_FAN,
	ID_INV_COMPRESS,
	ID_TEMP_ERROR,
	ID_VOLTS_FRIDGE,
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
#if WITH_FAKE_COMPRESSOR
	ID_FAKE_COMPRESSOR,
	ID_FAKE_COMP_ON,
	ID_FAKE_PROB_ERROR,
	ID_FAKE_AMBIENT,
	ID_FAKE_INSULATE,
	ID_FAKE_COOLING,
	ID_FAKE_HEATING,
	ID_FAKE_COOLDOWN,
#endif
    0
};


static enumValue fridgeModes[] = {
	"Off",
	"RUN_MIN",
	"RUN_MAX",
	"RUN_USER",
	"RUN_MECH",
	"RUN_TEMP",
    0};

#define VALUE_STYLE_RO_TEMP		(VALUE_STYLE_READONLY | VALUE_STYLE_TEMPERATURE)

// default temperatures are in centigrade

const valDescriptor fridge_values[] =
{
    {ID_DEVICE_NAME,		VALUE_TYPE_STRING,  VALUE_STORE_PREF,     	VALUE_STYLE_REQUIRED,   NULL,   	NULL,   FRIDGE_CONTROLLER },        // override base class element

	{ID_FRIDGE_MODE,		VALUE_TYPE_ENUM,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_fridge_mode,     NULL, 	{ .enum_range = { 0, fridgeModes }} },
	{ID_USER_RPM,           VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_user_rpm,        NULL,	{ .int_range	= {2600, 2000, 3500}}, },
	{ID_MIN_RPM,            VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_min_rpm,         NULL,	{ .int_range	= {2000, 2000, 3500}}, },
	{ID_MAX_RPM,            VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_max_rpm,         NULL,	{ .int_range	= {3200, 2000, 3500}}, },
	{ID_FRIDGE_SENSE_ID,    VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_fridge_sense_id, NULL,	MY_TSENSOR_01 },
	{ID_COMP_SENSE_ID,      VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_comp_sense_id,   NULL,	MY_TSENSOR_02 },
	{ID_EXTRA_SENSE_ID,     VALUE_TYPE_STRING,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_extra_sense_id,  NULL,	},
	{ID_TEMP_SENSE_SECS,    VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_temp_sense_secs, NULL,	{ .int_range	= {10,  0,		300}},  },
	{ID_INV_SENSE_MS,   	VALUE_TYPE_INT,		VALUE_STORE_PREF,		VALUE_STYLE_NONE,		(void *) &Fridge::_inv_sense_ms,	NULL,	{ .int_range	= {20,  5,		1000}}, },
	{ID_SETPOINT_HIGH,      VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_TEMPERATURE,(void *) &Fridge::_setpoint_high,   NULL,	{ .float_range	= {-12,	-200,	200}},	},
	{ID_SETPOINT_LOW,       VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_TEMPERATURE,(void *) &Fridge::_setpoint_low,    NULL,	{ .float_range	= {-20,	-200,	200}},	},

	{ID_FRIDGE_TEMP,        VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_fridge_temp,     NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_COMP_TEMP,          VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_comp_temp,       NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_EXTRA_TEMP,         VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_RO_TEMP,	(void *) &Fridge::_extra_temp,      NULL,	{ .float_range	= {0,	-200,	200}},	},
	{ID_MECH_THERM,         VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_mech_therm,      NULL,	},
	{ID_COMP_RPM,           VALUE_TYPE_INT,		VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_comp_rpm,        NULL,	{ .int_range	= {0,	0,		5000}}, },
	{ID_INV_ERROR,          VALUE_TYPE_INT,		VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_error,       NULL,	{ .int_range	= {0,	0,		7}}, },
	{ID_INV_PLUS,           VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_plus,        NULL,	},
	{ID_INV_FAN,            VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_fan,         NULL,	},
	{ID_INV_COMPRESS,       VALUE_TYPE_BOOL,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_inv_compress,    NULL,	},
	{ID_TEMP_ERROR,         VALUE_TYPE_INT,		VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_temp_error,      NULL,	{ .int_range	= {0,	0,	10}}, },
	{ID_VOLTS_FRIDGE,		VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_volts_fridge,	NULL,	{ .float_range	= {0,	0,	15}},	},
	{ID_VOLTS_5V,			VALUE_TYPE_FLOAT,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_volts_5v,		NULL,	{ .float_range	= {0,	0,	15}},	},

	{ID_CHART_LINK,			VALUE_TYPE_STRING,	VALUE_STORE_PUB,		VALUE_STYLE_READONLY,	(void *) &Fridge::_chart_link, },

#if WITH_FAKE_COMPRESSOR
	{ID_FAKE_COMPRESSOR,	VALUE_TYPE_BOOL,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_compressor,	NULL,	{ .int_range	= {1,	0,	1}}, },
	{ID_FAKE_COMP_ON,		VALUE_TYPE_BOOL,    VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_comp_on,		NULL,	{ .int_range	= {0,	0,	1}}, },
	{ID_FAKE_PROB_ERROR,	VALUE_TYPE_INT,     VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_prob_error,	NULL,	{ .int_range	= {3,	0,	100}}, },
	{ID_FAKE_AMBIENT,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_ambient,		NULL,	{ .float_range	= {26.67, 20, 40}},	},
	{ID_FAKE_INSULATE,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_insulate,	NULL,	{ .float_range	= {8,   -1000, 1000}},	},
	{ID_FAKE_COOLING,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_cooling,		NULL,	{ .float_range	= {7, 	-1000, 1000}},	},
	{ID_FAKE_HEATING,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_heating,		NULL,	{ .float_range	= {0.5, -1000, 1000}},	},
	{ID_FAKE_COOLDOWN,		VALUE_TYPE_FLOAT,	VALUE_STORE_PREF,		VALUE_STYLE_NONE,	(void *) &fakeCompressor::_cooldown,	NULL,	{ .float_range	= {1.0, -1000, 1000}},	},
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

float	Fridge::_fridge_temp;
float	Fridge::_comp_temp;
float	Fridge::_extra_temp;
bool 	Fridge::_mech_therm;
int		Fridge::_comp_rpm;
int		Fridge::_inv_error;
bool	Fridge::_inv_plus;
bool	Fridge::_inv_fan;
bool	Fridge::_inv_compress;
int		Fridge::_temp_error;
float	Fridge::_volts_fridge;
float	Fridge::_volts_5v;

String 	Fridge::_chart_link;


//--------------------------------
// main
//--------------------------------


void setup()
{
    Serial.begin(MY_IOT_ESP32_CORE == 3 ? 115200 : 921600);
    delay(1000);

	ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	ledcAttachPin(PIN_PUMP_PWM, PWM_CHANNEL);
	ledcWrite(PWM_CHANNEL, 0);

    Fridge::setDeviceType(FRIDGE_CONTROLLER);
    Fridge::setDeviceVersion(FRIDGE_CONTROLLER_VERSION);
    Fridge::setDeviceUrl(FRIDGE_CONTROLLER_URL);

    LOGU("");
    fridge = new Fridge();
    fridge->addValues(fridge_values,NUM_FRIDGE_VALUES);
	fridge->setTabLayouts(dash_items,config_items);
    LOGU("fridgeController.ino setup() started on core(%d)",xPortGetCoreID());

    fridge->setup();

    LOGU("fridgeController.ino setup() finished",0);
}



void loop()
{
    fridge->loop();
}



