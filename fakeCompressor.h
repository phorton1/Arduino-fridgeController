//-------------------------------------------------
#pragma once
#include "fridge.h"
#include <myIOTLog.h>

// if _use_fake is true then tSense and vSense get their data
// from this object.  If it is false, then the actual tSense and
// vSense data is used. Changing the state of _use_fake calls init(),
// as does the RESET_FAKE button.


class fakeCompressor
{
public:

    static void run();     // at approximately 1/sec

    static void init()
        // called directly from RESET_FAKE command button
    {
        LOGW("fakeCompressor::init(%0.3f)",_ambient);
        g_fridge_temp = -16;    // start in middle of default setpoint range
        g_comp_temp = _ambient;
        g_sample_plus = 0;
        g_sample_fan = 0;
        g_sample_diode = 0;
        init_for_start(1);
    }

    // feature parameters

    static bool _use_fake;      // whether to use this or not
    static bool _comp_on;       // whether or not it's turned on
    static int  _prob_error;    // probability of a start up error, integer, default 3

    // configuration parameters (FAKE_ values)

    static float _ambient;      // ambient temperature, default(80F) in Centigrade

    static int   _fake_period;      
    static float _cooling_accel;
    static float _warming_accel;
    static float _heating_accel;
    static float _cooldown_accel;
    static float _max_cool_vel;
    static float _max_warm_vel;
    static float _max_heat_vel;
    static float _max_down_vel;
    
    // output parameters, sampled to tSense.cpp and vSense.cpp

    static float g_fridge_temp;
    static float g_comp_temp;

	static int g_sample_plus;
	static int g_sample_fan;
	static int g_sample_diode;
    static int g_sample_5V;

    // working parameters
    
private:

    static void init_for_start(bool all);

};






