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

#include <Arduino.h>


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

	int getRecordSize()  			{ return m_rec_size; }
	int getNumRecords()  			{ return m_num_recs; }

	// the following implement the current
	// temporary circ buffer implementation

	void init(int num_mem_recs);
	void addRecord(const logRecord_t rec);
	const logRecord_t getRecord(int i);

	// myIOTWebServer interface
	// Called from Fridge::onCustomLink()

	String getChartHeader();
		// returns a String containing the json used to create a chart
	String sendChartData();
		// sends the chart data to the myiot_web_server and returns
		// RESPONSE_HANDLED


private:

	const char *m_name;

	// note that the following members MUST
	// pack equivilantly to a logHeader_t record

	int m_num_cols;
	int m_rec_size;
	logColumn_t *m_col;

	// temporary implementation

	int m_head;
	int m_tail;
	int m_num_recs;
	int m_num_alloc;
	uint8_t *m_rec_buffer;

	logRecord_t m_rec(int i)
		// the raw ith recorfd in the m_rec_buffer
	{
		return (logRecord_t) &m_rec_buffer[i * m_rec_size];
	}
	

};
