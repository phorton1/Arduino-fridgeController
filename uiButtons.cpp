//-----------------------------------------
// uiButtons.cpp
//-----------------------------------------

#include "uiButtons.h"
#include "uiScreen.h"
#include <myIOTLog.h>


// buttons call uiScreen::onButton() when pressed.
// buttons 1 and 2 implement long press auto-increment


#define DEBUG_BUTTONS  1

#if DEBUG_BUTTONS
    #define DBG_BUTTON(...)     LOGD(__VA_ARGS__)
#else
    #define DBG_BUTTON(...)
#endif


#define POLL_INTERVAL       20  // ms
    // handles debouncing too ..


void uiButtons::init_button(int num, int pin)
{


}


uiButtons::uiButtons(int pin1, int pin2, int pin3)
{
    m_state = 0;
    m_poll_time = 0;
    m_time = 0;
    m_repeat_count = 0;
    m_repeat_mask = 0;
    m_handled = 0;

    m_pins[0] = pin1;
    m_pins[1] = pin2;
    m_pins[2] = pin3;
}

void uiButtons::init()
{
    for (int i=0; i<NUM_BUTTONS; i++)
    {
        pinMode(m_pins[i],INPUT_PULLUP);
    }
}



void uiButtons::loop()
{
    uint32_t now = millis();
    if (now > m_poll_time + POLL_INTERVAL)
    {
        m_poll_time = now;
        for (int i=0; i<NUM_BUTTONS; i++)
        {
            uint16_t mask = 1<<i;
            bool was_pressed = m_state & mask ? 1 : 0;
            bool is_pressed = !digitalRead(m_pins[i]);

            if (is_pressed != was_pressed)
            {
                m_time = now;
                m_repeat_count = 0;
                if (is_pressed)
                {
                    DBG_BUTTON("BUTTON_PRESS(%d)",i);
                    m_state |= mask;
                    // m_handled = ui_screen->onButton(i,BUTTON_TYPE_PRESS);
                }
                else
                {
                    DBG_BUTTON("BUTTON_RELEASE(%d)",i);
                    m_state &= ~mask;
                    if (!m_handled)
                    {
                        DBG_BUTTON("BUTTON_CLICK(%d)",i);
                        // ui_screen->onButton(i,BUTTON_TYPE_CLICK);
                    }

                    m_time = 0;
                    m_repeat_count = 0;
                    m_handled = 0;
                }
            }
            else if (is_pressed)
            {
                int dif = now - m_time;
                if (mask & m_repeat_mask)
                {
                    #define START_REPEAT_DELAY   300    // ms
                    #define LONG_PRESS_TIME     1800    // ms

                    if (dif > START_REPEAT_DELAY)
                    {
                        // this will be called every 20 ms
                        // we want to scale the mod for repeat count
                        // from 10 down to 1 over 2 seconds

                        dif -= START_REPEAT_DELAY;      // 0..n
                        dif = 2000 - dif;               // 2000..n
                        if (dif < 0) dif = 0;           // 2000..0
                        dif = dif / 200;                // 10..0
                        if (dif == 0) dif = 1;          // 10..1

                        if (m_repeat_count % dif == 0)
                        {
                            DBG_BUTTON("BUTTON_REPEAT(%d)",i);
                            // ui_screen->onButton(i,BUTTON_TYPE_REPEAT);
                        }

                        m_repeat_count++;
                    }
                }
                else if (!m_handled && dif > LONG_PRESS_TIME)
                {
                    m_time = 0;
                    m_repeat_count = 0;
                    m_handled = true;
                    DBG_BUTTON("BUTTON_LONG_CLICK(%d)",i);
                    // ui_screen->onButton(i,BUTTON_TYPE_LONG_CLICK);
                }
            }
        }
    }
}
