# ABB Vent Control: Embedded Architecture

Embedded Software is written for LPC1549 development board.<br>
Primary language is C++.

The project creates a smart system solution for adjusting and monitoring pressure in ventilation, using ABB frequency controller simulator.

# Overview

Pictures and general description of the project is provided in root folder.

The system consists of several components:
* ABB Frequency Controller simulator (Arduino)
* Ventilation and servo, controlled by ABB drive.
* LiquidCrystal display
* Button module with 3 buttons
* Wi-Fi module<br>

The main idea is to provide two operation modes for the system: **automatic** and **manual**:
* In manual mode frequency is controlled by the user, other items must not be adjustable.
* In automatic mode system's target prussere is controlled by the user, other items must not be adjustable.<br>

The system expects input from WEB UI (via MQTT payload) and Button module.<br>
Considering several input and output methods, software needs to syncronize LCD, WEB UI with current machine's state.

# Hardware Architecture

The following diagram describes the hardware architecure, that includes the components described above, in the abstract form.

![Hardware Architecture](../images/hw_architecture.png)

More detailed information is provided further in schematics.

## Schematics
![Overall project arcitecture](https://i.imgur.com/IRX7cOy.png)
Full HD link: [Schematics FULL HD](https://i.imgur.com/IRX7cOy.png)

## Components used

MCU LPC1549: [MCU](https://www.nxp.com/docs/en/data-sheet/LPC15XX.pdf)<br>
LCD: Not specified, containg backlight<br>
WI-Fi module: Not specified, PCB provided by Metropolia UAS<br>
Pressure Sensor SDP6x0-125: [SDP](https://www.scribd.com/document/144110195/Sensirion-Differential-Pressure-SDP6x0)<br>
ABB Frequency Simulator: [ABB](https://library.e.abb.com/public/25ba8ab3f04e2266c12572e9004ffafe/EN_ACH550_EFB_D.pdf)<br>

# Software Architecture

Abstract software architecture is described in the figure below.

![Software](../images/sw_architecture.png)

As we can see, the main components of the architecture are:
* State Machine
* MQTT payloads
* Menu button inputs
* Liquid Crystal Display output
* Main Program body

In addition to interaction described in a diagram, main program generate eTick event for state machine at 1T rate.

Colors on the diagram describe when interaction is happening.<br>
<span style="color: purple;"><b>Purple</b></span> - interaction happens immediately<br>
<span style="color: black;"><b>Black</b></span> - interaction happens periodically (specified by label).<br>
1T = 1 Tick = 1000 ms.

Some of the complex concepts are described in details.

## State machine

State machine architecture is described in the UML, introduced in this section.

Member variables of state machine:<br>
Pressure, Setpoint, Frequency, Mode, Timer, Status, Pointer to ABB drive, Pointer to SDP pressure sensor

Statuses:<br>
0 - status **OK**<br>
1 - status **LOADING** (unused in current implementation)<br>
-1 - status **HARDWARE_FAILURE**<br>
-2 - status **TIMEOUT** (setpoint unreachable)<br>

The functions introduced in the UML are the member dunctions of state machine.<br>
Additionally, there should be a member function for exporting data and status (returning members).
* **read_pressure** - read from SDP
* **set_frequency(v)** - set frequency to v
* **adjust_frequency** - automatically adjust frequency for a specific step
* **modbus_heartbeat** - keep modbus connection awake<br><br>

![State Machine UML](../images/state_machine.png)<br>

The described is recommended and current implementation. Differences and improvements are possible.

## Main handlers

The idea behind main handlers is to have a "connector" that will keep WEB UI, LCD and State machine syncronized.<br>
In order to achieve this syncronization, handler functions are called with periodicity described in the software acrhitecture diagram.<br>
There are four handlers that follow the same pattern:<br>
pressure_handler, frequency_handler, setpoint_handler, mode_handler

General algorithms for handlers:<br>
**handler(value):<br>**
1. If LCD_CURRENT != value, then update LCD
2. If STATE_MACHINE_CURRENT != value, then update STATE_MACHINE

## Usage of external libraries

One external library was used for JSON parsing.<br>
Credits: [JSON](https://github.com/nlohmann/json)
