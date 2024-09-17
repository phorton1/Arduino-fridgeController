# fridgeController Electronics

**[Home](readme.md)** --
**[Design](design.md)** --
**Electronics** --
**[Build](build.md)** --
**[Notes](notes.md)**


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

- BATT- (GND)
- FAN/DIODE+ (equivilant to BATT+)
- FAN- (pulled towards ground, but never more than BATT+ minus 12V, when fan is on)
- DIAG_DIODE (pulled up to 12V when diode is on)
- C2 (switched C pin from inverter/minibox)
- T (directly from the inverter)

Once again, for sanity we bring another version of the GND (BATT-) from
the miniBox to the controller board via the interconnect cable.


[**Next:**](build.md) Building the fridgeController ...
