# fridgeController Implementation Notes

**[Home](readme.md)** --
**[Design](design.md)** --
**[Electronics](electronics.md)** --
**[Build](build.md)** --
**Notes** --
**[Analysis](analysis.md)** 


### A. New VALUE_STYLE_TEMPERATURE and DEGREE_TYPE style and value

The base
[myIOTDevice](https://github.com/phorton1/Arduino-libraries-myIOT)
now has a **DEGREES_TYPE** Value that can
be set by the user, and which may be *Centigrade* or *Farenheit*.
Degrees are tagged with the new **VALUE_STYLE_TEMPERATURE** style
and stored as VALUE_TYPE_FLOAT (32 bit floating point number) representing
a temperature in 1/128ths of a degree centigrade.

The Controller, WebUI and myIOTDevice UI will automatically display
and manipulate temperatures in the current DEGREE_TYPE as specified
by the user.

Note that DS18B20 temperature sensors essentially return a
signed 16 bit integer representing the temperature reading
in 1/128th degrees Centigrade, hence this myIOTDevice addition.

See fridgeController::tSense.cpp for a notion of how
complicated and programmable these sensors are, and
how I am using the boot-up defaults on the sensors I
have.

This notion is also extended into the myIOTDataLog object,
which now has an explicit type, **LOG_COL_TYPE_TEMPERATURE**,
for interpretation of logged data values by the webUI
javascript.


## B. Data Logging, Charting, and Plotting

A *bunch* of work was done to the
[myIOT library](https://github.com/phorton1/Arduino-libraries-myIOT)
to implement **Data Logging, Charting,** and realtime **Plotting**
in the WebUI, as well as a nascent notion of device specific **Widgets**.

These new capabilities have *not yet been documented* in the myIOT library,
although this device, and the
[Bilge Alarm](https://github.com/phorton1/Arduino-bilgeAlarm)
have been modified to implement Widgets and Charting, and
this device, and
[theClock3](https://github.com/phorton1/Arduino-theClock3) have
been updated to implement real-time plotting.

At this time, as a result, it is very likely that the
[myIOTServer](https://github.com/phorton1/base-apps-myIOTServer)
has been broken and will not work correctly, which is semi-intentional,
as my current plan is to rework the myIOTServer *away from being
a Websocket emulator* into more of a pure *socket forwarder*, as
I think it was **crashing often** on the rPi I use for it.

In any case, the myIOTServer is probably broken at this time.


## C. the fakeCompressor

In order to test the software, particularly the *Control Algorithms*,
*Data Logging*, and *WebUI **Charting and Plotting***, on a *bare ESP32*
without any PCB's, I spent quite a bit of time implementing the **fakeCompressor**,
which is included (by default) if the compile define *WITH_FAKE_COMPRESSOR=1*
is set when compiling the firmware.

The fakeCompressor feature can entirely be turned ON and OFF with
the *USE_FAKE* Value, and within the feature the emulated Compressor (12V power supply)
can be turned OFF and ON with the *FAKE_COMP_ON* Value.
When the feature is turned ON, **the code stops using the "real" Voltage (vSense.cpp) and
Temperature (tSense.cpp) sensors**, and instead, the fakeCompressor
outputs "fake" *FRIDGE_TEMP*, *COMP_TEMP*, and *MECH_TERM* Values
for use by the rest of the system.

As with the real compressor, if the power supply is turned OFF,
or the power supply is OFF and the *COMP_RPM* is zero (the compressor
is not running),  the freezer and compressor will move towards the
**FAKE_AMBIENT** temperature and level off at that temperature.

If the power supply is turned on, then the fakeCompressor is sensitive
to the *COMP_RPM* being set by the Controller, and when the COMP_RPM is
non-zero, the fakeCompressor will emulate the cooling of
the freezer, and heating of the compressor.

There are a number of **Values** that control the fakeCompressor
that are presented on the *Config* tab of the WebUI if the feature
is compiled in.

- **USE_FAKE** - *bool 0/1* - Whether to use the fakeCompressor or not.
- **RESET_FAKE** - *command* - A command to reset the fake Compressor to some initial values
  (FRIDGE_TEMP=-16C, COMP_TEMP=FAKE_AMBIENT)
- **FAKE_COMP_ON** - *bool 0/1* - Emulates turning the 12V power supply Off or On.
- **FAKE_AMBIENT** - *float, temperature, default(26.67C)* - the ambient temperature used for the model.
- **FAKE_PERIOD** - *integer, default(30)* - approximate number of seconds for a FRIDGE_MODE=TEMP or
  MECH cooling cycle

I have messed a lot with this stupid thing.  I have had luck with FAKE_PERIOD=10,
to show a 10 second cycle, and FAKE_PERIOD=30 to show a 30 second cycle. However,
modelling a real compresser/freezer combination is exceeingly complicated, and
so the model is not guaranteed to work over all ranges.

The remainder of the Values associated with the Fake Compressor, are listed here,
without descriptions or default values.  Please see fakeCompressor.cpp if
you really want to mess with this.

- **COOLING_ACCEL** - *float, -10000-10000*
- **WARMING_ACCEL** - *float, -10000-10000*
- **HEATING_ACCEL** - *float, -10000-10000*
- **COOLDOWN_ACCEL** - *float, -10000-10000*
- **MAX_COOL_VEL** - *float, -10000-10000*
- **MAX_WARM_VEL** - *float, -10000-10000*
- **MAX_HEAT_VEL** - *float, -10000-10000*
- **MAX_DOWN_VEL** - *float, -10000-10000*
- **FAKE_PROB_ERROR** - *integer 0..100, not implemented* -
  the odds of the fakeCompressor failing to start,
  or encountering a compressor error while running

The fakeCompressor emulates a warming-cooling cycle which takes
approximately **FAKE_PERIOD**, in seconds, to complete.  The
default is 30 seconds, or in other words, over about 30 seconds,
with the default SETPOINTS, the fakeCompressor FRIDGE_TEMP will warm
to the SETPOINT_HIGH, temperature, the MECH_THERM will be turned on,
and, depending on the FRIDGE_MODE and SETPOINTS, the Controller will
set the COMP_RPM to a non-zero value, turning the compressor on and
the fakeCompressor will cool the refrigerator, and heat the compressor
until the RPMs goes back to zero, ending the cycle.

Here is a chart with the default settings of TEMP_SENSE_SECS=30 and
FAKE_PERIOD=30 where you can clearly see the fridge temperature
(Blue line) rising until the SETPOINT_HIGH is reached, at which point
the fakeCompressor turns on the MECH_TERM (Gold line), and the Controller
turns on the COMP_RPM (Green line), at which point the COMP_TEMP (Orange
line) starts rising, and the FRIDGE_TEMP starts falling.

![notes_fakeCompressorCycle.jpg](images/notes_fakeCompressorCycle.jpg)

When the FRIDGE_TEMP hits SETPOINT_LOW, the Controller turns off the
COMP_RPM (the compressor gets turned off), and the temperature of the
fridge begins (slowly) rising, and the temperature of the compressor starts
(more quickly) falling, towards FAKE_AMBIENT.
This helps prove that the basic **FRIDGE_MODE**, **DataLogging** and **Charting**
software is working.

The fakeCompressor also outputs random values for the 5V and 12V power supplies so that **Plotting**
can basically be tested.  Here is an example plot where you can see the fake 12V
power supply slightly changing:

![notes_fakeCompressorPlot.jpg](images/notes_fakeCompressorPlot.jpg)




[**Next**](analysis.md) A detailed Analysis of the Inverter ...
