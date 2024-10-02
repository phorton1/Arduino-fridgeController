//---------------------------------------------
// tSense.h
//---------------------------------------------
// My interface to DS18B20 temperature sensors, based
// on original DallasTemperature Arduino library.
//
// - Does not (?) support parasite power.
// - Does not support alarms.
// - Assumes a bus, and is not optimized for a single sensor.
// - Cannot currently be used to set the resolution of programmable devices.
//		all of mine default to 12 bits (750ms required)
//
// In this implementation, you MUST know the 8 byte address of the
//		sensors you are using.
// init() will display() the addresses and resolution of all devices
//		on the bus. You may then copy-paste those back into tSense.cpp
//		and/or your applictication code as needed.
// init() will also does the initial measure() call.
//
// The usage model is to check the temperatures no more often than
// every N seconds, where N > the number of devices on the bus.
//
// Every N seconds:
//
//      if (!pending())
//		{
//			getDegreesF() or getDegreesC() for each address.
//				This will actually give you the PREVIOUS measurement.
// 			measure().  This will send out the command
//				for all temperature sensors to do a measurement
//      		and set the m_pending 800 ms timer for the
//				next time through.
//		}
//
// Note that Devices may go off/online due to faulty wiring.


#pragma once

#include <OneWire.h>

// Don't use the float if it >= TEMPERATURE_ERROR

#define TEMPERATURE_ERROR  		10000.0
	// the temperature returned if there are any problems
	// during getDegreesC()

// Error Codes - see implementation for details

#define TSENSE_OK					0
#define TSENSE_ERROR_NO_DEVICES		1
#define TSENSE_ERROR_OFFLINE		2
#define TSENSE_ERROR_EMPTY_DATA		3
#define TSENSE_ERROR_BAD_ADDR		4
#define TSENSE_ERROR_BAD_CRC		5
#define TSENSE_ERROR_BAD_CONFIG		6
#define TSENSE_ERROR_PENDING		7



typedef uint8_t DeviceAddress[8];



class tSense
{
public:

	tSense(OneWire *oneWire) :
		m_wire(oneWire),
		m_pending(0),
		m_last_error(0)
	{}

	int init();
		// returns TSENSE_OK or reports error and returns error code
		// calls myDebug::display() to show the addresses
		// 		of all devices on the bus.
		// makes the initial measure() call.
		// can return errors but they *msy* be ignored, since
		// each measurement is a new oneWire request
	bool pending();
		// returns true if a measurement is pending
		// 800ms timeout period.

	int measure();
		// returns TSENSE_OK or reports error and returns error code
		// Starts a measurement on all devices on the bus.
		// Sets the m_pending timer.

	float getDegreesC(String str_addr);
		// Return the temperature for a given sensor
		// or TEMPERATURE_ERROR if pending or any problems
	int getLastError() { return m_last_error; }
		// call if any method fails

private:

	OneWire *m_wire;
	uint32_t m_pending;
	int m_last_error;

	int getResolution(const DeviceAddress addr);
		// returns 9..12 or 0 upon an error
	int readScratchPad(const DeviceAddress addr, uint8_t *scratch_pad);
		// returns TSENSE_OK or reports and returns error code
	int tsenseError(int err_code, const DeviceAddress addr);
		// reports and returns the error; addr may be NULL

};




//-----------------------------------------------------------
// My devices
//-----------------------------------------------------------
// For convenience, tSense.cpp enumerates all the DS18B20's
// I have purchased and displays their position in init().

extern const char *MY_TSENSOR_01;
extern const char *MY_TSENSOR_02;
extern const char *MY_TSENSOR_03;
extern const char *MY_TSENSOR_04;
extern const char *MY_TSENSOR_05;



// end of tSense.h

