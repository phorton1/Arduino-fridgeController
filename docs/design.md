# fridgeController Design

**[Home](readme.md)** --
**Design** --
**[Electronics](electronics.md)** --
**[Build](build.md)** --
**[Notes](notes.md)**


This page presents a more detailed look at the design of the
fridgeController.


### A6. PWM to control C-T current

Although the simplest physical way to control the compressor speed is
the use of a fixed resistor between the C and T pins, with a dead-short
equaling 2000 rpms (min), and a 1.5K resistor equaling about 3500 rpms (max),
the doc mentions that the compressor speed can be controlled by PWM,
and it is actually the current flowing between the T and C pins that
is important.

This is great news, because it means that you can vary the compressor
speed (and turn it on and off) with a simple NPN transitor with the
collector connected to T, the emitter to C, and a PWM signal coming
from the MPU to the base (thru a 4.7K resistor).

Which means that a bulky and difficult to use relays and fixed resistors,
and/or digital potentiometers are not needed to control the compressor
speed.  The implementation of the PWM signal is documented in the
C++ source code.


## miniBox Details

The **miniBox** is intended to remain in place and be testable,
and nominally usable, without the controller being present.
It allows the compressor to be run via switches, at low or
high speed in the event the ESP32 based controller fails,
or is not present (i.e. in development or being repaired).
It has the following connections:

  The fan is to be rated 12V at 0.7A max and allowed to have surges of
  a few seconds at 1A. FWIW, this is the largest current draw item from
  any of those 6 pins!


## Controller Details

The controller sits in a closet out of view, so it will use a single
external WS2812B LED, visible anywhere in the salon,  as an indicator
(green=power, blue=running, maybe cyan==high speed, red flashing=error,
etc).

It will have an LCD and two or three buttons to allow for diagnostic
messages, to program setpoints, and to display the current
freezer and/or refridgerator temperature(s).




### Pump running indicator (BLUE LED on controller box)

Prevously, on my boat, I had hooked up a blue "12V LED" between the the FAN/DIODE+
and FAN- pins, and, with the 101N0212, that LED acted like a "pump running" indicator.

The previous Waeco board ran the FAN/DIODE+ and FAN- signals to a relay that ran
both the FAN, and a potential external cooling water pump (or in my distant past to
another relay far away on the boat that subsequently actually turned on the
water pump).

However, with the 101N0200 on my desk, the fan is turned on not only when the pump
runs, but also in any error states (pending a restart effort 60-90 seconds later).

Therefore, apparently, I cannot solely use the "fan running" as an indicztor that
the pump is running.  Instead I am forced to use a combination of "there
are no errors" and "the fan is running" to show a succesful pump running state

Once again, more testing once I create a device and actually hook it up to the
101N0212 will perhaps provide more information.



[**Next:**](electronics.md) The fridgeController electronics ...



