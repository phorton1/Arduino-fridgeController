//-------------------------------------------------
// fakeCompressor.cpp
//-------------------------------------------------
#include "fakeCompressor.h"

bool    fakeCompressor::_compressor;      // whether to use this or not
bool    fakeCompressor::_comp_on;         // whether or not it's turned on
int     fakeCompressor::_prob_error;      // probability of a startup error default(3)
float   fakeCompressor::_ambient;         // ambient temperature, default(80F) in Centigrade
float   fakeCompressor::_insulate;        // ???
float   fakeCompressor::_cooling;         // ???
float   fakeCompressor::_heating;         // ???
float   fakeCompressor::_cooldown;        // ???

float   fakeCompressor::g_fridge_temp;
float   fakeCompressor::g_comp_temp;
int     fakeCompressor::g_sample_plus;
int     fakeCompressor::g_sample_fan;
int     fakeCompressor::g_sample_diode;


static  int actual_rpm;
static  uint32_t last_start_attempt;
static  uint32_t last_start_time;
static  uint32_t last_stop_time;

static  int start_error;
static  uint32_t last_flash_cycle;
static  uint32_t last_flash;

extern int cur_rpm;
    // in fridge.cpp


void fakeCompressor::init_for_start()
{
    LOGD("    init_for_start()");

    g_sample_fan = 0;
    g_sample_diode = 0;

    actual_rpm = 0;
    last_start_attempt = 0;
    start_error = 0;
    last_flash_cycle = 0;
    last_flash = 0;
}


void fakeCompressor::run()
    // called every _inv_sense_ms by Fridge
{
    uint32_t now = millis();

    if (_comp_on)
    {
        float voltage = random(100) - 50;
        voltage /= 10;
        voltage += 12;
        g_sample_plus = (4096.0 * voltage/15.0);

        if (!cur_rpm && (last_start_attempt || actual_rpm))     // stopped by Fridge
        {
            init_for_start();
            last_stop_time = now;
            LOGI("fakeCompressor stopped by Fridge");
        }
        else if (cur_rpm && !actual_rpm && !last_start_attempt)
        {
            last_start_attempt = now;
            last_start_time = 0;
            g_sample_fan = g_sample_plus;
            LOGI("fakeCompressor initiating start attempt at rpm(%d)",cur_rpm);
        }
        else if (last_start_attempt && now-last_start_attempt > 2000)
        {
            last_start_attempt = 0;
            last_start_time = now;
            last_stop_time = 0;
            actual_rpm = cur_rpm ? cur_rpm : 2000;
            LOGI("fakeCompressor completed start attempt rpm(%d)",actual_rpm);
        }
    }
    else
    {
        g_sample_plus = 0;
        if (last_start_attempt)
            init_for_start();
    }

    // Do temperature modelling once per second for now
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
            uint32_t start_elapsed = last_start_time ? now - last_start_time : 0;
            uint32_t stop_elapsed = last_stop_time ? now - last_stop_time : 0;
            if (start_elapsed > 100000) start_elapsed = 100000;
            if (stop_elapsed > 100000) stop_elapsed = 100000;

            float rpm_effect = actual_rpm / 10; // 0 or 200 to 350

            float cool_lost =       elapsed * ((_insulate * (_ambient - g_fridge_temp)) / 100);
            float cooling_effect =  (start_elapsed/100000) * elapsed * ((_cooling * rpm_effect) / 100);
            float heating_effect =  (start_elapsed/100000) * elapsed * ((_heating * rpm_effect) / 100);
            float cooldown_effect = (stop_elapsed/100000) * elapsed * ((_cooldown * (g_comp_temp - _ambient)) / 100);

            g_fridge_temp += cool_lost - cooling_effect;
            g_comp_temp += heating_effect - cooldown_effect;

            LOGD("  fridge(%0.3f) comp(%0.3f) elapsed(%d) lost(%0.3f) cooling(%0.3f) heating(%0.3f) cooldown(%0.3f)",
                 g_fridge_temp,
                 g_comp_temp,
                 elapsed,
                 cool_lost,
                 cooling_effect,
                 heating_effect,
                 cooldown_effect);

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