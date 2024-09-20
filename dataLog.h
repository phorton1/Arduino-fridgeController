//-----------------------------------------------
// dataLog.h - data logging object
//-----------------------------------------------
// rather than send the data in a raw jqplot ready format
// which means sending a bunch of text, redundant at that,
// we send the raw binary data for each record.
//
// This object will eventually have a more complicated API,
// splitting the responsibility for graph scales between
// the app specific C++ and generic HTML/javascript.
//
// Normalization and magic tricks to get jqPlot to display
// decent scales are currently all performed in the javascript.
// Currently the JS picks scales based on the intervals so
// that all ticks line up on the chart, but this wastes visual
// space. Other alternatives include:
//
// - automatically creating tick marks that are not lined up,
//   (but grid lines only on the 0th y axis) that maximize the
//   displayed accuracy
// - having the app explicitly determine the scales with
//   min and max members per column and API on this object.
// - allowing the app and/or user to specify if the zooming
//   javascript should be included.
// - allowing the user to specify, in the HTML, which cols
//   they want to display
// - allowing the user to specify the period of time they
//   wish to look at, based on ..
// - storing the data on the SD card with or without an
//   additional date-index file of some sort.
// - generally addressing memory usage in myIOT apps
// - changing to an aynchronous myIOTWebServer, which would
//   require a lot of reworking.
//
// I am checking this in as-is because it is a pretty big
// advancement.


#pragma once

#include <myIOTTypes.h>
#include <SD.h>

#if !WITH_SD
	#error dataLog requires myIOT define WITH_SD=1 !!
#endif



#define LOG_COL_TYPE_UINT32		0x00000001
#define LOG_COL_TYPE_INT32		0x00000002
#define LOG_COL_TYPE_FLOAT		0x00000004

// the tick_intervals are used to determine the
// min/max and num_ticks in the javascript.

typedef struct {
	const char *name;			// provided by caller
	uint32_t type;				// provided by caller
	float tick_interval;		// provided by caller
	// float min;
	// float max;
} logColumn_t;


typedef uint32_t *logRecord_t;
	// an array of uin32's where the 0th the dt in unix format
	// followed by num_cols 32bit values


class dataLog
{
public:

	dataLog(
		const char *name,
		int num_cols,
		logColumn_t *cols );
	bool init(int num_mem_recs = 10);
		// Will report an error and return false
		// if there is no SD Card.  The size of the
		// memory queue is determined by the application
		// based on it's likely addRecord:flushToSD() ratio,
		// where 10 is a good default.

	bool addRecord(const logRecord_t rec);
		// Quickly add a record to a in-memory queue to be
		// written to the SD Card later.  The queue can
		// hold a limited number of records before
		// flushToSD() must be called.  Will report an error
		// and return false if there is a buffer overflow.
		// Will report an error an return false if the timestamp
		// at the front of the record is not a valid current time,
		// to prevent writing 1970 log records.
		// Typically called from myIOTDevice's stateMachine task.
	bool flushToSD();
		// Flush the in-memory queue (Slow) to the SD card.
		// Will NOT block calls to addRecord().
		// Will be added to myIOTDevice::loop() at some point.
		// Currently called from myIOTDevice's loop() method
		// BEFORE it calls myIOTDevice::loop() so that sendChartData()
		// works entire from the SD card.

	// myIOTWebServer interface
	// Called from the myIOTDevice's onCustomLink()

	String getChartHeader();
		// returns a String containing the json used to create a chart
	String sendChartData(uint32_t num_recs);
		// sends the chart data to the myiot_web_server and returns
		// RESPONSE_HANDLED. num_recent_recs is determined by the
		// the client and their logging rate, where 0 means "all"


private:

	const char *m_name;
	int m_num_cols;
	int m_rec_size;
	logColumn_t *m_col;

	// in-memory queue

	int m_num_alloc;
	uint8_t *m_rec_buffer;
	volatile int m_head;
	volatile int m_tail;

	logRecord_t mem_rec(int i)
	{
		return (logRecord_t) &m_rec_buffer[i * m_rec_size];
	}
	
	String dataFilename();
	bool writeSDRecs(File &file, const char *what, int at, int num_recs);


};
