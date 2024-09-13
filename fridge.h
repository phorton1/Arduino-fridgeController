//----------------------------------------------
// constants for fridgeController
//----------------------------------------------
// uses ESP32 standard SPI mosi(23) miso(19) sclk(18)
// and I2C scl(22) sda(21)

#pragma once

#include <myIOTDevice.h>

// global debugging defines

#define PLOT_VALUES 		0		// to output only vSense integers for Arduino plotter
#define TEMP_BREADBOARD		1

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


#if TEMP_BREADBOARD
	#define PIN_LED_BLUE		2		// onboard led
	#define PIN_S_PLUS			35
	#define PIN_S_FAN			34
	#define PIN_S_DIODE 		39
#endif


#define LED_COMPRESS_ON		PIN_LED_BLUE
#define LED_DIODE_ON 		PIN_LED_RED
#define LED_POWER_ON		PIN_LED_GREEN
#define LED_FAN_ON			PIN_LED_YELLOW



//------------------------
// myIOT definition
//------------------------

#define FRIDGE_CONTROLLER			"fridgeController"
#define FRIDGE_CONTROLLER_VERSION	"fc0.1"
#define FRIDGE_CONTROLLER_URL		"https://github.com/phorton1/Arduino-fridgeController"

// ids

#define ID_TEMPERATURE_1           	"TEMPERATURE_1"
#define ID_TEMP_ERROR           	"TEMP_ERROR"

#define ID_INV_ERROR           		"INV_ERROR"
#define ID_INV_PLUS           		"INV_PLUS"
#define ID_INV_FAN           		"INV_FAN"
#define ID_INV_COMPRESS           	"INV_COMPRESS"



class Fridge : public myIOTDevice
{
public:

    Fridge();
    ~Fridge() {}

    virtual void setup() override;
    virtual void loop() override;

    static const valDescriptor m_fridge_values[];

    // values

	static float	_temperature1;
	static int		_temp_error;

	static int		_inv_error;
	static bool 	_inv_plus;
	static bool		_inv_fan;
	static bool		_inv_compress;

	// methods

	void stateMachine();
	static void stateTask(void *param);

	// experiments
	
	String onCustomLink(const String &path,  const char **mime_type) override;

};


extern Fridge *fridge;





//=========================================================
// external utilities in fridgeUtils.cpp
//=========================================================

extern int rpmToDuty(int rpm);

