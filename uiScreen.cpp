//-----------------------------------------
// uiScreen.cpp
//-----------------------------------------

#include "uiScreen.h"
#include "fridge.h"
#include <myIOTLog.h>
#include <LiquidCrystal_I2C.h>


#define DEBUG_SCREEN  1
#define SHOW_BOOT_REASON  1


#if DEBUG_SCREEN
    #define DBG_SCREEN(...)     LOGV(__VA_ARGS__)
#else
    #define DBG_SCREEN(...)
#endif


#define LCD_LINE_LEN              16

#define LCD_ADDR    0x23    // current working one ...
// #define LCD_ADDR    0x27    // bilge_alarm, currently broken two


LiquidCrystal_I2C lcd(LCD_ADDR,LCD_LINE_LEN,2);   // 20,4);
    // set the LCD address to 0x27 for a 16 chars and 2 line display


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



void uiScreen::init()
{
    lcd.init();                      // initialize the lcd
    lcd.backlight();

    #if SHOW_BOOT_REASON
        lcd.setCursor(0,0);
        lcd.print(bootReason());
        delay(2000);
    #endif

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(0,Fridge::getDeviceType());
    lcd.setCursor(0,1);
    lcd.print(Fridge::getVersion());
}

