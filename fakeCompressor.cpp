//-------------------------------------------------
// fakeCompressor.cpp
//-------------------------------------------------
#include "fakeCompressor.h"
#include <cmath>

bool    fakeCompressor::_use_fake;      // whether to use this or not
bool    fakeCompressor::_comp_on;       // whether or not it's turned on
int     fakeCompressor::_prob_error;    // probability of a startup error default(3)
float   fakeCompressor::_ambient;       // ambient temperature, default(80F) in Centigrade

int     fakeCompressor::_fake_period;
float   fakeCompressor::_cooling_accel;
float   fakeCompressor::_warming_accel;
float   fakeCompressor::_heating_accel;
float   fakeCompressor::_cooldown_accel;
float   fakeCompressor::_max_cool_vel;
float   fakeCompressor::_max_warm_vel;
float   fakeCompressor::_max_heat_vel;
float   fakeCompressor::_max_down_vel;

float   fakeCompressor::g_fridge_temp;
float   fakeCompressor::g_comp_temp;
int     fakeCompressor::g_sample_plus;
int     fakeCompressor::g_sample_fan;
int     fakeCompressor::g_sample_diode;
int     fakeCompressor::g_sample_5V;


#define DEBUG_FAKE  0
#define DEBUG_VOLTS 0



static int      actual_rpm;
static uint32_t last_start_attempt;
static int      start_error;
static uint32_t last_flash_cycle;
static uint32_t last_flash;

static  float   fridge_vel;
static  float   comp_vel;



extern int cur_rpm;
    // in fridge.cpp


// note that g_sample_fan is pulled LOW when the
// fan is on, so it must be initialized to g_sample_on

void fakeCompressor::init_for_start(bool all)
    // init_for_start(1) called from init
    // init_for_start(0) called from actual start attempt
{
    LOGD("    init_for_start(%d)",all);

    g_sample_fan = g_sample_plus;
    g_sample_diode = 0;

    actual_rpm = 0;
    last_start_attempt = 0;
    start_error = 0;
    last_flash_cycle = 0;
    last_flash = 0;

    if (all)
    {
        fridge_vel = 0;
        comp_vel= 0;
    }
}


void fakeCompressor::run()
    // called every _inv_sense_ms (20ms) by Fridge
{
    // (1) if the whole feature turned off or on, we clear any
    // fake errors on the fridge, call init(), and if off, return

    static bool last_use_fake = 0;
    if (last_use_fake != _use_fake)
    {
        last_use_fake = _use_fake;

        fridge->m_fridge_temp_error = 0;
        fridge->m_comp_temp_error = 0;

        init();
    }
    if (!_use_fake)
        return;

    // (2) if using the feature we always occastionally
    // set a fake random 5V voltage +/- 0.5v

    uint32_t now = millis();

    #define EVERY_SO_OFTEN  3000
    bool every_so_often = 0;
    static uint32_t last_often;
    if (now - last_often > EVERY_SO_OFTEN)
    {
        last_often = now;
        every_so_often = 1;
    }

    if (every_so_often)
    {
        #define SAMPLE_5V   1910
            // empirically determined sample which will report about 5V

        int one_volt = (SAMPLE_5V/5);
        int rfactor = random(one_volt) - (one_volt/2);
        g_sample_5V = SAMPLE_5V + rfactor;
        #if DEBUG_VOLTS
            LOGD("   fake 5V(%d) rand(%d) base(%d)",g_sample_5V,rfactor,SAMPLE_5V);
        #endif
    }


    // (3) check for _comp_on changes and re-init for start if it changes

    static bool last_comp_on;
    if (last_comp_on != _comp_on)
    {
        LOGD("fakeCompressor::_comp_on(%d)",_comp_on);
        last_comp_on = _comp_on;
        init_for_start(0);
        g_sample_plus = 0;
    }


    // (4) emulate "rapid" behavior when starting/stopping
    // the compressor to emulate the voltages in that process.
    // Once the compressor is running, this section reaches a
    // steady state (with the exception of possible future random faults)

    if (_comp_on)
    {
        // set random inverter voltage +/- 0.5V
        if (!g_sample_plus || every_so_often)
        {
            #define SAMPLE_12V      2680
                // empirically determined sample which will report about 12V

            int one_volt = (SAMPLE_12V/12);
            int rfactor = random(one_volt) - (one_volt/2);
            g_sample_plus = SAMPLE_12V + rfactor;
            #if DEBUG_VOLTS
                LOGD("   fake 12V(%d) rand(%d) base(%d)",g_sample_plus,rfactor,SAMPLE_12V);
            #endif
        }

        if (!cur_rpm && (last_start_attempt || actual_rpm))     // stopped by Fridge
        {
            init_for_start(0);
            LOGI("fakeCompressor stopped by Fridge");
        }
        else if (cur_rpm && !actual_rpm && !last_start_attempt)
        {
            last_start_attempt = now;
            LOGI("fakeCompressor initiating start attempt at rpm(%d)",cur_rpm);
        }
        else if (last_start_attempt && now-last_start_attempt > 2000)
        {
            last_start_attempt = 0;
            actual_rpm = cur_rpm ? cur_rpm : 2000;

            g_sample_fan = 0;   // fan is turned on by setting sample to zero
                // we don't emulate inverter behavior.
                // we turn the yellow fan LED on when the start attempt completes

            LOGI("fakeCompressor completed start attempt rpm(%d)",actual_rpm);
        }

        // special case to handle turning the fan off when its on
        // and we are not running or trying a start attempt.
        // This happens when the FAKE_RESET command is issued
        
        else if (g_sample_plus && !g_sample_fan && !last_start_attempt && !actual_rpm)
        {
            g_sample_fan = g_sample_plus;   // turns the fan off
        }
    }
    
    // (5) Do temperature modelling once per second.
    //
    // The hard part is/was coming up with a reasonable model
    // of how the refridgerator and compressor warm up
    // or cool down based on their current temperatures
    // and the actual_rpm, and developing nice parameters
    // for controlling that process.  As a result there
    // are a bunch of if 1/0 sections for different attempts
    // to make it work better.  I LEFT IT WITH REASONABLE
    // VALUES FOR _fake_period=1 and fridge->_temp_sense_secs==1

    static uint32_t last_time = 0;
    static uint32_t prev_time = 0;
    uint32_t current_time = time(NULL);

    if (last_time != current_time)
    {
        last_time = current_time;

        if (prev_time)
        {
            // Calculate time elapsed since last iteration and a scaling factor.

            uint32_t elapsed = current_time - prev_time;
            float scale = ((float) elapsed) / ((float)_fake_period);

            // basic algorithm uses newtonian modelling with position, velocity,
            // and acelleration concepts. Each call sets
            //
            //      temp = temp_start + secs*vel + secs*accel^2

            float fridge_accel = 0;
            float comp_accel = 0;
            float cool_rpm_factor = 0;
            float heat_rpm_factor = 0;
            float comp_ambient_factor = 1;

            if (actual_rpm)     // compressor on, decrease fridge temperature and increase comp temperature
            {
                cool_rpm_factor = 1 + ((actual_rpm - 2000) / 1800) * 0.5;
                heat_rpm_factor = 1 + ((actual_rpm - 2000) / 1500) * 0.5;

                fridge_accel = -cool_rpm_factor * _cooling_accel;
                comp_accel = heat_rpm_factor * _heating_accel;

                #if 1 // OPTION TO de-accelerate more quickly on vel vs rpm mismatches
                      // creates more realistic behavior but also artifacts
                    if (fridge_vel > 0)
                        fridge_vel *= 1 - (0.5 * scale);
                    if (comp_vel < 0)
                        comp_vel *= 1 - (0.5 * scale);
                #endif

            }
            else    // compressor off, increase fridge temperature and decreate comptemperature
            {

                #if 1   // OPTION to try to smooth cooldown graph near ambient

                    #define AMBIENT_THRESHOLD  20.0

                    float dif = g_comp_temp - _ambient;
                    if (dif <= AMBIENT_THRESHOLD)
                    {
                        float ratio = dif / AMBIENT_THRESHOLD;
                        comp_ambient_factor = ratio * ratio;
                        #if DEBUG_FAKE
                            LOGD("--> dif(%0.3f) ratio(%0.3f) factor(%0.3f)",dif,ratio,comp_ambient_factor);
                        #endif
                    }
                #endif

                fridge_accel = _warming_accel;
                comp_accel = -_cooldown_accel;

                #if 1 // OPTION TO de-accelerate more quickly on vel vs rpm mismatches
                    if (fridge_vel < 0)
                        fridge_vel *= 1 - (0.2 * scale);
                    if (comp_vel > 0)
                        fridge_vel *= 1 - (0.2 * scale);
                #endif
            }

            #if 1   // OPTION to add some randomness to accel model
                float rand_fridge_factor = 1 + scale * (((float) random(40)) / 100.0) - 0.2;
                float rand_comp_factor =   1 + scale * (((float) random(40)) / 100.0) - 0.2;
                fridge_accel *= rand_fridge_factor;
                comp_accel *= rand_comp_factor;
            #endif


            #if 1   // OPTION Various attempt to get period scaling working
                fridge_accel *= ((float)_fake_period);
                comp_accel *= ((float)_fake_period);
            #elif 0
                fridge_accel *= scale;
                comp_accel *= scale;
            #endif

            // add the accelleration to the veloicty
            // and factor in the ambient effect

            fridge_vel += fridge_accel;
            comp_vel += comp_accel;
            comp_vel *= comp_ambient_factor;

            // constrain the velocities to the scaled maximums

            #if 1
                if (actual_rpm)
                {
                    if (fridge_vel < -_max_cool_vel * scale)
                        fridge_vel = -_max_cool_vel * scale;
                    if (comp_vel > _max_heat_vel * scale)
                        comp_vel = _max_heat_vel * scale;
                }
                else
                {
                    if (fridge_vel > _max_warm_vel * scale)
                        fridge_vel = _max_warm_vel * scale;
                    if (comp_vel < -_max_down_vel * scale)
                        comp_vel = -_max_down_vel * scale;
                }
            #endif

            // BASIC NEWTONIAN - add the velocity to the temperature

            g_fridge_temp += fridge_vel;
            g_comp_temp += comp_vel;

            // debugging

            #if DEBUG_FAKE
                LOGD("--> fridge rpm(%d) fridge(%0.2f) comp(%0.2f) elapsed(%d) period(%d) scale(%0.3f)",
                     actual_rpm,
                     g_fridge_temp,
                     g_comp_temp,
                     elapsed,
                     _fake_period,
                     scale);
                #if DEBUG_FAKE > 1
                    LOGD("        fridge vel(%0.3f) rand(%0.2f) accel(%0.3f) rpm_factor(%0.3d)",
                         fridge_vel,
                         rand_fridge_factor,
                         fridge_accel,
                         cool_rpm_factor);
                    LOGD("        comp   vel(%0.3f) rand(%0.2f) accel(%0.3f) rpm_factor(%0.3d) ambient_factor(%0.3f)",
                         comp_vel,
                         rand_comp_factor,
                         comp_accel,
                         heat_rpm_factor,
                         comp_ambient_factor);
                #endif
            #endif


            // constrain temps to model

            if (g_fridge_temp > _ambient)
                g_fridge_temp = _ambient;
            if (g_comp_temp < _ambient)
                g_comp_temp = _ambient;

            #if 1   // OPTION constrain temps to reasonable values
                if (g_fridge_temp < -100)
                    g_fridge_temp = -100;
                if (g_comp_temp > 100)  // boiling
                    g_comp_temp = 100;
            #endif

        }   // ! first time through th eloop

        // first time (and thereafter) through the loop sets prev_time
        // so we can get elapsed correctly

        prev_time = current_time;

    }   // 1 second temperature modelling

}   // fakeCompressor::run()


// end of fakeCompressor.cpp

