# fridgeController - for Danfoss BD50 w/inverter unit



The original **Waeco Adler Barbour Cold Machine Board F-1886 Rev2** used on
my boat's refridgeration system burned out.  It has failed several
times, and from the beginning, and over the 17 years I have had the boat,
I have jury rigged solutions for controlling the temperature of the freezer,
having indicator lights that I can see, and so forth.

The compressor is a **Danfoss BD50** and uses the typical **12V Inverter Module**
to actually run the compressor.  For many years I fought with water cooling the
refridgerator, but after changing many hoses and pumps, over and over, I decided
to just let it be air cooled.

In any case I have decided to trash the original controller board and wiring harness,
and build my own **ESP32** based **Thermostat and Controller** using my
[**myIOT**]](https://github.com/phorton1/Arduino-libraries-myIOT)
architecture to also provide a consistent **WebUI** and remote monitoring capabilities.

The circuit will NOT include a relay to control a water cooling pump, but that
could easily be added.

Unfortunately there are no schematics of the Danfoss Inverter and so I have had
to reverse engineer it's behavior empirically.   I have two different units.
There is an **101N0212** currently on the pump.  I also have a spare **101N0220**
I used for reverse engineering on my desk (wihtout a compressor).


## A. Inverter Analysis

The Inverter has the following blade connectors, from top to bottom:

- **BATT-** - GND. connected directly to the 12V battery negative (in my case, a negative shunt)
- **BATT+** - connected to the 12V battery positive thru a fuse (in my case a 15A breaker)
- **FAN/DIODE+** - goes, basically, to BATT+, when there is power to the inverter
- **FAN-** - pulled towards ground (to BATT+ minus 12V) via a regulator, to turn the fan on.
  Current limited to 0.7A with 1A surges allowed for a few seconds.
- **DIODE-** - pulled to ground to flash a led.  Current limited to 10ma.
- **C** - common thermostat terminal. At ground potential.
- **P** - optionl battery cutoff resistor to C. At 5V potential. With no resistor the default
   battery cutoff is 9.6V.  With a 33K resistor it is something like 11V.
- **T** - thermostat terminal. has 5V potential. Short to C gives 2000 rpm, 1.5K resistor to C gives
  max 3500 rpm


### A1. Cutoff Voltage

The documentation "standard battery protection settings" says that the
default cutoff is 10.4V, restored at 11.7V, and can be adjusted with a
resistor between C and P as shown in tables.

On my 101N0220, the inverter, with no connection, appears to cut out at
10.3V and come back on at 12V, which is close to the documentation.

I tried shorting C and P, and a few resistor values, and the table appears
to be roughly accurate.

The Waeco controller board I am replacing had a 1.5K resistor between C and P,
but I am leaving it out, in favor of the defaults, in my new implementation.
There is a blank place to solder a resistor into the MiniBox for an fixed
alternative voltage cutoff.


### A2. FAN/DIODE+

I measure less than 1/2 ohm between this and BATT+.  That is likely within
the limits of my meter, connections, wires, and probes.  So, basicially,
I believe BATT+ and FAN/DIODE+ are connected inside the inverter.

Varying BATT++ from 10 to 14V on my power supply causes FAN/DIODE+ to track
the change exactly as far as I can tell.

I will use FAN/DIODE+ as a proxy for the BATT+ voltage to the inverter
for displaying the battery voltage.


### A3. FAN-

The documentation says that a fan connected between FAN/DIODE+ and FAN- will
get 12V of regulated power, 0.7A continuous with 1A surges allowed, and that
the inverter will fault and give a 2 flash error code if the fan draws more
than 1A.

So, it appears FAN- is pulled low, through a 12V regulator, with current sensing,
probably a low ohm shunt, and turned on and off via a mosfet or internal relay.
Note that the FAN will not turn on at all if the BATT++ (==FAN/DIODE+) voltage
is too low and the unit gives a 1 flash error code.

Otherwise, as measured on an oscilloscope, the FAN never receives more than
12V when FAN- is pulled down, and there is a small voltage offset required
for the 12V regulator.   Therefore, for example, if the battery is
14V, FAN-, when on, will be -12V relative to FAN/DIODE+ and approximately +2V
relative to BATT- (ground).

As BATT+ (and hence FAN/DIODE+) falls below 12V, but remains above the battery
cutoff, the fan receives less than 12V.   At 11V of BATT+, for example, it
gets about -10.5V relative to FAN/DIODE+, and it is still about 0.5V above
ground.


### A4. DIODE-

Likewise, DIODE- is pulled to ground, relative to DIODE/FAN+, in order to
flash the red diagnostic LED.   The documentation mentions that this is
automagically current limited to 10ma.

I measured this with an oscilloscope, and it indeed drops to ground potential.
I believe there is a 1K or so internal resistor in the inverter on the DIODE- pin.

This is becausse I have had success hooking a regular 5V LED directly between
DIODE/FAN+ and FAN-, and I have never burnt one out.  There was no resistor used
on the Waeco controller board I am replacing, and that diode lights up fine
with 5V through a 220 ohm resistor, which futher confirms my supposition.

FWIW there is no such thing as a "12V diode", even though you will see it
mentioned on variouse websites.  What these folks apparently mean is a
regular LED with a series resistor (around 1K) soldered to one leg and
covered with heatshrink tubing.  I don't think the extra resistor is needed,
and would only serve to make the LED dimmer.


### A5. C-T connection (pump on & speed)

Shorting the **C** and **T** pins will cause the pump to (start as needed and)
run at the lowest 2000 RPM.  Placing a resistor across these terminal
will cause it to run at higher RPMS.  According to the docs, a 1.5K resistor
will cause it to run at 3500 rpm.

It is actually based on the current.  It is difficult to actually measure
the pump RPMs (especally with no pump hooked up!) to test this.

Note, FWIW, that the T pin actually appears to be driven to +5V relative to
BATT- (ground), probably by a 5V regulator and that the P pin appears to
be at the same potential, probably through a 10K resistor, guessing
from the battery protection resistor values.


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


### A7. Starting, Running, and Errors

The behavior of the inverter is complicated and difficult to explain.

**(a) INITIAL STATE  (no C-T connection)**

Assume that the inverter is powered up with no connection (thermostat)
between the C and T pins.

In this state it will NOT detect an undervoltage.  It brings FAN/DIODE+
upto BATT+, whatever that is.  It brings the T and P pins upto 5V through
a 5V regulator (so they would be below 5V if the BATT+ was less than, say,
6V or so).

The LED is not flashing (DIODE- is not pulled down) and the FAN is not
running (FAN- is not pulled down).

It is only when the C&T pins are connected (a current flows from T to C),
that a "startup attempt" will be made, and it is only after a startup
attempt that an undervoltage will be detected (and/or cause 1 flash
of the diode).

**(b) STARTUP ATTEMPT (C-T connected)**

The fan is turned off if it is on. The error_state is reset to zero and the
diode stops flashing. There is a delay of a few seconds before anything else happens.

Then, if BATT+ is lower than the cutoff voltage, the diode will
start flashing once every four seconds and nothing else will happen. We call this
error_state(1).

Otherwise, with sufficient voltage, it then briefly pulls the FAN- low to check
for a FAN overcurrent condition. If there is a fan overcurrent, it will go into
error_state(2), flashing twice every four seconds, and nothing else will happen.

Otherwise, the inverter makes three rapid attempts to start the pump.  If it
fails to start the pump, it starts the fan and goes into error_state(3),
flashing 3 times every four seconds

**(c) TIMED RESTART**

In an error_state, after 60-90 seconds, the pump will make another startup attempt.

**(d) Undervoltage and Fan Overcurrent**

These checks happen continiously in real time, during a startup attempt, as well
as while the pump is running normally. As soon as an undervoltage or fan overcurrent
is detected, the pump and fan will be turned off immediately.  However, if already
in an error_state (i.e. pending a restart attempt), the error_state (number of flashes)
will not change until the **next** startup attempt.

In other words, If an undervoltage or fan overcurrent occurs while there is an error_state,
the LED keeps flashing the old error_state, and will not start flashing the 1 or 2 flashes
for undervoltage or fan overcurrent until the next restart attempt (60-90 seconds).


**(d) Voltage Restored**

If an undervoltage is restored while in an error_state, the fan will be started
immediately. A restart attempt will take place at the next 60-90 second interval.


**(e) ERROR_STATE**

To summarize, an ERROR_STATE is when the LED is flashing. Once an error state is
established it remains in place until the next 60-90 second restart attempt.



**(f) Toggling C&T**

The above descriptions assume that once C&T are connected, they stay connected.

However, I noted the following during testing.

C&T are normally connected and disconnected by a thermostat (the controller).
So, normally, the LED is not flashing and the pump (and fan) starts and stops
based on the CT connection.

If C&T are disconnected while in an error_state, the FAN will stop, but the
LED will keep flashing the error_state.

However, EVEN IN AN ERROR STATE, if C&T are disconnected then re-connected,
the inverter will immediately do a new start attempt (which has a built in
few seconds of delay).  Note that this subverts the 60-90 second automatic
restart attempt.

Thus, one way to clear an error state, and return the inverter, more or less to
its INITIAL STATE is to toggle the C&T connection.  If the re-connection is
brief, after the fan turns off, but before the delay until the actual
restart attempt happens, essentially this clears the error_state and
(more or less) returns the inverter to its initial state.



## Design Overview

The BATT- and BATT+ will be hooked up directly to the inverter with
only a 15A breaker between BATT+ and the battery bank positive (and an
ammeter shunt between BATT- and the batteries negative).

Apart from that I have decided to break the design into two parts.

The first part is the **miniBox** which is relatively simple, and
which will sit atop the inverter in a 3D printed enclosure.  The
second part will be the more complicated ESP32 based **controller**
which will live in a kitchen cabinet and have an external WS2812b
LED.


## miniBox

The **miniBox** is intended to remain in place and be testable,
and nominally usable, without the controller being present.
It allows the compressor to be run via switches, at low or
high speed in the event the ESP32 based controller fails,
or is not present (i.e. in development or being repaired).
It has the following connections:

- connects to the **BATT-** and **BATT+** via a 2 pin Pheonix plug
  The BATT+ connection is not used, so no fuse is needed.
  Only the BATT- is used as a common ground.
- gets the **other 6 pins** from the inverter (FAN/DIODE+, FAN-,
  DIODE-, C, P, and T) via a custom 3d printed blade connector and
  uses small (22 guage) wires.
  These pins are all low amperage and do not need to be big
  honking wires as was in my archaic previous setup.
- has a 2 pin Pheonix plug for the **FAN**, which connect directly to
  the FAN/DIODE+ and FAN- pins brought in from the inverter.
  The fan is to be rated 12V at 0.7A max and allowed to have surges of
  a few seconds at 1A. FWIW, this is the largest current draw item from
  any of those 6 pins!
- connects to the ESP32 based **controller** via a 6 pin connector and
  an 6 of 8 conductors of 24 guage wire that I have. Once again, these are
  all low amperage signals. No more than 10ma flows over any of these wires
  to the controller.


The miniBox has three indicator LEDs

- **GREEN** connected from FAN/DIODE+ to ground via
  a 1K resistor to show when the FAN/DIODE+ (the inverter
  box generally) has power.
- **YELLOW** connected from FAN/DIODE+ to FAN- via a 1.2K
  resistor to show when the FAN is on.
- **RED** is the flashing diagnostic diode.


The miniBox has two switches that allow the compressor to
be run in an emergency, in the absense of a controller, or
generally, to override the controller.

- OVERRIDE - Turns the pump on, overriding the (possibly missing)
  controller by connecting the T and C pins brought in from the inverter
- LOW/HIGH SPEED - Used only if OVERRIDE is turned on. In the
  LOW position, T and C are shorted, allowing the pump to run
  at low speed (2000 rpm).  In the HIGH position, a 1.5K resistor
  is inserted between T and C, allowing the pump to run at
  high speed (3500 rpm)

When in OVERRIDE, the C pin is disconnected from the controller.
Hence when not in OVERIDE, connected to the controller, it is given a
new name "C2" in the schematics and diagrams to differentiate
from the actual "C" pin brought into the miniBox from the inverter.


### RED diagnostic LED PNP transistor

A note regarding the **RED** diagnostic diode.  Instead of being
directly connected between FAN/DIODE+ and DIODE-, as in most online
documentation, which makes it very difficult to detect when the led is
on from a ESP32, in order to ease in the implementation of the controller
box, the LED is connected between the FAN/DIODE+ and DIODE- pins through
the collector and base of a **PNP transistor**.

When DIODE- is pulled low, current flows from the FAN/DIODE+ to the
transistor *collector* to the *base*, through the LED, to DIODE-,
with almost no resistance.  The LED lights up normally and gets about
10ma of power.

When the diode is turned on, the full current of FAN/DIODE+
(safely limited to 0.7 to 1amp) is available on the *emitter* pin
of the transistor, which the miniBox sends to the ESP controller
as **DIAG_DIODE**.  By then pulling DIAG_DIODE low through a 15K
(10K + 4.7K) voltage divider on the controller, it is easy to tell
when the diode is turned on, because the center of the voltage divider
will go from zero to 2.5V, which is within the ESP32 ADC ranges.

It took me a while to figure out, design, and test this part of the circuit.


### Emergency Two

In a real emergency, as long as the main 12V power supply to the inverter
is working, the minibox can be unpluged and by shorting the T&C pins on the
inverter the compressor will run at slow speed.



## Controller Box

The controller sits in a closet out of view, so it will use a single
external WS2812B LED, visible anywhere in the salon,  as an indicator
(green=power, blue=running, maybe cyan==high speed, red flashing=error,
etc).

It will have an LCD and two or three buttons to allow for diagnostic
messages, to program setpoints, and to display the current
freezer and/or refridgerator temperature(s).

The ESP32 controller will be powered separately (through a 2 pin Pheonix
connector), so that the Inverter power supply can be turned off and on as
needed without necessarily rebooting the ESP32, thus allowing the ESP32 to detect
and alert about power outages, like a broken 15A breaker, and also to allow
me to restart the inverter (pump), by toggling the 15A breaker, in case of an error,
without necessarily rebooting the ESP32

Thus, the ESP32 will share, albeit by a very roundabout set of wires, the same +12V
and GROUND as the inverter.  It will also receive the GND from the miniBox

The controller will receive the following pins from the miniBox,
via 6 of 8 conductors of 24 guage wire with an 6 pin JST style
connector

- BATT- (GND)
- FAN/DIODE+ (equivilant to BATT+)
- FAN- (pulled towards ground, but never more than BATT+ minus 12V, when fan is on)
- DIAG_DIODE (pulled up to 12V when diode is on)
- C2 (switched C pin from inverter/minibox)
- T (directly from the inverter)

Once again, for sanity we bring another version of the GND (BATT-) from
the miniBox to the controller board via the interconnect cable.



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



### Unresolved Issues

(1) I am waiting for some 6 pin right angle 2.54mm JST connectors.

(2) I am waiting for some DS18B20 temperature probes.
    I have not yet decided how many of these should really be
	on the controller.  At least one, maybe upto 4.
	Additionally, I don't know if I will forgoe the current mechanical
	thermostat or possibly use it in parallel, or as a software
	option.




## Please Also See

This project directly makes direct use of my
[**MyIOT**](https://github.com/phorton1/Arduino-libraries-myIOT)
("my internet of things*) repository, which provides a generalized approach to
building relatively simple parameter driven ESP32 devices that include a Web UI.

Other projects that also make user of the *myIOT* repository include the
[**Bilge Alarm**](https://github.com/phorton1/Arduino-bilgeAlarm)
that I built for my boat, and
[**Wooden Geared Clock**](https://github.com/phorton1/Arduino-theClock3)
that uses an electromagnet to drive a pendulum, and the
[**Wire Cutter and Stripper**](https://github.com/phorton1/Arduino-wireStripper32)
that I most recently made.

The PCBs (printed circuit boards) were **milled** using the
[**cnc3018**](https://github.com/phorton1/Arduino-esp32_cnc3018) CNC
machine that I built.

I hope you will take a few minutes to check out some of these other pages too!!



## Credits

This project directly relies upon the public open source Arduino development system,
the Espressif ESP32 online documentation and additions to the Arduino IDE, as
well as a number open source Arduino libraries

I would like to thank the countless individuals who have contributed to making these
development tools, source codes, and documentation available for free public use, including,
but not limited to:

- [KiCad](https://www.kicad.org/)
- [FlatCAM](https://bitbucket.org/jpcgt/flatcam/downloads/) by **Juan Pablo Caram**
- [Arduino IDE and Libraries](https://www.arduino.cc/)
- [Espressif Systems](https://www.espressif.com/en/products/socs/esp32)
- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)


## License

This program, project, and repository is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 3 as published by
the Free Software Foundation.

These materials are distributed in the hope that they will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Please see [LICENSE.TXT](../LICENSE.TXT) for more information.
