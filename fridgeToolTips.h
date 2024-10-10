// fridgeToolTips.h

static const char *fridge_tooltips[] = {

	ID_FRIDGE_MODE,
		"default(Off) - "
		"Sets one of several alternative modes of operation for the controller.",
	ID_SETPOINT_HIGH,
		"default(-12C) - "
		"Temperature above which the compressor will be started in "
		"the RUN_TEMP mode.",
	ID_SETPOINT_LOW,
		"default(-20C) - "
		"Temperature below which the compressor will be turned off in "
		"the RUN_TEMP mode.",
	ID_USER_RPM,
		"default(2600) - "
		"Sets the RPM at which the compressor will normally run in MECH or TEMP modes",
	ID_MIN_RPM,
		"default(2000) - "
		"Sets the minimum RPM at which the compressor may run.  Used in MIN mode",
	ID_MAX_RPM,
		"default(3200) - "
		"Sets the maximum RPM at which the compressor may run.  Used in MAX mode",
	ID_FRIDGE_SENSE_ID,
		"The ID of the DS18B20 temperature sensor that is considered "
		"the 'main' sensor used for controlling the compressor"
		"The default value is currently set to the known ID "
		"of a DS18B20 sensor that I own, and a Factory Reset "
		"will wipe out any user entered FRIDGE_SENSE_ID.",
	ID_COMP_SENSE_ID,
		"The ID of the DS18B20 temperature sensor "
		"that is used to monitor the temperature of the compressor",
	ID_EXTRA_SENSE_ID,
		"The ID of an extra DS18B20 that is not used for anything yet",
	ID_TEMP_SENSE_SECS,
		"default(30), minimum(10) - "
		"How often in seconds, to read the temperature sensors",
	ID_INV_SENSE_MS,
		"default(20) - "
		"How often, in milliseconds, to read the voltages from the inverter "
		"to determine the state of the compressor",
	ID_CALIB_VOLTS_INV,
		"default(1.0) - "
		"Calibration factor used in reporting the VOLT_INV value",
	ID_CALIB_VOLTS_5V,
		"default(1.0) - "
		"Calibration factor used in reporting the VOLT_5V value",

	ID_STATUS,
		"String that shows various error states, esp. TSENSE_ERRORS ",
	ID_FRIDGE_TEMP,
		"Temperature of the the DS18B20 at FRIDGE_SENSE_ID",
	ID_COMP_TEMP,
		"Temperature of the DS18B20 at COMP_SENSE_ID",
	ID_EXTRA_TEMP,
		"Temperature from the DS18B20 at EXTRA_SENSE_ID",
	ID_MECH_THERM,
		"Raw 0/1 digitalRead() of the value of the mechanical thermostat",
	ID_COMP_RPM,
		"Current RPM (PWM signal) that the Controller is sending to the Inverter",
	ID_INV_ERROR,
		"The number of flashes the inverter DIAG_DIODE (the Red LED on both PCBs) "
		"is currently flashing",
	ID_INV_PLUS,
		"Set when the Inverter Power supply is On",
	ID_INV_FAN,
		"Set when the FAN is On",
	ID_VOLTS_INV,
		"The actual voltage measured for FAN/DIODE+ pin "
		"which serves as a proxy for the compressor power supply ",
	ID_VOLTS_5V,
		"The voltage measured at the 5V pin on the ESP32",

	ID_CLEAR_ERROR,
		"Clears the INV_ERROR (diag diode flashes) by briefly "
		"turning the RPMs On and Off",

	ID_BACKLIGHT_SECS,
		"default(15) max(120) -  "
		"How long the LCD back light will stay on after booting or any button press. "
		"A value of 120 means 'forever', never turn the backlight off",
	ID_LED_BRIGHTNESS,
		"default(30) max(254) - "
		"Sets the brightness of the external WS2812B LEDs.  Note that "
		"values under 5 turn the LED off completely, and certain colors "
		"are not visible under values of 10, so 10 is the effective useful "
		"minimum value of this value. ",

#if WITH_FAKE_COMPRESSOR
	ID_USE_FAKE,		"Whether to use the fakeCompressor or not, which provides fake temperatures and voltages to the system",
	ID_RESET_FAKE,		"A command to reset the fake Compressor to some known initial temperatures",
	ID_FAKE_COMP_ON,	"Emulates turning the 12V power supply Off or On",
	ID_FAKE_AMBIENT, 	"default(26.67C) - the ambient temperature used for the model",
	ID_FAKE_PERIOD,		"default(30) - approximate number of seconds for a complete cooling cycle",
	ID_COOLING_ACCEL,	"How fast the fridge cools when compressor on",
	ID_WARMING_ACCEL,	"How fast the fridge warms when compressor off",
	ID_HEATING_ACCEL,	"How fast compressor heats up when on",
	ID_COOLDOWN_ACCEL,	"How fast compressor cools down when off",
	ID_MAX_COOL_VEL,	"Maximum rate of fridge cooling",
	ID_MAX_WARM_VEL,	"Maximum rate of fridge warming",
	ID_MAX_HEAT_VEL,	"Maximum rate of compressor heating",
	ID_MAX_DOWN_VEL,	"Maximum rate of compressor cooldown",
	ID_FAKE_PROB_ERROR,	"not implemented yet",
#endif

	0 };	// fridge_tooltips


// end of fridgeToolTips.h
