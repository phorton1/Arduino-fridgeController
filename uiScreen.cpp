//-----------------------------------------
// uiScreen.cpp
//-----------------------------------------
// rebooting currently only by turning controller off and then on
//
// BACKLIGHT
//
//      is turned on automatically, and stays on if
//          - no SD (data logging wont work)
//          - bad NTP time (data logging wont work)
//          - inverter error
//          - temperature sens errors

//      will go off if _backlight_secs != BACKLIGHT_ALWAYS_ON
//      and there are no button presses for _backlight secs.
//
//      when the backlight goes off it returns to the MAIN_SCREEN,
//      thus terminating any edit in progress
//
// BUTTONS
//
//    if the backlight is off any press eaten to turn backlight on
//    otherwise:
//
//      left click - next screen
//      middle_click - decrement value if editable
//      right_click - increment value if editable
//      left long click - save value (or execute command) if changed
//
// Notes:
//
//      We don't make any effort to normalize values that are being
//      edited versus changes that could be made via the WebUI,
//      serial or telnet ports, taking the attitude that the
//      last man wins.
//
//      For temperatures we cache the DEGREE_TYPE at the moment
//      we get the float for editing on the off chance someone
//      changes DEGREE_TYPE while a temperature is being edited.

#define WITH_SSD1306    0       // small i2c OLED display
    // otherwise its the big bulky old 1602LCD display


#include "fridge.h"
#include "uiScreen.h"
#include "uiButtons.h"
#include <myIOTLog.h>
#include <myIOTWebServer.h>

#if WITH_SSD1306
    #include <Wire.h>
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>
#else
    #include <LiquidCrystal_I2C.h>
#endif


#define DEBUG_SCREEN  1

#if DEBUG_SCREEN
    #define DBG_SCREEN(...)     LOGD(__VA_ARGS__)
#else
    #define DBG_SCREEN(...)
#endif


#define LCD_ADDR    0x23        // current working fridge 1602A LCD ...
// #define LCD_ADDR    0x27     // bilge_alarm and currently broken two



#define LCD_LINE_LEN              16
#define LCD_BUF_LEN               32    // for safety

#define REFRESH_MS                30
#define ACTIVITY_TIMEOUT          15000
    // if no activity for this long, we return to the main screen

//--------------------------------
// Screen definitions
//--------------------------------
// all iot value screens must follow all static screens

#define SCREEN_MAIN             0
#define SCREEN_IP_ADDRESS       1
#define SCREEN_POWER            2
#define FIRST_IOT_SCREEN        3
#define SCREEN_CLEAR_ERROR      3
#define NUM_IOT_SCREENS         13
#define NUM_SCREENS             (FIRST_IOT_SCREEN + NUM_IOT_SCREENS)

const char *edit_ids[NUM_IOT_SCREENS] = {
    ID_CLEAR_ERROR,
    ID_SETPOINT_HIGH,
    ID_SETPOINT_LOW,
    ID_FRIDGE_MODE,
    ID_USER_RPM,
    ID_MIN_RPM,
    ID_MAX_RPM,
    ID_BACKLIGHT_SECS,
    ID_LED_BRIGHTNESS,
    ID_DEGREE_TYPE,
    ID_WIFI,
    ID_STA_SSID,
    ID_REBOOT,
};
    


//-------------------------------------
// bootReason()
//-------------------------------------

#define SHOW_BOOT_REASON  1

#if SHOW_BOOT_REASON
    // grumble, in ESP Core 3.0.4 there are many versions of rtc.h and
    // they use different identifiers for these constants.  Certain ESP32's
    // return different sets of reset_reasons(), and there is no single
    // verifiable consolidated list.  So, instead of using the identifiers,
    // for this non-critical display-only method, I am going to use numeric
    // constants, and have not taken the effort to build a single consolidated
    // list, as I don't even know if that's possible (ffs, i hope they dont
    // use the same number for different reasons on different chips).

    #include <rom/rtc.h>

    static const char *bootReason()
    {
        int val = rtc_get_reset_reason(0);
        switch (val)
        {
            case  0 : return "NO_MEAN        ";    /* no meaning? */
            case  1 : return "POWERON_RESET  ";    /**<1, Vbat power on reset*/
            case  3 : return "SW_RESET       ";    /**<3, Software reset digital core*/
            case  4 : return "OWDT_RESET     ";    /**<4, Legacy watch dog reset digital core*/
            case  5 : return "DEEPSLEEP_RESET";    /**<5, Deep Sleep reset digital core*/
            case  6 : return "SDIO_RESET     ";    /**<6, Reset by SLC module, reset digital core*/
            case  7 : return "TG0WDT_SYS_RESE";    /**<7, Timer Group0 Watch dog reset digital core*/
            case  8 : return "TG1WDT_SYS_RESE";    /**<8, Timer Group1 Watch dog reset digital core*/
            case  9 : return "RTCWDT_SYS_RESE";    /**<9, RTC Watch dog Reset digital core*/
            case 10 : return "INTRUSION_RESET";    /**<10, Instrusion tested to reset CPU*/
            case 11 : return "TGWDT_CPU_RESET";    /**<11, Time Group reset CPU*/
            case 12 : return "SW_CPU_RESET   ";    /**<12, Software reset CPU*/
            case 13 : return "RTCWDT_CPU_RESE";    /**<13, RTC Watch dog Reset CPU*/
            case 14 : return "EXT_CPU_RESET  ";    /**<14, for APP CPU, reseted by PRO CPU*/
            case 15 : return "RTCWDT_BROWN_OU";    /**<15, Reset when the vdd voltage is not stable*/
            case 16 : return "RTCWDT_RTC_RESE";    /**<16, RTC Watch dog reset digital core and rtc module*/
            default :
            {
                static char reason_buf[32];
                sprintf(reason_buf,"UNKNOWN_REASON(%d)",val);
                return reason_buf;
            }
        }
    }
#endif


//------------------------------------------------
// implementation
//------------------------------------------------


uiScreen  ui_screen;

#if WITH_SSD1306

    #define SCREEN_WIDTH    128     // OLED display width, in pixels
    #define SCREEN_HEIGHT   64      // OLED display height, in pixels (little ones are 32, square ones 64)
    #define OLED_RESET      -1      // Reset pin # (or -1 if sharing Arduino reset pin)

    #define OLED_I2C_ADDR   0x3C    // works on all displays; sample prog has "0x3D for 128x64"

    Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#else
    LiquidCrystal_I2C lcd(LCD_ADDR,LCD_LINE_LEN,2);   // 20,4);
        // set the LCD address to 0x27 for a 16 chars and 2 line display
#endif


void uiScreen::init()
{
    ui_buttons.init();
#if WITH_SSD1306
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    bool ok = oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR);
    // No error handling for time being
    if (ok)
    {
        oled.display();  // show built in Adafruit splash screen
        delay(1000);
        clear();
    }

#else
    lcd.init();                      // initialize the lcd
#endif
    backlight(1);

    #if SHOW_BOOT_REASON
        displayLine(0,"%s",bootReason());
        delay(2000);
    #endif

    displayLine(0,"%s",Fridge::getDeviceType());
    displayLine(1,"%s",Fridge::getVersion());
    delay(2000);
}



//------------------------------
// utilities
//------------------------------

void uiScreen::clear()
{
    #if WITH_SSD1306
        oled.clearDisplay();
        oled.display();
    #else
        lcd.clear();
    #endif
}


void uiScreen::displayLine(int line_num, const char *format, ...)
{
    va_list var;
    va_start(var, format);
    char buffer[LCD_BUF_LEN * 2];
        // allow for strings upto 63 characters

    vsnprintf(buffer,LCD_BUF_LEN,format,var);
    int len = strlen(buffer);
    if (len > LCD_LINE_LEN)
        len = LCD_LINE_LEN;
    while (len < LCD_LINE_LEN)
    {
        buffer[len++] = ' ';
    }
    buffer[len] = 0;

#if WITH_SSD1306
    #define LINE_HEIGHT         16      // 2 or 4 lines depending on LCD
    oled.setTextSize(1);                // Font size
    oled.setTextColor(WHITE,BLACK);     // White on Black text
    oled.setCursor(0,line_num*16);      // Start at top-left corner
    oled.print(buffer);
    oled.display();
#else
    lcd.setCursor(0,line_num);
    lcd.print(buffer);
#endif
}


void printBufStr(char *buf, int width, const char *val, bool right=false)
{
    int len = strlen(val);
    char *ptr = &buf[strlen(buf)];
    while (right && len + strlen(ptr)<width)
        strcat(ptr," ");
    sprintf(&ptr[strlen(ptr)],"%s",val);
    while (!right && strlen(ptr)<width)
        strcat(ptr," ");
}


void printBufInt(char *buf, int width, int val, int err=0, bool right=false)
{
    char int_buf[12];
    if (err)
        sprintf(int_buf,"ERR%d ",err);
    else
        sprintf(int_buf,"%4d ",val);
    printBufStr(buf, width, int_buf, right);
}


void printBufFloat(char *buf, int width, float val, int err=0, bool right=false)
{
    char temp_buf[12];
    if (err)
        sprintf(temp_buf,"ERR%d",err);
    else
    {
        // static screen uses current fridge DEGREE_TYPE
        int faren = fridge->getEnum(ID_DEGREE_TYPE);
        sprintf(temp_buf,"%0.0f",faren ? centigradeToFarenheit(val) : val);
        strcat(temp_buf,faren?"F":"C");
    }
    printBufStr(buf, width, temp_buf, right);
}


void uiScreen::backlight(int val)
{
    m_backlight = val;
    m_activity_time = millis();
#if WITH_SSD1306
#else
        if (val)
            lcd.backlight();
        else
            lcd.noBacklight();
#endif
}



//---------------------------------------
// loop()
//---------------------------------------

void uiScreen::loop()
{
    uint32_t now = millis();

    if (now - m_last_refresh > REFRESH_MS)
    {
        m_last_refresh = now;

        // if any error states, turn the backlight on
        // and leave it on after error states turn off
        // by setting m_activity_time on any changes
        
        static bool last_force;
        bool force = (fridge->getBool(ID_WIFI) &&
             fridge->getConnectStatus() != IOT_CONNECT_STA) ||
            fridge->_inv_error ||
            fridge->m_log_error ||
            fridge->m_fridge_temp_error ||
            fridge->m_comp_temp_error;
        if (force && !m_backlight)
        {
            backlight(1);
        }
        if (last_force != force)
        {
            last_force = force;
            m_activity_time = now;
        }

       // if !force && backlight timeout, turn it off

        if (!force && m_backlight)
        {
            int secs = fridge->_backlight_secs;
            if (secs != BACKLIGHT_ALWAYS_ON)
            {
                if (now - m_activity_time > (secs * 1000))
                {
                    setScreen(SCREEN_MAIN);
                    backlight(0);
                }
            }
        }

        // in all cases goto SCREEN_MAIN on activity timeout

        if (m_screen_num && now - m_activity_time > ACTIVITY_TIMEOUT)
            setScreen(SCREEN_MAIN);

        // and finally, if the backlight is on, draw the screen
        
        if (m_backlight)
            showScreen();
    }

    ui_buttons.loop();
}



//-----------------------------------
// setScreen() && init_edit_value()
//-----------------------------------

void uiScreen::setScreen(int screen_num)
{
    DBG_SCREEN("uiScreen::setScreen(%d)",screen_num);
    m_screen_num = screen_num;
    init_edit_value();
}


void uiScreen::init_edit_value()
{
    m_iot_value = 0;
    m_iot_style = 0;
    m_value_min = 0;
    m_value_max = 0;
    m_edit_value = 0;
    m_initial_value = 0;
    m_use_repeat = 0;

    m_value_id = m_screen_num >= FIRST_IOT_SCREEN ?
        edit_ids[m_screen_num - FIRST_IOT_SCREEN] : 0;
    if (m_value_id)
    {
        m_iot_value = fridge->findValueById(m_value_id);
        if (!m_iot_value)
        {
            LOGE("setScreen(%d) could not findValue(%s)",m_screen_num,m_value_id);
            m_value_id = 0;
            return;
        }

        m_iot_type = m_iot_value->getType();
        m_iot_style = m_iot_value->getStyle();

        // get the (integer) value
        // IOT_TYPE_STRING can be viewed, but not edited

        if (!(m_iot_style & VALUE_STYLE_READONLY))
        {
            if (m_iot_type == VALUE_TYPE_BOOL)
                m_edit_value = m_iot_value->getBool();
            else if (m_iot_type == VALUE_TYPE_INT)
                m_edit_value = m_iot_value->getInt();
            else if (m_iot_type == VALUE_TYPE_ENUM)
                m_edit_value = m_iot_value->getEnum();
            else if (m_iot_type == VALUE_TYPE_FLOAT)
            {
                float float_value = m_iot_value->getFloat();
                if (m_iot_style & VALUE_STYLE_TEMPERATURE)
                {
                    m_degree_type = fridge->getEnum(ID_DEGREE_TYPE);
                    if (m_degree_type)
                        float_value = centigradeToFarenheit(float_value);
                }

                // round to integer
                float_value += float_value < 0 ? -0.5 : 0.5;
                m_edit_value = float_value;
                
                if (DEBUG_SCREEN > 1)
                    DBG_SCREEN("init_edit_value(%d) id(%s) set int(%d) from float(%0.3f) DEGREE_TYPE(%d)",
                        m_screen_num, m_value_id, m_edit_value, float_value,m_degree_type);
            }
            else if (m_iot_type == VALUE_TYPE_COMMAND)
            {
                if (DEBUG_SCREEN > 1)
                    DBG_SCREEN("init_edit_value(%d) COMMAND id(%s)",m_screen_num,m_value_id);
                m_edit_value = 1;
            }

            // Strings don't "do" anything by implementation logic
            // because there is no inc/dec case for their "value", so
            // m_edit_value never changs, and they can't be "saved"

            else if (m_iot_type != VALUE_TYPE_STRING)
            {
                LOGE("setScreen(%d) illegal value_type %s(%c)",m_screen_num,m_value_id,m_iot_type);
                m_value_id = 0;
                m_iot_value = 0;
                m_iot_type = 0;
                m_iot_style = 0;
                return;
            }
        }
        else
        {
            LOGE("setScreen(%d) attempt to edit readonly value %s(%c)",m_screen_num,m_value_id,m_iot_type);
            m_value_id = 0;
            m_iot_value = 0;
            m_iot_type = 0;
            m_iot_style = 0;
            return;
        }

        if (m_iot_type != VALUE_TYPE_COMMAND)
        {
            m_last_value = m_edit_value;
            m_initial_value = m_edit_value;
            m_iot_value->getIntRange(&m_value_min,&m_value_max);
        }

        // set repeat mode for BACKLIGHT_SECS, and USER/MIN,/MAX _RPM

        m_use_repeat =
            !strcmp(m_value_id,ID_BACKLIGHT_SECS) ||
            !strcmp(m_value_id,ID_USER_RPM) ||
            !strcmp(m_value_id,ID_MIN_RPM) ||
            !strcmp(m_value_id,ID_MAX_RPM);
        ui_buttons.setRepeatMask(m_use_repeat ? (0x2 | 0x4) : 0);
    }
}




//-----------------------------------
// onButton
//-----------------------------------

bool uiScreen::onButton(int button_num, int event_type)
    // called from uiButtons::loop()
{
    if (DEBUG_SCREEN > 1)
        DBG_SCREEN("uiScreen::onButton(%d,%d)",button_num,event_type);

    // eat the keystroke to turn on backlight

    if (!m_backlight)
    {
        backlight(1);
        return true;
    }
    m_activity_time = millis();

    if (button_num == 0)
    {
        if (event_type == BUTTON_TYPE_CLICK)
        {
            int new_screen = (m_screen_num + 1) % NUM_SCREENS;
            if (new_screen == SCREEN_CLEAR_ERROR && !fridge->_inv_error)
                new_screen++;
            setScreen(new_screen);
        }
        else if (event_type == BUTTON_TYPE_LONG_CLICK)
        {
            if (m_iot_value &&
                m_edit_value != m_initial_value)
            {
                DBG_SCREEN("onButton accepting change id(%s) from(%d) to(%d)",
                    m_value_id,m_initial_value, m_edit_value);

                if (m_iot_type == VALUE_TYPE_BOOL)
                    m_iot_value->setBool(m_edit_value);
                else if (m_iot_type == VALUE_TYPE_INT)
                    m_iot_value->setInt(m_edit_value);
                else if (m_iot_type == VALUE_TYPE_ENUM)
                    m_iot_value->setEnum(m_edit_value);
                else if (m_iot_type == VALUE_TYPE_FLOAT)
                {
                    float float_value = m_edit_value;
                    if (m_iot_style & VALUE_STYLE_TEMPERATURE &&
                        m_degree_type)
                        float_value = farenheitToCentigrade(float_value);
                    DBG_SCREEN("    set float(%0.3f) from int(%d) DEGREE_TYPE(%d)",
                        float_value, m_edit_value, m_degree_type);
                    m_iot_value->setFloat(float_value);
                }
                else if (m_iot_type == VALUE_TYPE_COMMAND)
                {
                    m_iot_value->invoke();
                    setScreen(SCREEN_MAIN);
                }
                init_edit_value();
            }
        }
    }
    else if (
        m_iot_value &&
        m_iot_type != VALUE_TYPE_COMMAND &&
        (event_type == BUTTON_TYPE_PRESS ||
         event_type == BUTTON_TYPE_REPEAT))
    {
        if (button_num == 1)
        {
            m_edit_value--;
            if (m_edit_value < m_value_min)
                if (m_use_repeat)
                    m_edit_value = m_value_min;
                else
                    m_edit_value = m_value_max;
            return true;
        }
        else if (button_num == 2)
        {
            m_edit_value++;
            if (m_edit_value > m_value_max)
                if (m_use_repeat)
                    m_edit_value = m_value_max;
                else
                    m_edit_value = m_value_min;
            return true;
        }
    }

    // otherwise,

    return false;
}




//-----------------------------------
// showScreen
//-----------------------------------

void uiScreen::showScreen()
{
    bool screen_changed = false;
    static int last_screen_num = -1;
    if (last_screen_num != m_screen_num)
    {
        last_screen_num = m_screen_num;
        screen_changed = true;
        clear();
    }

    if (m_screen_num == SCREEN_MAIN)
    {
        // Fridge Temp        ###F/C  ERR1..ERR7                width 5 incl/1 following spaces
        // Compressor Temp    ###F/C  ERR1..ERR7                width 7 incl/3 following spaces
        // Compressor RPM     ####    ERR1..ERR7                width 4 right justified
        //
        // Fridge Mode        Off_,MIN_,MAX_,USER,MECH,TEMP     width 5 incl folowing space
        // WIFI State         W_OFF,STA,AP,AP_STA,W_ERR         width 7 incl following space
        // SD State           __SD/NOSD/ELOG                    width 4

        static String last_main1;
        static String last_main2;
        char buf1[LCD_BUF_LEN] = {0};
        char buf2[LCD_BUF_LEN] = {0};
        const char *wifi = "W_OFF";

        if (fridge->getBool(ID_WIFI))
        {
            iotConnectStatus_t mode = fridge->getConnectStatus();
            if (mode == IOT_CONNECT_ALL)
                wifi = "AP_STA";
            else if (mode == IOT_CONNECT_AP)
                wifi = "AP";
            else if (mode == IOT_CONNECT_STA)
                wifi = "STA";
            else
                wifi = "W_ERR";
        }

        printBufFloat(buf1,5,fridge->_fridge_temp,fridge->m_fridge_temp_error);
        printBufFloat(buf1,7,fridge->_comp_temp,fridge->m_comp_temp_error);
        printBufInt(buf1,4,fridge->_comp_rpm,fridge->_inv_error,true);

        printBufStr(buf2,5,fridgeModes[fridge->_fridge_mode]);
        printBufStr(buf2,7,wifi);
        printBufStr(buf2,4,
            fridge->hasSD() ?
            fridge->m_log_error ?
                "ELOG":"SD":"NOSD",
            true);

        if (screen_changed || last_main1 != buf1)
        {
            last_main1 = buf1;
            displayLine(0,"%s",buf1);
        }
        if (screen_changed || last_main2 != buf2)
        {
            last_main2 = buf2;
            displayLine(1,"%s",buf2);
        }
    }
    else if (m_screen_num == SCREEN_IP_ADDRESS)
    {
        if (screen_changed)
        {
            String sta_ssid = fridge->getString(ID_STA_SSID);
            iotConnectStatus_t mode = fridge->getConnectStatus();
            const char *mode_str =
                !fridge->getBool(ID_WIFI) ? "WIFI_OFF" :
                mode == WIFI_MODE_AP ? "WIFI_AP" :
                mode == WIFI_MODE_STA ? sta_ssid.c_str() : // "WIFI_STA" :
                mode == WIFI_MODE_APSTA ? "WIFI_AP_STA" :
                "WIFI_ERROR";
            displayLine(0,"%s",mode_str);
            displayLine(1,"%16s",mode?fridge->getString(ID_DEVICE_IP).c_str():"");
        }
    }
    else if (m_screen_num == SCREEN_POWER)
    {
        char v_buf[48];
        static float volts_inv;
        static float volts_5v;

        if (screen_changed ||
            volts_inv != fridge->_volts_inv ||
            volts_5v != fridge->_volts_5v)
        {
            volts_inv = fridge->_volts_inv;
            volts_5v = fridge->_volts_5v;
            displayLine(0,"Inverter   %4.1fV",volts_inv);
            displayLine(1,"Controller %4.1fV",volts_5v);
        }
    }

    // all other screens are iot values

    else if (m_iot_value)
    {
        if (screen_changed || m_last_value != m_edit_value)
        {
            m_last_value = m_edit_value;
            displayLine(0,m_value_id);
            if (m_iot_type == VALUE_TYPE_COMMAND)
                displayLine(1,"%16s","confirm?");
            else if (m_iot_type == VALUE_TYPE_ENUM)
                displayLine(1,"%16s",m_iot_value->getDesc()->enum_range.allowed[m_edit_value]);
            else if (m_iot_style & VALUE_STYLE_TEMPERATURE)
                displayLine(1,"%15d%s",m_edit_value,m_degree_type?"F":"C");
            else if (m_iot_type == VALUE_TYPE_BOOL)
                displayLine(1,"%16s",m_edit_value?"On":"Off");
            else if (m_iot_type == VALUE_TYPE_STRING)
                displayLine(1,"%16s",m_iot_value->getString().c_str());
            else
                displayLine(1,"%16d",m_edit_value);
            screen_changed = true;
        }
    }


    // set the editing/dirty indicator

    if (m_iot_value)
    {
        bool dirty = m_edit_value != m_initial_value;
        static bool last_dirty = 0;
        if (screen_changed ||
            last_dirty != dirty)
        {
            last_dirty = dirty;
            #if WITH_SSD1306
                oled.setCursor(127-8,0);
                oled.print(dirty ? "*" : " ");
                oled.display();
            #else
                lcd.setCursor(15,0);
                lcd.print(dirty ? "*" : " ");
            #endif
        }
    }
}







