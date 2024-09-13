//-----------------------------------------
// uiButtons.h
//-----------------------------------------

#pragma once

#include <Arduino.h>

#define NUM_BUTTONS          3

#define BUTTON_TYPE_PRESS           1
#define BUTTON_TYPE_REPEAT          2
#define BUTTON_TYPE_CLICK           3
#define BUTTON_TYPE_LONG_CLICK      4


class uiButtons
{
    public:

        uiButtons(int pin1, int pin2, int pin3);
        void init();

        void loop();

        bool setRepeatMask(uint16_t mask)  { m_repeat_mask = mask; }
            // enables repeats from buttons by mask
            // button0 == 1, button1=2, and button3=4

    private:

        int      m_pins[NUM_BUTTONS];

        int      m_state;
        uint32_t m_poll_time;
        uint32_t m_time;
        int      m_repeat_count;
        uint16_t m_repeat_mask;
        bool     m_handled;

        void init_button(int num, int pin);
};


extern uiButtons ui_buttons;
    // in fridge.cpp
    