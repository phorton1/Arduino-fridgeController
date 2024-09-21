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







### Temperature Sensors

There are two 3 pin connectors on the Controller board for
connecting DS18B20 temperature sensors.  Although there
are only two connectors, they share the same OneWire interface,
and, in reality, upto 64 sensors *could* be conneced to
the fridgeController.








[**Next:**](electronics.md) The fridgeController electronics ...



