# fridgeController Design

**[Home](readme.md)** --
**Design** --
**[Electronics](electronics.md)** --
**[Build](build.md)** --
**[Notes](notes.md)**


This page presents a more detailed look at the design of the
fridgeController application, and the paramaters that control
the compressor.


## Requirements

The main requirements for the **fridgeController** are as follows

- allows use of **DS18B20 temperature sensors** with software programmed
  temperature setpoints
- allows use of a **mechanical thermostat** that has its own built-in setpoints
- allows for direct **manual control** of the compressor
- allows for setting the **compressor RPMs** either explicity,
  or, possibly, algorithmically.
- has green, yellow, and red *LEDs* that track those on the **miniBox**
- displays the most important *state* (error codes, current compressor RPMS,
  freezer and/or compressor temperature) on the *LCD* by default.
- can be operated directly from the **fridgeController box**
  via the built in *LCD* and *buttons* to see certain additional
  state information and modify the most important parameters (setpoints,
  mode, etc) of the system.
- has a webUI that allows for more detailed remote control and configuration
  of the system.
- has webUI visualization tools (graphs) that show the state of the system
  over time to help in opimizizing the configuration.

The **fridgeController** has been designed with a fair amount
of flexibility and an initial implementation that works in
a specific way.


## Values  (Configuration Parameters and State Variables)

myIOTDevices use **Values** that are configured for the specific myIOTDevice.

Values can have a **type**, a *storage type**, a **style**, and a number of other
details that are used by the myIOTFramework to present the values on the webUI,
store the to the NVS (non-volatile storage) when they change, and so on.

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

### New base myIOTDevice values and styles

DS18B20 temperature sensors are now somewhat understood by
the base myIOTDevice.  These sensors essentially return a
signed 16 bit integer representing the temperature reading.
These integer values may now be used with myIOTValue and will
automatically convert to Farenheit or Centigrade based on a
new myIOTDevice common value, "DEGREE_TYPE", and a corresponding
value style VALUE_DISPLAY_TYPE_DEGREES.

See fridgeController::tSense.cpp for a notion of how
complicated and programmable these sensors are, and
how I am using the boot-up defaults on the sensors I
have.  That code has the int16_t calculateRaw() method.
Conversion from that raw value to Centigrade is simply
raw_value/128 as the raw value indicates the absolute
temperature in 128's of a degree centigrade.  Conversion
to Farenheit is also relatively simple.


This notion is also extended into the myIOTDataLog object,
which now has an explicit type (internally a uint32_t)
for interpretation by javascript.

- **DEGREE_TYPE** - default(FARENHEIGHT) - allows one to set
  the preferred display type for temperatures to CENTIGRADE
  or FARENHEIGHT.



### Configuration Values

The following values are typically stored in NVS and showed on the
"Config" tab in the webUI, and represent the persistent "configuration"
of the device which does not change between reboots.

A Factory Reset of the device will reset all NVS values to a known
initial state.

For each configuration value, we give it's default value, and
describe what it is or does.

- **FRIDGE_MODE** - enum, default(RUN_MIN) - sets one of several
  alternative modes of operation for the controller.  By
  default, the refridgerator will attempt to start and
  run the compressor continuously at its MIN_RPM value.
  Please see below for more discussion of FRIDGE_MODE.
- **SETPOINT_HIGH** - float, degrees C, TEMP_RANGE, DISPLAY_STYLE_TEMPERATURE,
  a temperature above which the compressor will be started in
  the RUN_TEMP mode. Defaults to -12C (10.4F) well below freezing
- **SETPOINT_LOW** - float, degrees C, TEMP_RANGE, DISPLAY_STYLE_TEMPERATURE,
  a temperature below which the compressor will be turned off in
  the RUN_TEMP mode. Defaults to -20C (-4.0C) a super solid freeze.
  the freezing point of water.
- **USER_RPM** - integer, default(2750) - sets the RPM at
  which the compressor will normally run in the initial
  impelementation.  Danfoss compressors can
  typically run from 2000-3500 rpm, and so this value is
  limited in range from 2000 to 3500, and the default is
  right in the middle.
- **MIN_RPM** - integer, default(2000) - sets the minimum RPM at
  which the compressor may run.  Danfoss compressors can
  typically run from 2000-3500 rpm, and so this value is
  limited in range from 2000 to 2750, or half of the design
  range of the compressor.  This value is typically not
  changed by the User.
- **MAX_RPM** - integer, default(3500) - sets the minimum RPM at
  which the compressor may run.  Danfoss compressors can
  typically run from 2000-3500 rpm, and so this value is
  limited in range from 2750 to 3500, or half of the design
  range of the compressor.  This value is typically not
  changed by the User.
- **FRIDGE_SENSE_ID** - string; DS18B20 temperature sensors as used
  by the Controller each have a unique 64 bit identifier.
  This value is a String of text that will contain a
  series of Hex Digits representing the ID of the DS18B20
  temperature sensor that is considered the **"main"
  sensor** used for controlling the compressor.
  The default value is currently set to the known ID
  of a DS18B20 sensor that I own, and a Factory Reset
  will render other devices unusable.  I don't yet have
  a good solution for this, other than someone who clones
  this repo and builds such a device, changes the ID's
  present in tSense.cpp. Readings from this sensor,
  rounded to one decimal point, are set into the FRIDGE_TEMP
  Value.
- **COMP_SENSE_ID** - string; the ID of a DS18B20 temperature sensor
  that is used to monitor the temperature of the compressor.
  Defaults to a known DS18B20 that I own.
  This sensor is not currently used to control the compressor,
  but the temperature is available in the dataLog and charts.
  Readings from this sensor, rounded to one decimal point, are
  set into the COMP_TEMP Value.
- **EXTRA_SENSE_ID** - string; the ID of an extra DS18B20 that can be
  used however the user chooses, perhaps for the refridgerator
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
- **TEMP_SENSE_SECS** - integer, default(10), minimum(10); How often
  in seconds, to read the temperature sensors, and hence, invoke
  any control algorithmns.
- **INV_SENSE_MILLIS** - integer, default 20, how often to read
  the voltages from the inverter to determine the compressor's
  operational state.  At 50 times per second, it is plenty accurate
  to detect and count the DIAG_DIODE blinking, and the instantaneous
  values can be visualized in the webUI Plot tab.
- **CALIB_VOLTS_INV** - a calibration factor used in reporting the
  VOLT_INV value (FAN_DIODE+ pin as proxy for the inverter input
  voltage), set by measuring with a multi-meter during
  initial testing.
- **CALIB_VOLTS_5V** - a calibration factor used in reporting the
  VOLT_5V value (buck convertor output), set by measuring with a
  multi-meter during initial testing.


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

- **STATUS** - String, a string that is sent to the WebUI to show
  various error states, including TSENSE_ERRORS
- **FRIDGE_TEMP** - float, degrees C, TEMP_RANGE, VALUE_STYLE_TEMPERATURE,
  set every TEMP_SENSE_SECS from the DS18B20 at FRIDGE_SENSE_ID
- **COMP_TEMP** - float, degrees C, TEMP_RANGE, VALUE_STYLE_TEMPERATURE,
  set every TEMP_SENSE_SECS from the DS18B20 at COMP_SENSE_ID
- **EXTRA_TEMP** - float, degrees C, TEMP_RANGE, VALUE_STYLE_TEMPERATURE,
  set every TEMP_SENSE_SECS from the DS18B20 at EXTRA_SENSE_ID
- **MECH_THERM** - bool, 0/1 default(0) - a raw digitalRead() of
  the value of the ESP32 pin assigned to read the mechanical
  thermostat.  This pin is an INPUT_PULLDOWN, with
  3.3V being fed to the output pin.  It can be used in
  RUN_MECH_XXX *FRIDGE_MODES*. See below for more information.
- **COMP_RPM** - integer, 2000-3500, set by the controller whenever
  it turns the compressor on, indicating the rpms the compressor
  is running at, or trying to achieve.
- **INV_ERROR** - intger 0..5; is set to the number of flashes the inverter
  DIAG_DIODE is currently flashing.  TODO: chart of flash
  meanings.
- **INV_PLUS** - bool 0/1; is set when the ground relative voltage of
  the FAN/DIODE+ pin from the invertor, is above a certain threshold.
  displayed as the green diode.
- **INV_FAN** - bool; is set to the inverted INV_PLUS relative
  voltage of the FAN- pin from the invertor, is above a certain threshold.
  displayed as the yellow diode.
- **INV_COMPRESS** - integer 0/1; set to one when the controller
  sets the RPMs of the compressor to a non-zero value.
  Displayed as the blue diode.
  It is up to the user to understand that the blue diode does not
  necessarily mean a running compressor and they must also look
  at the RED DIAG DIODE and see that it is not flashing for
  this determination.
- **VOLTS_INV** float, 0..15, the actual voltage measured for FAN/DIODE+,
  which serves as a proxy for the compressor power supply
- **VOLTS_5V** - float, 0..12, the voltage of the VUSB to the ESP32
  measured from the buck converter


### UI Configuration Variables

- **BACKLIGHT_SECS** - ingeger, default(15) max(120)
  How long the LCD back light will stay on after booting or any button press.
  A value of 120 means "forever", never turn the backlight off.
  
## Initial Implementation Notes

I have to try this to see if and how it works.   At this time I believe
that I will allow the compressor to use its own Automatic restart mode
and will not otherwise have configuration values for things like a
restart timer, as I will be closely monitoring the system in real
time.

Later I may wish to implement some kind of a restart protection,
like "if it doesn't start automatically after 10 compressor retries"
stop trying and add a new diag code or messaging system to display it.


### LCD and buttons

TBD

### WS2812 LED not initially implemented

The WS2812 LED is not initially implemented.  Later I envision
it to carry the basic green, yellow, red, and blue diodes outside
of the box, and/or to indicate the Wifi state or additional
error modes.


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



