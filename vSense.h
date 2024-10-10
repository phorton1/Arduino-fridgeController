//-----------------------------------------------
// vSense.h - analog voltage sensors
//-----------------------------------------------
// Circular buffer sensing of ESP32 analog in pins
// for the inverter's FAN/DIODE+, FAN-, DIODE- voltage
// divider outputs and a divider for the mini360 5v buck
// convertor.
//
// Thresholds on the inverter voltage sensors turn the
// POWER, FAN, and DIODE (green, yellow, red) on when
// those change.
//
// Additionally encapsulates the 'error state' of the
// inverter by counting dignostic LED flashes with knowledge
// of the inverter's 'restart' behavior which implicitly
// clears the error state.

#pragma once


class vSense
{
public:

	vSense()	{}
	void		init();			// call from setup()
	void		sense();		// call 20 times per second

	int			_error_code;	// current # flashes on inv DIAG_DIOD
	bool		_diag_on;
	bool 		_plus_on;		// 1 if inverter has power
	bool		_fan_on;		// 1 if fan is on

	float		_volts_inv;		// voltage plus as proxy for inverter voltage, to 1 decimal place
	float		_volts_5V;		// buck converter output to 1 decimal place

private:
};

extern vSense v_sense;


