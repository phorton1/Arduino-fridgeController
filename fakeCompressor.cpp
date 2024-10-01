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


#define DEBUG_FAKE  2


static int      actual_rpm;
static uint32_t last_start_attempt;
static int      start_error;
static uint32_t last_flash_cycle;
static uint32_t last_flash;

static  float   fridge_vel;
static  float   comp_vel;



extern int cur_rpm;
    // in fridge.cpp


void fakeCompressor::init_for_start(bool all)
    // init_for_start(1) called from init
    // init_for_start(0) called from actual start attempt
{
    LOGD("    init_for_start(%d)",all);

    g_sample_fan = 0;
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
    static bool last_use_fake = 0;
    if (last_use_fake != _use_fake)
    {
        last_use_fake = _use_fake;
        init();
    }
    if (!_use_fake)
        return;

    // first section contains "rapid" behavior when starting/stopping
    // the compressor to emulate the voltages in that process.
    // Once the compressor is running, this section reaches a
    // steady state (with the exception of possible future random faults)

    uint32_t now = millis();

    if (_comp_on)
    {
        float voltage = random(100) - 50;
        voltage /= 10;
        voltage += 12;
        g_sample_plus = (4096.0 * voltage/15.0);

        if (!cur_rpm && (last_start_attempt || actual_rpm))     // stopped by Fridge
        {
            init_for_start(0);
            LOGI("fakeCompressor stopped by Fridge");
        }
        else if (cur_rpm && !actual_rpm && !last_start_attempt)
        {
            last_start_attempt = now;
            g_sample_fan = g_sample_plus;
            LOGI("fakeCompressor initiating start attempt at rpm(%d)",cur_rpm);
        }
        else if (last_start_attempt && now-last_start_attempt > 2000)
        {
            last_start_attempt = 0;
            actual_rpm = cur_rpm ? cur_rpm : 2000;
            LOGI("fakeCompressor completed start attempt rpm(%d)",actual_rpm);
        }
    }
    else
    {
        g_sample_plus = 0;
        if (last_start_attempt)
            init_for_start(0);
    }

    // Do temperature modelling once per second for now.
    // This helps to see the behavior of the model, but
    // perhaps should be broken out into a separate routine
    // that is only called when the values (tSense) would
    // actually be used.
    //
    // The hard part is coming up with a reasonable model
    // of how the refridgerator and compressor warm up
    // or cool down based on their current temperatures
    // and the actual_rpm, and developing nice parameters
    // for controlling that process.

    static uint32_t last_time = 0;
    static uint32_t prev_time = 0;
    uint32_t current_time = time(NULL);

    if (last_time != current_time)
    {
        last_time = current_time;

        if (prev_time)
        {
            // Calculate time elapsed since last iteration

            uint32_t elapsed = current_time - prev_time;

            // The basic idea is that we keep the fridge_delta and
            // comp_delta as the degrees per _fake_period (default 60secs=1 minute)
            // that their temperature will change, and add the appropriate fraction,
            // baed on _elapsed, of those to the current temperatures for each time
            // through this code.
            //
            // We use newtoning concepts of velocity and per second-squared acceleration.
            // The increased benefit of rpms falls off more quickly for cooling than it does for heating.
            // We use the fact that the ambient temperature matters when the fridge is warming,
            // or the compressor is cooling down.

            float fridge_accel = 0;
            float comp_accel = 0;
            float cool_rpm_factor = 0;
            float heat_rpm_factor = 0;
            float fridge_ambient_factor = 0;
            float comp_ambient_factor = 0;

            if (actual_rpm)     // compressor on, decrease fridge temperature and increase comp temperature
            {
                cool_rpm_factor = 1 + ((actual_rpm - 2000) / 1800) * 0.5;
                heat_rpm_factor = 1 + ((actual_rpm - 2000) / 1500) * 0.5;
                    // 1..1.5 for min_rpm ... max_rpm, more for heating than cooling

                // the rpm factor will multiply the acceleration by 1-1.5, so _cooling_accel and
                // _heating_accel are the "minimum" accelerations, a fact user must be aware of.
                // a more complicated log() or exp() function and/or parameter might be needed.

                fridge_accel = -cool_rpm_factor * _cooling_accel;
                comp_accel = heat_rpm_factor * _heating_accel;

                if (fridge_vel > 0)
                    fridge_vel *= 0.6;
                if (comp_vel < 0)
                    fridge_vel *= 0.5;

            }
            else    // compressor off, increase fridge temperature and decreate comptemperature
            {
                // ambient difference affects compressor more because fridge is insulated

                #if 1

                    fridge_ambient_factor = 1;
                    comp_ambient_factor = 1;

                #else

                    float fridge_ambient_diff = _ambient - g_fridge_temp;
                    float comp_ambient_diff = g_comp_temp - _ambient;

                    fridge_ambient_factor = std::exp(-fridge_ambient_diff / 100.0);
                    comp_ambient_factor = std::exp(-comp_ambient_diff / 60.0);

                    // fridge_ambient_factor = fridge_ambient_diff > 0 ? 0.01 + fridge_ambient_diff / 100 : 0;
                    // comp_ambient_factor = comp_ambient_diff > 0 ? 0.01 + comp_ambient_diff / 60 : 0;

                    if (fridge_ambient_factor > 1)
                        fridge_ambient_factor = 1;
                    if (comp_ambient_factor > 1)
                        comp_ambient_factor = 1;

                #endif

                fridge_accel = fridge_ambient_factor * _warming_accel;
                comp_accel = - comp_ambient_factor * _cooldown_accel;

                if (fridge_vel < 0)
                    fridge_vel *= 0.6;
                if (comp_vel > 0)
                    fridge_vel *= 0.5;
            }


            fridge_accel =  elapsed * fridge_accel / _fake_period;
            comp_accel = elapsed * comp_accel / _fake_period;

            // add in a 0.8..1.2 random factor for each

            float rand_fridge_factor = 1 + (((float) random(40)) / 100.0) - 0.2;
            float rand_comp_factor =   1 + (((float) random(40)) / 100.0) - 0.2;

            fridge_vel += fridge_accel * rand_fridge_factor;
            comp_vel += comp_accel * rand_comp_factor;

            if (actual_rpm)
            {
                if (fridge_vel < -_max_cool_vel)
                    fridge_vel = -_max_cool_vel;
                if (comp_vel > _max_heat_vel)
                    comp_vel = _max_heat_vel;
            }
            else
            {
                if (fridge_vel > _max_warm_vel)
                    fridge_vel = _max_warm_vel;
                if (comp_vel < -_max_down_vel)
                    comp_vel = -_max_down_vel;
            }

            g_fridge_temp += fridge_vel;
            g_comp_temp += comp_vel;

            #if DEBUG_FAKE
                LOGD("--> fridge rpm(%d) fridge(%0.2f) comp(%0.2f) elapsed(%d)",
                     actual_rpm,
                     g_fridge_temp,
                     g_comp_temp,
                     elapsed);
                #if DEBUG_FAKE > 1
                    LOGD("        fridge vel(%0.3f) rand(%0.2f) accel(%0.3f) rpm_factor(%0.3d) ambient_factor(%0.3f)",
                         fridge_vel,
                         rand_fridge_factor,
                         fridge_accel,
                         cool_rpm_factor,
                         fridge_ambient_factor);
                    LOGD("        comp   vel(%0.3f) rand(%0.2f) accel(%0.3f) rpm_factor(%0.3d) ambient_factor(%0.3f)",
                         comp_vel,
                         rand_comp_factor,
                         comp_accel,
                         heat_rpm_factor,
                         comp_ambient_factor);
                #endif
            #endif
            

            if (g_fridge_temp > _ambient)
                g_fridge_temp = _ambient;
            if (g_fridge_temp < -100)
                g_fridge_temp = -100;

            if (g_comp_temp < _ambient)
                g_comp_temp = _ambient;
            if (g_comp_temp > 100)  // boiling
                g_comp_temp = 100;


        }
        prev_time = current_time;

    }


}