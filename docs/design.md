# fridgeController Design

**[Home](readme.md)** --
**Design** --
**[Electronics](electronics.md)** --
**[Build](build.md)** --
**[Notes](notes.md)** --
**[Analysis](analysis.md)**



This page presents a more detailed look at the design of the
fridgeController application, the paramaters that control
the compressor, and the LCD UI on the device.


## A. Requirements

The main requirements for the **fridgeController** are as follows

- allows use of **DS18B20 temperature sensors** with software programmed
  temperature setpoints
- allows use of a **mechanical thermostat** that has its own built-in setpoints
- allows for direct **manual control** of the compressor
- allows for setting the **compressor RPMs** either explicity,
  or, algorithmically based on the temperature sensors and/or mechanical thermostate.
- has *Green, Yellow, and Red* **LEDs** that track those on the *miniBox*
- has a *Blue* **LED** that indicates if the Controller has turned the Compressor on
  by setting a non-zero value for the *RPMs*
- displays the most important **state** (error codes, current compressor RPMS,
  freezer and/or compressor temperature) on the *LCD* by default.
- can be operated directly from the **fridgeController box**
  via the built in *LCD* and *buttons* to see certain additional
  state information and modify the most important parameters (Setpoints,
  Operating Mode, etc) of the system.
- has a webUI that allows for more detailed remote control and configuration
  of the system.
- has webUI visualization tools (plots and graphs) that show the state of the
  system over time to help in optimizizing the configuration.

The **fridgeController** has been designed with a fair amount
of flexibility and an initial implementation that works in
a specific way.


## B. Values  (Configuration Parameters and State Variables)

myIOTDevices use **Values** that are configured for the specific myIOTDevice.

Values can have a **type**, a **storage type**, a **style**, and a number of other
details that are used by the myIOTFramework to present the values on the webUI,
store them to the NVS (non-volatile storage) when they change, and so on.

Device specific values can be presented in the webUI on the "Dashboard" or
the "Config" tabs.  All myIOTDevices share a number of common Values which
are presented on the "Device" tab in the webUI.  In addition, the webUI lets
one look at the files on the SPIFFS (embedded ESP32) file system, and the SD
Card, if any.  The fridgeController specifically has an SD Card where the
history of various variables, and a logfile, are kept.

Thus Values can be used to **configure** the device, and also can
be used to report and keep track of the **state** of the device.
Values can also be **commands** which present as "buttons" in the
webUI, and can translate directly into calls to device specific
methods to do certain things.

Please see the
[**MyIOT**](https://github.com/phorton1/Arduino-libraries-myIOT)
repository for more details about the general idea of myIOT Values.



### Configuration Values

The following values are typically stored in NVS and showed on the
"Config" tab in the webUI, and represent the persistent "configuration"
of the device which does not change between reboots.

A Factory Reset of the device will reset all NVS values to a known
initial state.

For each configuration value, we give it's default value, and
describe what it is or does.

- **FRIDGE_MODE** - *enum, default(RUN_MIN)* - sets one of several
  alternative modes of operation for the controller.  By
  default, the refridgerator will attempt to start and
  run the compressor continuously at its MIN_RPM value.
  Please see below for more discussion of FRIDGE_MODE.
- **SETPOINT_HIGH** - *float, temperature, default(-12C)* -
  a temperature above which the compressor will be started in
  the RUN_TEMP mode. The defaults of -12C (10.4F) is well below
  freezing
- **SETPOINT_LOW** - *float, temperature, default(-20C)* -
  a temperature below which the compressor will be turned off in
  the RUN_TEMP mode. Defaults to -20C (-4.0F) a super solid freeze.
- **USER_RPM** - *integer, default(2600)* - sets the RPM at
  which the compressor will normally run in the initial
  impelementation.  Danfoss compressors can
  typically run from 2000-3500 rpm, and so this value is
  limited in range from 2000 to 3500, and the default is
  somewhere in the middle.
- **MIN_RPM** - *integer, default(2000)* - sets the minimum RPM at
  which the compressor may run.  Danfoss compressors can
  typically run from 2000-3500 rpm. This value is typically not
  changed by the User.
- **MAX_RPM** - *integer, default(3200)* - sets the maximum RPM at
  which the compressor may run.  Danfoss compressors can
  typically run from 2000-3500 rpm. This value is
  intitially set to 3200 for safety and may be changed
  up to 3500 by the User as desired.
- **FRIDGE_SENSE_ID** - *string* -  DS18B20 temperature sensors
  as used by the Controller each have a unique 64 bit identifier.
  This value is a String of Hex Digits representing the ID of
  the DS18B20 temperature sensor that is considered the **"main"
  sensor** used for controlling the compressor.
  Readings from this sensor, rounded to one decimal point, are
  set into the FRIDGE_TEMP Value.
  *The default value is currently set to the known ID
  of a DS18B20 sensor that I own, and a Factory Reset
  will wipe out any user entered FRIDGE_SENSE_ID.
- **COMP_SENSE_ID** - *string* - the ID of a DS18B20 temperature sensor
  that is used to monitor the temperature of the compressor.
  Readings from this sensor, rounded to one decimal point, are
  set into the COMP_TEMP Value.
  This sensor is not currently used to control the compressor,
  but the temperature is available in the dataLog and charts.
  *Defaults to a known DS18B20 that I own.
  See tSense.cpp for default value.*
- **EXTRA_SENSE_ID** - *string* - the ID of an extra DS18B20 that
  can be used however the user chooses, perhaps for the refridgerator
  portion of a combined Freezer/Refridgerator, or perhaps as
  a second (low in the) freezer temperature sensor.
  Defaults to ''.  Note that although the controller board
  only has two connectors for DS18B20 temperature sensors,
  upto 64 can be connected in parallel, overall, to either
  or both of the connectors (which are wired in parallel).
  This sensor is not currently used to control the compressor,
  but the temperature is available in the dataLog and charts.
  Readings from this sensor, rounded to one decimal point, are
  set into the EXTRA_TEMP Value.
- **TEMP_SENSE_SECS** - *integer, default(30), minimum(10)* - How often
  in seconds, to read the temperature sensors, and hence, invoke
  any control algorithmns.
- **INV_SENSE_MILLIS** - *integer, default 20* - how often to read
  the voltages from the inverter to determine the compressor's
  operational state.  At 50 times per second, it is accurate enough
  to detect and count the DIAG_DIODE blinking. The instantaneous
  values can be visualized in the webUI Plot tab.
- **CALIB_VOLTS_INV** - *float, default(1.0)* - a calibration factor
  used in reporting the VOLT_INV value (FAN_DIODE+ pin as proxy for
  the inverter input voltage). Can be set by measuring the actual
  temperatures during installation with a multi-meter.
- **CALIB_VOLTS_5V** - - *float, default(1.0)* - a calibration factor
  used in reporting the VOLT_5V value (buck convertor output). Set
  during [Electronics](electronics.md) Integration Testing by measuring
  with the output of the Buck Converter.


Note that SETPOINT_HIGH must be above SETPOINT_LOW by
at least 2C (3.6F). If, when setting either of these values,
if that condition is not met, the other value will automatically
be changed accordingly.


### FRIDGE_MODE

This section describes the possible values for **FRIDGE_MODE**.
Initially, to get this project out the door and onto the boat, I
will implement the following simple FRIDGE_MODES:

- **OFF** - the controller will (or has) turned the compressor off.
- **RUN_MIN** - the controller will start and continously run the
  refridgerator at MIN_RPM.
- **RUN_MAX** - the controller will start and continously run the
  refridgerator at MAX_RPM.
- **RUN_USER** - the controller will start and continously run the
  refridgerator at USER_RPM.
- **RUN_MECH** - the controller will start the compressor when the
  MECH_THERM value changes from 0 to 1 and will stop it when the
  MECH_THERM value changes from 1 to 0
- **RUN_TEMP** - the controller will start the compressor when the
  temperature rises above SETPOINT_HIGH, and will turn it off
  when the temperature falls below SETPOINT_LOW


### Dashboard (State) Values

State values are typically NOT stored in NVS, exist only in
the ESP32 memory, and are reset to zero on a reboot.

- **STATUS** - *string* - a string that is sent to the WebUI to show
  various error states, including TSENSE_ERRORS
- **FRIDGE_TEMP** - *float, temperature* -
  set every TEMP_SENSE_SECS from the DS18B20 at FRIDGE_SENSE_ID. Used with the
  **RUN_TEMP** *FRIDGE_MODE*.
- **COMP_TEMP** - *float, temperature* -
  set every TEMP_SENSE_SECS from the DS18B20 at COMP_SENSE_ID
- **EXTRA_TEMP** - - *float, temperature* -
  set every TEMP_SENSE_SECS from the DS18B20 at EXTRA_SENSE_ID
- **MECH_THERM** - *bool, 0/1 default(0)* - a raw digitalRead() of
  the value of the ESP32 pin assigned to read the mechanical
  thermostat.  This pin is an INPUT_PULLDOWN, with
  3.3V being fed to the output pin.  Used with the
  **RUN_MECH** *FRIDGE_MODE*.
- **COMP_RPM** - *integer, 2000-3500* - set by the Controller whenever
  it turns the compressor on, indicating the rpms the compressor
  is running at, or trying to achieve.  A non-zero value lights
  up the *Blue LED* on the Controller board.
- **INV_ERROR** - *integer 0-7* - is set to the number of flashes the
  inverter DIAG_DIODE (the Red LED on both PCBs) is currently flashing.
  TODO: Tooltip and Document chart of flash meanings.
- **INV_PLUS** - *bool 0/1* - is set when the ground relative voltage of
  the FAN/DIODE+ pin from the invertor, is above a certain threshold.
  Displayed as the the Green LED on both PCBs.
- **INV_FAN** - *bool 0/1* - is set when the relative
  voltage of the FAN- pin from the invertor drops below a certain threshold.
  Displayed as the the Yellow LED on both PCBs.
- **INV_COMPRESS** - *bool 0/1* -  set to one when the controller
  sets the RPMs of the compressor to a non-zero value.
  Displayed as the Blue LED on the Controller board..
  **NOTE** - *It is up to the user to understand that the blue diode does not
  necessarily mean a running compressor and they must also look
  at the RED DIAG DIODE and see that it is not flashing for
  this determination.*
- **VOLTS_INV** - *float, 0..18* - the actual voltage measured for FAN/DIODE+,
  which serves as a proxy for the compressor power supply
- **VOLTS_5V** - *float, 0..12*, the voltage of the 5V pin on the ESP32.


### Commands

- **CLEAR_ERROR** - this IOT command takes advantage of the note that
  it is possible to clear the INV_ERROR on the inverter by turning the
  PWM Off, then On briefly, then Off again briefly, before possibly turning
  it On again based on the FRIDGE_MODE.

  The inverter resets its state in preperation for a restart attempt, but
  we turn the PWM off before it actually tries a restart.

CLEAR_ERROR can only be used when there is an INV_ERROR,
and WITH_PWM is compiled into the code.


### UI Configuration Variables

- **BACKLIGHT_SECS** - *ingeger, default(15) max(120)* - 
  How long the LCD back light will stay on after booting or any button press.
  A value of 120 means "forever", never turn the backlight off.


## C. LCD and button UI

The device's native UI has an LCD Screen and three buttons.


### Buttons

If the backlight is off any button press is eaten to turn backlight on.
Otherwise:

- **left click** - next screen
- **middle_click** - decrement value if editable
- **right_click** - increment value if editable
- **left long click** - execute command or save value if changed

When a value has been changed an asterisk **(\*)** will show in
the upper right corner of the LCD.


### MAIN SCREEN

The main screen Top Line shows

- **Freezer Temperature** - in C or F according to device's DEGREE_TYPE
- **Compressor Temperature** - in C or F according to device's DEGREE_TYPE
- **Compressor RPM** - 0, or a value between 2000 and 3500, or an ERR#
  where # is the 1-6 Danfoss error code for the number of diode flashes.

The main screen Second Line Shows:

- **Fridge Mode** - Off, MIN, MAX, USER, MECH, or TEMP
- **Wifi Status** - W_OFF, STA, AP, AP_STA, or W_ERR
- **SD Card State** - SD or NO_SD if there is/was a problem initializing the SD

Note that the SD is crucial for logging data, and without it, the Logfile will
contain many errors.  Note also that having the device successfully connect
as a STAtion and getting NTP time from the internet is also crucial to data logging.

### Other Screens

Screens are accessed in this order.
There are a few screens that provide consolidated information about the
state of the system, then the rest of the screens correlate to myIOTValues,
most of which can be edited.


- **IP_ADDRESS** - shows the wifi status and IP Address of the device.
  If connected as a STA, the STA_SSID of the device is shown as the status
- **POWER** - shows the sensed voltage into the Inverter (12V) and the
  sensed voltage from the Buck Converter (5V) powering the ESP32.
- **CLEAR_ERROR** - Screen only shows up if there is an INV_ERROR. Clears the INV_ERROR for an immediate restart attempt.
- **SETPOINT_HIGH** - Allows for editing the SETPOINT_HIGH value.
- **SETPOINT_LOW** - Allows for editing the SETPOINT_LOW value.
- **FRIDGE_MODE** - Allows for editing the FRIDGE_MODE enumerated value.
- **USER_RPM** - Allows for editing the USER_RPM value.
- **MIN_RPM** - Allows for editing the MIN_RPM value.
- **MAX_RPM** - Allows for editing the MAX_RPM value.
- **BACKLIGHT_SECS** - Allows for editing the BACKLIGHT_SECS value.
- **DEGREE_TYPE** - Allows for editing the device's DEGREE_TYPE enumerated value.
- **WIFI** - Allows the unit to turn WIFI off and on; Off is a bad idea (no clock!)
- **STA_SSID** - Shows the STA_SSID for debugging
- **REBOOT** - Allows user to Soft Reboot the ESP32


### Backlight

**BACKLIGHT_ALWAYS_ON** = 120 Seconds

The Backlight will go off if BACKLIGHT_SECS != BACKLIGHT_ALWAYS_ON
and there are no button presses for BACKLIGHT secs.

When the backlight goes off the system returns to the MAIN_SCREEN,
thus terminating any edit in progress


### Activity Timeout

If no buttons are pressed for 15 seconds, the system will return
to the MAIN_SCREEN, terminating any edit in progress.



## D. Initial Implementation Notes

I have to try this to see if and how it works.   At this time I believe
that I will allow the compressor to use its own Automatic restart mode
and will not otherwise have configuration values for things like a
restart timer, as I will be closely monitoring the system in real
time.

- It is likely that a **PID Controller** will be implemented to provide
  more sophisticated control of the Compressor based on the FRIDGE_TEMP
  Value over time to *optimize the energy usage* of the Compressor,
- Another nice thing might to have built in Compressor protection
  which takes the COMP_TEMP into account.
- It may be desireable to *Count Restart Attempts* and/or
  implement some kind of a restart protection, like "if it doesn't start
  automatically after 10 Cmpressor retries" stop trying and set a new
  INV_ERROR Code(8) to display it.


### WS2812 LED not initially implemented

The WS2812 LED is not initially implemented.  Later I envision
it to carry the basic green, yellow, red, and blue diodes outside
of the box, and/or to indicate the Wifi and/or System state and/or
additional error information.

There also may be an WS2812 LED for the Wifi and/or System state,
in addition to the Green, Yellow, Red, and Blue diodes on the
Controller box.  Future Hardware revision may get rid of the
actual LEDs in the current design and just use WS2812's
for simplicity and to save ESP32 pins.


### DS18B20 Calibration

In practice different DS18B20's give different readings
when at the same temperature.  I don't have any good
temperature references or measuring tools that I really
trust, so, at this time the best I could do test all
my DS18B20's at various temperatures and try to develop
a "calibration scheme" that would get them to report
"about" the same temperatures under the same contions.
However, for the initial implementation, I am just going
to use them "AS-IS", as if they were 100% accurate and
precise.



[**Next:**](electronics.md) The fridgeController electronics ...



