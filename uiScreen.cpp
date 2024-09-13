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
    #include <rom/rtc.h>

    static const char *bootReason()
    {
        switch (rtc_get_reset_reason(0))
        {
            case NO_MEAN                : return "NO_MEAN        ";
            case POWERON_RESET          : return "POWERON_RESET  ";    /**<1, Vbat power on reset*/
            case SW_RESET               : return "SW_RESET       ";    /**<3, Software reset digital core*/
            case OWDT_RESET             : return "OWDT_RESET     ";    /**<4, Legacy watch dog reset digital core*/
            case DEEPSLEEP_RESET        : return "DEEPSLEEP_RESET";    /**<3, Deep Sleep reset digital core*/
            case SDIO_RESET             : return "SDIO_RESET     ";    /**<6, Reset by SLC module, reset digital core*/
            case TG0WDT_SYS_RESET       : return "TG0WDT_SYS_RESE";    /**<7, Timer Group0 Watch dog reset digital core*/
            case TG1WDT_SYS_RESET       : return "TG1WDT_SYS_RESE";    /**<8, Timer Group1 Watch dog reset digital core*/
            case RTCWDT_SYS_RESET       : return "RTCWDT_SYS_RESE";    /**<9, RTC Watch dog Reset digital core*/
            case INTRUSION_RESET        : return "INTRUSION_RESET";    /**<10, Instrusion tested to reset CPU*/
            case TGWDT_CPU_RESET        : return "TGWDT_CPU_RESET";    /**<11, Time Group reset CPU*/
            case SW_CPU_RESET           : return "SW_CPU_RESET   ";    /**<12, Software reset CPU*/
            case RTCWDT_CPU_RESET       : return "RTCWDT_CPU_RESE";    /**<13, RTC Watch dog Reset CPU*/
            case EXT_CPU_RESET          : return "EXT_CPU_RESET  ";    /**<14, for APP CPU, reseted by PRO CPU*/
            case RTCWDT_BROWN_OUT_RESET : return "RTCWDT_BROWN_OU";    /**<15, Reset when the vdd voltage is not stable*/
            case RTCWDT_RTC_RESET       : return "RTCWDT_RTC_RESE";    /**<16, RTC Watch dog reset digital core and rtc module*/
            default                     : return "UNKNOWN_REASON ";
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

