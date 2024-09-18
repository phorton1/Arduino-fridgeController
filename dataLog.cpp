//-----------------------------------------------
// dataLog.cpp - data logging object
//-----------------------------------------------
// As far as I can tell, there is actually only one ABSOLUTE_SCALE for a plot,
// determined by the 1st y-axis definition.  Then, weirdly, the other axis
// scale LABELS can be changed, but their values are always charted against
// the ABSOLUTE_SCALE, so the data must be normalized.
//
// This gives me two choices, I think :
//
//		normalize all subsquent column data to the 0th column.
//			In other words, if col0 is temp1, and temp1 goes from
//			 -32 to -20, then all the other columns will be scaled
// 			from -32 to -20 the temperature is from, even though
//			their min&max scales on the yaxisN are arbitrary
//
//  or more sanely:
//
//		try to use a hidden main yaxis with min0 and max0,
//			and then scale all the values within their own
//			min and max values.

#include "dataLog.h"
#include "fridge.h"
#include <myIOTLog.h>
#include <myIOTWebServer.h>



dataLog::dataLog(
		const char *name,
		int num_cols,
		const logColumn_t *cols ) :
	m_name(name),
	m_num_cols(num_cols),
	m_col(cols)
{
	m_rec_size = (1 + m_num_cols) * sizeof(uint32_t);

	// temporary implementation

	m_head = 0;
	m_tail = 0;
	m_num_recs = 0;
	m_num_alloc = 0;
	m_rec_buffer = NULL;

}



void dataLog::init(int num_mem_recs)
{
	m_num_alloc = num_mem_recs;
	int bytes = m_num_alloc * m_rec_size;
	LOGI("dataLog(%s) init(%d) will require %d bytes",
		 m_name,
		 m_num_alloc,
		 bytes);
	m_rec_buffer = new uint8_t[bytes];
	memset(m_rec_buffer,0,bytes);
}




void dataLog::addRecord(const logRecord_t in_rec)
{
	LOGD("    dataLog::addRecord(%d) alloc=%d head=%d tail=%d",
		m_num_recs,
		m_num_alloc,
		m_head,
		m_tail);
	m_num_recs++;
	if (m_num_recs > m_num_alloc)
		m_num_recs = m_num_alloc;

	int at = m_head++;
	if (m_head >= m_num_alloc)
		m_head = 0;
	if (m_tail == m_head)
	{
		m_tail++;
		if (m_tail >= m_num_alloc)
			m_tail = 0;
	}

	LOGV("   adding at(%d) new_head(%d) new_tail(%d)",at,m_head,m_tail);
	logRecord_t new_rec = m_rec(at);
	new_rec[0] = in_rec[0];
	memcpy(&new_rec[1],&in_rec[1],m_num_cols * sizeof(uint32_t));

	// debugging

	#if 0
		String tm = timeToString(new_rec[0]);
		LOGD("   dt = %s",tm.c_str());
		for (int i=0; i<m_num_cols; i++)
		{
			uint32_t col_type = m_col[i].type;
			if (col_type == LOG_COL_TYPE_FLOAT)
			{
				float float_val = *((float*)&new_rec[i+1]);
				LOGD("   %-15s = %0.3f",m_col[i].name,float_val);
			}
			else if (col_type == LOG_COL_TYPE_INT32)
			{
				int32_t int32_val = *((int32_t*)&new_rec[i+1]);
				LOGD("   %-15s = %d",m_col[i].name,int32_val);
			}
			else
			{
				uint32_t val = new_rec[i+1];
				LOGD("   %-15s = %d",m_col[i].name,val);
			}
		}
	#endif
}




const logRecord_t dataLog::getRecord(int i)
{
	if (!m_num_recs)
		return NULL;
	
	// normalize 0..i to circular buffer

	if (i < 0) i = 0;
	if (i > m_num_recs-1)
		i = m_num_recs-1;
		
	int at = m_tail + i;
	if (at > m_num_alloc - 1)
		at -= m_num_alloc;

	return m_rec(at);
}



void addJsonVal(String &rslt, const char *field, String val, bool quoted, bool comma, bool cr)
{
	rslt += "\"";
	rslt += field;
	rslt += "\":";
	if (quoted) rslt += "\"";
	rslt += val;
	if (quoted) rslt += "\"";
	if (comma) rslt += ",";
	if (cr) rslt += "\n";
}


String dataLog::getChartHeader()
{
	String rslt = "{\n";

	addJsonVal(rslt,"name",m_name,true,true,true);
	addJsonVal(rslt,"num_cols",String(m_num_cols),false,true,true);

	rslt += "\"col\":[\n";

	for (int i=0; i<m_num_cols; i++)
	{
		if (i) rslt += ",";
		rslt += "{";

		const logColumn_t *col = &m_col[i];
		const char *str =
			col->type == LOG_COL_TYPE_FLOAT ? "float" :
			col->type == LOG_COL_TYPE_INT32 ? "int32_t" :
			"uint32_t";

		addJsonVal(rslt,"name",col->name,		true,true,false);
		addJsonVal(rslt,"type",str,				true,true,false);
		addJsonVal(rslt,"min",String(col->min),	false,true,false);
		addJsonVal(rslt,"max",String(col->max),	false,false,false);

		rslt += "}\n";
	}
	rslt += "]\n";
	rslt += "}";

	// Serial.println(rslt.c_str());
	return rslt;
}



String dataLog::sendChartData()
{
	int content_len = 4 + m_num_recs * m_rec_size;
		// 4 byte m_num_recs followed by records

	if (!m_num_recs)
		LOGW("getChartData() sending empty response!");
	else
		LOGI("getChartData() sending %d records in %d bytes",m_num_recs,content_len);

	if (myiot_web_server->startBinaryResponse("application/octet-stream", content_len))
	{
		if (myiot_web_server->writeBinaryData((const char *)&m_num_recs,4))
		{
			bool ok = true;
			for (int i=0; i<m_num_recs; i++)
			{
				logRecord_t rec = getRecord(i);

				if (!myiot_web_server->writeBinaryData((const char *)rec,m_rec_size))
				{
					ok = false;
					i = m_num_recs;
				}
			}
			// if (ok)
			// 	myiot_web_server->finishBinaryResponse();
		}
	}
	return RESPONSE_HANDLED;
}




