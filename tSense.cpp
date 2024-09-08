//---------------------------------------------
// tSense.cpp
//---------------------------------------------
// Implements my generic interface to DS18B20 temperature sensors.
// and serves as a registry of all DS18B20 that I have purchased and
// labelled, that are numbered from one to infinity.
//
// The DallasTemperature library is poorly documented and
// 		can literally take several seconds when used as
//		demonstrated in most tutorials.
// It was not clear how to use the library most efficiently.
// Devices are only enumerated once, but may go offline.
// It can take upto 750ms for a full 12 bit conversion,
// 		in addition to the time spent sending bits out, and
// 		getting them back from the oneWire bus.
// The use of the library asynchronously may only be found
//		anecdotally on the web.
// So I rewrote it for me.

#include "tSense.h"
#include "fridge.h"
#include <myIOTLog.h>


#define PENDING_TIMEOUT	800
	// 750 plus some wiggle room



//----------------------
// defines
//----------------------

// Model IDs
#define DS18S20MODEL 	0x10  // also DS1820
#define DS18B20MODEL 	0x28  // also MAX31820
#define DS1822MODEL  	0x22
#define DS1825MODEL  	0x3B
#define DS28EA00MODEL 	0x42

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy scratchpad to EEPROM
#define READSCRATCH     0xBE  // Read from scratchpad
#define WRITESCRATCH    0x4E  // Write to scratchpad
#define RECALLSCRATCH   0xB8  // Recall from EEPROM to scratchpad
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// DSROM FIELDS
#define DSROM_FAMILY    0
#define DSROM_CRC       7

// Device resolution
#define TEMP_9_BIT  	0x1F //  9 bit
#define TEMP_10_BIT 	0x3F // 10 bit
#define TEMP_11_BIT 	0x5F // 11 bit
#define TEMP_12_BIT 	0x7F // 12 bit


typedef uint8_t ScratchPad[9];
const ScratchPad  empty_pad = {0,0,0,0,0,0,0,0,0};



//-------------------------------------------
// static stuff
//-------------------------------------------

const DeviceAddress MY_TSENSOR_01 = {0x28, 0x96, 0x08, 0xa1, 0x9d, 0x23, 0x0b, 0x89};
const DeviceAddress MY_TSENSOR_02 = {0x28, 0xd3, 0x80, 0x90, 0xc3, 0x23, 0x06, 0x57};
const DeviceAddress MY_TSENSOR_03 = {0x28, 0x3a, 0x0f, 0xe0, 0xc0, 0x23, 0x09, 0x41};
const DeviceAddress MY_TSENSOR_04 = {0x28, 0x66, 0x13, 0x0a, 0x9e, 0x23, 0x0b, 0x2e};
const DeviceAddress MY_TSENSOR_05 = {0x28, 0x60, 0x81, 0xdb, 0xc0, 0x23, 0x09, 0x15};


static const uint8_t *KNOWN_SENSORS[] = {
	MY_TSENSOR_01,
	MY_TSENSOR_02,
	MY_TSENSOR_03,
	MY_TSENSOR_04,
    MY_TSENSOR_05,
};

#define NUM_KNOWN_SENSORS		(sizeof(KNOWN_SENSORS)/sizeof(uint8_t *))



static int findKnownSensor(const DeviceAddress addr)
{
	for (int i=0; i<NUM_KNOWN_SENSORS; i++)
	{
		if (!memcmp(addr,KNOWN_SENSORS[i],8))
			return i+1;
	}
	LOGE("Could not find KNOWN_SENSOR(%s)",tSense::addrToStr(addr));
	return 0;
}



int16_t calculateRaw(const DeviceAddress addr, const uint8_t* scratchPad)
	// returns fixed-point temperature, scaling factor 2^-7
{
	int16_t raw =
		(((int16_t) scratchPad[TEMP_MSB]) << 11) |
		(((int16_t) scratchPad[TEMP_LSB]) << 3);

	// modified untested code for DS1820 and DS18S20
	// from original DallasTemperature.cpp

	if (addr[DSROM_FAMILY] == DS18S20MODEL &&
		scratchPad[COUNT_PER_C] != 0)
	{
		raw = ((raw & 0xfff0) << 3) - 32;
		int remain = (scratchPad[COUNT_PER_C] - scratchPad[COUNT_REMAIN]) << 7;
		raw += remain / scratchPad[COUNT_PER_C];
	}

	return raw;
}



// static
const char *tSense::addrToStr(const DeviceAddress addr)
{
	static char buf[80];
	for (int i=0; i<8; i++)
	{
		sprintf(&buf[i*6],"0x%02x%s",addr[i],i<7?", ":"");
	}
	return buf;
}



//---------------------------------------------
// API
//---------------------------------------------

int tSense::init()
{
	LOGD("tSense::init()",0);
	proc_entry();

	m_last_error = 0;

	DeviceAddress addr;

	int num_found = 0;
	m_wire->reset_search();
	while (m_wire->search(addr))
	{
		if (OneWire::crc8(addr, 7) == addr[DSROM_CRC])
		{
			bool valid_family =
				addr[DSROM_FAMILY] == DS18S20MODEL ||
				addr[DSROM_FAMILY] == DS18B20MODEL ||
				addr[DSROM_FAMILY] == DS1822MODEL  ||
				addr[DSROM_FAMILY] == DS1825MODEL  ||
				addr[DSROM_FAMILY] == DS28EA00MODEL;

			if (valid_family)
			{
				num_found++;

				int known = findKnownSensor(addr);
				int res = getResolution(addr);
				LOGD("known(%d) res(%d) {%s} ",
					known,res,addrToStr(addr));
			}
			else
			{
				LOGW("tSense.cpp INVALID FAMILY: %s",addrToStr(addr));
					// only a warning on the assumption the bus might
					// contain other valid OneWire devices
			}
		}
		else
			tsenseError(TSENSE_ERROR_BAD_ADDR,addr);
	}

	if (!m_last_error && !num_found)
		tsenseError(TSENSE_ERROR_NO_DEVICES,NULL);
	
	measure();
	proc_leave();

	return m_last_error;

}


bool tSense::pending()
{
	if (m_pending && millis() - m_pending > PENDING_TIMEOUT)
		m_pending = 0;
	return m_pending;
}



int tSense::measure()
	// Reports an error and returns false if a measurement is pending.
	// Reports an error and returns false on any oneWire errors.
	// Starts a measurement on all devices on the bus.
	// Sets the m_pending timer.
{
	if (pending())
		return tsenseError(TSENSE_ERROR_PENDING,NULL);

	if (!m_wire->reset())
		return tsenseError(TSENSE_ERROR_NO_DEVICES,NULL);

	m_wire->skip();
	m_wire->write(STARTCONVO, 0);	// 0 == parasite
	m_pending = millis();

	return TSENSE_OK;
}



float tSense::getDegreesC(const DeviceAddress addr)
{
	if (pending())
	{
		tsenseError(TSENSE_ERROR_PENDING,addr);
		return TEMPERATURE_ERROR;
	}
	ScratchPad scratch_pad;
	if (readScratchPad(addr, scratch_pad) != TSENSE_OK)
		return TEMPERATURE_ERROR;
	int raw = calculateRaw(addr,scratch_pad);
	// C = RAW/128
	return (float) raw * 0.0078125f;
}

float tSense::getDegreesF(const DeviceAddress addr)
{
	if (pending())
	{
		tsenseError(TSENSE_ERROR_PENDING,addr);
		return TEMPERATURE_ERROR;
	}
	ScratchPad scratch_pad;
	if (readScratchPad(addr, scratch_pad) != TSENSE_OK)
		return TEMPERATURE_ERROR;
	int raw = calculateRaw(addr,scratch_pad);
	// F = (C*1.8)+32 = (RAW/128*1.8)+32 = (RAW*0.0140625)+32
	return ((float) raw * 0.0140625f) + 32.0f;
}



//-------------------------------------------------
// private
//-------------------------------------------------

int tSense::tsenseError(int err_code, const DeviceAddress addr)
{
	LOGE("TSENSE_ERROR(%d) addr(%s)",err_code,addr?addrToStr(addr):"");
	m_last_error = err_code;
	return err_code;
}


int tSense::getResolution(const DeviceAddress addr)
{
	// DS1820 and DS18S20 have no resolution configuration register
	if (addr[DSROM_FAMILY] == DS18S20MODEL)
		return 12;

	ScratchPad scratch_pad;
	if (readScratchPad(addr, scratch_pad) != TSENSE_OK)
		return 0;

	switch (scratch_pad[CONFIGURATION])
	{
		case TEMP_12_BIT:
			return 12;

		case TEMP_11_BIT:
			return 11;

		case TEMP_10_BIT:
			return 10;

		case TEMP_9_BIT:
			return 9;
	}

	tsenseError(TSENSE_ERROR_BAD_CONFIG,addr);
	return 0;
}



int tSense::readScratchPad(const DeviceAddress addr, uint8_t *scratch_pad)
{
	if (!m_wire->reset())
		return tsenseError(TSENSE_ERROR_NO_DEVICES,addr);

	m_wire->select(addr);
	m_wire->write(READSCRATCH);

	for (uint8_t i = 0; i < 9; i++)
	{
		scratch_pad[i] = m_wire->read();
	}

	if (!m_wire->reset())
		return tsenseError(TSENSE_ERROR_OFFLINE,addr);
	if (!memcmp(scratch_pad,empty_pad,9))
		return tsenseError(TSENSE_ERROR_EMPTY_DATA,addr);
	if (OneWire::crc8(scratch_pad, 8) != scratch_pad[SCRATCHPAD_CRC])
		return tsenseError(TSENSE_ERROR_BAD_CRC,addr);

	return TSENSE_OK;
}




