//----------------------------------------------
// constants for fridgeController
//----------------------------------------------
// uses ESP32 standard SPI mosi(23) miso(19) sclk(18)
// and I2C scl(22) sda(21)

#pragma once

#include <myIOTDevice.h>

#define WITH_PWM	1


//=========================================================
// pins
//=========================================================

#define PIN_PUMP_PWM		16		// PWM sent to BC547 transistor tween C2 and T from inverter
#define PIN_EXT_LED			13		// external WS2812b LED(s)
#define PIN_ONE_WIRE		4		// One-Wire bus to DS18B20 temperature probes
#define PIN_MECH_THERM		15		// Sense the (old) Danfoss mechanical thermostat

#define PIN_BUTTON1			12
#define PIN_BUTTON2			33
#define PIN_BUTTON3			32

#define PIN_S_PLUS			34		// sense FAN/DIODE+ proxy for 12V
#define PIN_S_FAN			39		// sense FAN- low when fan running
#define PIN_S_DIODE 		36		// sense DIODE- made HIGH with BC557 transistor when diag diode on
#define PIN_S_5V			35		// sense buck 5V power supply

#define PIN_LED_RED			26		// diagnostic diode on
#define PIN_LED_GREEN		14		// have power
#define PIN_LED_BLUE		25		// compressor on
#define PIN_LED_YELLOW		27		// fan on

#define LED_COMPRESS_ON		PIN_LED_BLUE
#define LED_DIODE_ON 		PIN_LED_RED
#define LED_POWER_ON		PIN_LED_GREEN
#define LED_FAN_ON			PIN_LED_YELLOW


#define PWM_CHANNEL				0
#define PWM_FREQ				5000
#define PWM_RESOLUTION			8


//------------------------
// myIOT definition
//------------------------

#define FRIDGE_CONTROLLER			"fridgeController"
#define FRIDGE_CONTROLLER_VERSION	"fc0.2"
#define FRIDGE_CONTROLLER_URL		"https://github.com/phorton1/Arduino-fridgeController"

#define BACKLIGHT_ALWAYS_ON			120

#define MIN_SETPOINT_DIF	2.0		// centigrade


// enumerated FRIDGE_MODE types & ids

#define FRIDGE_MODE_OFF				0
#define FRIDGE_MODE_RUN_MIN			1
#define FRIDGE_MODE_RUN_MAX			2
#define FRIDGE_MODE_RUN_USER		3
#define FRIDGE_MODE_RUN_MECH		4
#define FRIDGE_MODE_RUN_TEMP		5

// main fridge config/state values

#define ID_FRIDGE_MODE				"FRIDGE_MODE"
#define ID_SETPOINT_HIGH            "SETPOINT_HIGH"
#define ID_SETPOINT_LOW             "SETPOINT_LOW"
#define ID_USER_RPM                 "USER_RPM"
#define ID_MIN_RPM                  "MIN_RPM"
#define ID_MAX_RPM                  "MAX_RPM"
#define ID_FRIDGE_SENSE_ID          "FRIDGE_SENSE_ID"
#define ID_COMP_SENSE_ID            "COMP_SENSE_ID"
#define ID_EXTRA_SENSE_ID           "EXTRA_SENSE_ID"
#define ID_TEMP_SENSE_SECS          "TEMP_SENSE_SECS"
#define ID_INV_SENSE_MS         	"INV_SENSE_MS"
#define ID_CALIB_VOLTS_INV          "CALIB_VOLTS_INV"
#define ID_CALIB_VOLTS_5V           "CALIB_VOLTS_5V"

#define ID_CLEAR_ERROR				"CLEAR_ERROR"

#define ID_STATUS					"STATUS"
#define ID_FRIDGE_TEMP              "FRIDGE_TEMP"
#define ID_COMP_TEMP                "COMP_TEMP"
#define ID_EXTRA_TEMP               "EXTRA_TEMP"
#define ID_MECH_THERM               "MECH_THERM"
#define ID_COMP_RPM                 "COMP_RPM"
#define ID_INV_ERROR                "INV_ERROR"
#define ID_INV_PLUS                 "INV_PLUS"
#define ID_INV_FAN                  "INV_FAN"
#define ID_VOLTS_INV				"VOLTS_INV"
#define ID_VOLTS_5V					"VOLTS_5V"

// fridge UI config/state values

#define ID_BACKLIGHT_SECS			"BACKLIGHT_SECS"
#define ID_LED_BRIGHTNESS			"LED_BRIGHTNESS"

#define ID_CHART_LINK				"CHART"




extern enumValue fridgeModes[];
	// in fridgeController.ino


class Fridge : public myIOTDevice
{
public:

    Fridge();
    ~Fridge() {}

    virtual void setup() override;
    virtual void loop() override;

    // fridge config values

	static int 		_fridge_mode;
	static int 		_user_rpm;
	static int		_min_rpm;
	static int		_max_rpm;
	static String	_fridge_sense_id;
	static String	_comp_sense_id;
	static String	_extra_sense_id;
	static int		_temp_sense_secs;
	static int		_inv_sense_ms;
	static float	_setpoint_high;
	static float	_setpoint_low;
	static float	_calib_volts_inv;
	static float	_calib_volts_5v;

	// fridge state values

	static String	_status_str;
	static float	_fridge_temp;
	static float	_comp_temp;
	static float	_extra_temp;
	static bool 	_mech_therm;
	static int		_comp_rpm;
	static int		_inv_error;
	static bool		_inv_plus;
	static bool		_inv_fan;
	static bool		_inv_compress;
	static float	_volts_inv;
	static float	_volts_5v;

	// ui config/state values

	static int		_backlight_secs;
	static int		_led_brightness;
	static String 	_chart_link;

	// public states available to other objects

	static bool		m_log_error;			// low level error calling myIOTDataLog::addRecord()
	static int 		m_fridge_temp_error;	// low level error getting fridge temperature
	static int 		m_comp_temp_error;		// low level error getting compressor temperature
	static int 		m_extra_temp_error;		// low level error getting extra temperature
	static bool		m_force_pixels;			// to force pixels redraw in onSetBrightness()

	// raw sensor values for logging

	static int16_t	m_raw_fridge_temp;
	static int16_t	m_raw_comp_temp;
	static int16_t	m_raw_extra_temp;

	// methods

	void setRPM(int rpm);
	void stateMachine();
	static void stateTask(void *param);
	static void onSetPointChanged(const myIOTValue *value, float val);
	static void onBacklightChanged(const myIOTValue *value, int val);
	static void onBrightnessChanged(const myIOTValue *desc, uint32_t val);
	static void clearInvError();
		// clears the DIAG flashing LED when FRIDGE_MODE==OFF
		// by toggling the PWM on for 1/2 second. See implementation.

	// extensions
	
	String onCustomLink(const String &path,  const char **mime_type) override;
    virtual bool hasPlot() override    { return true; }


};


extern Fridge *fridge;





//=========================================================
// external utilities
//=========================================================

extern int rpmToDuty(int rpm);
	// in fridge.cpp


