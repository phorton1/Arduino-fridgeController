//-------------------------------------------------
#pragma once
#include "fridge.h"
#include <myIOTLog.h>


class fakeCompressor
{
public:

    static void run();     // at approximately 1/sec

    static void init()
    {
        LOGW("fakeCompressor::init(%0.3f)",_ambient);
        g_fridge_temp = 0;
        g_comp_temp = _ambient;
    }

    // feature parameters

    static bool _compressor;    // whether to use this or not
    static bool _comp_on;       // whether or not it's turned on
    static int  _prob_error;    // probability of a start up error, integer, default 3

    // configuration parameters (FAKE_ values)

    static float _ambient;      // ambient temperature, default(80F) in Centigrade
    static float _insulate;     // insulation coefficient ???
    static float _cooling;      // cooling coefficient ???
    static float _heating;      // heating coefficient ???
    static float _cooldown;     // the heater cooldown coefficient
    
    // output parameters, sampled to tSense.cpp and vSense.cpp

    static float g_fridge_temp;
    static float g_comp_temp;

	static int g_sample_plus;
	static int g_sample_fan;
	static int g_sample_diode;

    // working parameters
    
private:

    static void init_for_start();

};






