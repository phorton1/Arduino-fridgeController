# fridgeController Electronics

**[Home](readme.md)** --
**[Design](design.md)** --
**Electronics** --
**[Build](build.md)** --
**[Notes](notes.md)**


## Electronics Overview

It all starts with the 8 pins on the 101N0212/101N0220 (Secop) Danfoss Compressor Inverter:

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

Most of these pins are brought directly to the Controller directly
through the miniBox.  The BATT- and BATT+ are brought to the miniBox
through via "tees" from the wires to the inverter with 20 guage wire.
The other 6 pins are from the inverter are brought to the miniBox via
a custom made blade connector, through 22 guage wires, to a 6 pin JST
connector.


### miniBox BATT+ inline fuse

We "tee" off separate wires from the BATT- and BATT+ into the inverter
to go to the miniMox 2 pin Pheonix connector.

The BATT+ to the inverter already has a 15A breaker near the battery bank,
but we also place a 2A fuse inline in the BATT+ sent to the miniBox for
added protection.


### FAN/DIODE+ has a (green) LED indicator

There is a (green) LED indictor from FAN/DIODE+ to ground (BATT-)
through a 1K resitor.  The LED comes on when the FAN/DIODE+ goes
high.

FWIW, in all of my experiments, the FAN/DIODE+ is a proxy for
the BATT+ level.  It tracks the BATT+ level well past 12V
in my experiments.

### FAN/DIODE+ and FAN- hooked directly to the FAN and Controller

The FAN/DIODE+ and FAN- JST pins into the miniBox are
connected directly to another 2 pin Pheonix connector to
the fan.  Note that FAN- is pull down to "FAN/DIODE+ minus
12V", and not necessarily to ground.  It is also current
limited to 1A by the inverter.

In addition, there is a (yellow) FAN indicator LED that is
connected between FAN/DIODE+ and FAN- with a 1.2k ohm current
limiting resistor.

Finally the FAN/DIODE+ and FAN- miniBox JST pins are connected
directly to the Controller's 8 pin JST connector on the miniBox.


### RED diagnostic LED PNP transistor

Most online documentation for the inverter has one connect a
diagnostic LED directly between the FAN/DIODE+ and DIODE- pins
from the inverter. This pair of pins is already current limited
within the inverter to about 10ma, so even though it is 12V,
it will not burn out a regular 5V LED.

However, it is fairly difficult to detect the DIODE- pin
being pulled down from a floating state to near 0V on an 3.3V
MCU (Esp32).

My solution is that, instead of being directly connected between
FAN/DIODE+ and DIODE- the (red) diagnostic LED is connected between
the FAN/DIODE+ and DIODE- pins through the collector and base of a
**PNP transistor**.  This way the emitter of the transistor cleanly
goes from 0V to 12V with the full current of the FAN/DIODE+ pin,
which is current limited to 1A, and makes a much more reliable
DIAG_DIODE and easy-to-use signal for the Controller.



### Controller Box Pin Inputs

In addition to having 12V (BATT- and BATT+) on a separate
JST connector on the Controller, 5 of the 6 remaining pins,
and an extra GND are broght to the Controller on a 6 pin
JST connector.


- BATT- (GND)
- FAN/DIODE+ (proxy for BATT+)
- FAN- (pulled towards ground, but never more than BATT+ minus 12V, when fan is on)
- DIAG_DIODE (pulled up to 12V when diode is on)
- C (directly from inverter/miniBox
- T (directly from inverter/miniBox

Once again, for sanity we bring another version of the GND (BATT-) from
the miniBox to the controller board via the interconnect cable.



### A6. PWM to control C-T current

Although the simplest physical way to control the compressor speed is
the use of a fixed resistor between the C and T pins, with a dead-short
equaling 2000 rpms (min), and a 1.5K resistor equaling about 3500 rpms (max),
the doc mentions that the compressor speed can be controlled by PWM,
and it is actually the current flowing between the T and C pins that
is important.

This is great news, because it means that we can vary the compressor
speed (and turn it on and off) with a simple NPN transitor with the
collector connected to T, the emitter to C, and a PWM signal coming
from the MPU to the base (thru a 4.7K resistor).

This means that bulky and difficult to use relays and fixed resistors,
and/or digital potentiometers are not needed to control the compressor
speed.  The implementation of the PWM signal is documented in the
C++ source code.


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
are no errors" and "the fan is running" to show a succesful pump running state.

The blue LED is handled by the ESP32 in software.




[**Next:**](build.md) Building the fridgeController ...
