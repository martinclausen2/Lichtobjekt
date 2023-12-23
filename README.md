# Lichtobjekt

The "Lichtobjekt" projects allows to create portable lighting appliances.

## Features

- Two independent channel on board for LED driver
- Flicker free brightness control through current adjustment
- Integrated Li Ion battery charger
- Status LED to indicate power on, channel, standby, and charging status
- Brightness measurement to adjust brightness at power on
- IR remote control (receiver and sender)
  -	Option to network devices
- Single rotational encoder and switch needed to control
- Embedded terminal to control settings, clock, and alarms

## Technical Features
- ARM Cortex M3 based STM32L151 microcontroller based
  - EEPROM for permanent storage of settings
- RC5 based communication (sender and receiver)
- Two TPS61165DBVT step up LED driver with PWM adjustment of LED driving current up to 350mA
- High dynamic brightness measurement with low cost phototransistor SFH320
- Low power design
  - Shutdown for IR receiver and voltage measurement
  - MCU power down and frequency adjustment
  - Low power logic voltage generations with MCP1703
- Integrated Li Ion charger based on BQ2057WSN
  - Battery temperature monitoring
  - Charging status
- PCB prepared (not used in current software) for 
  - Acceleration sensor MMA8854
  - Pin header for extension with additional I2C devices, LCD or LED drivers
  - RTC RV3029-C2 - currently the MCU's internal RTC is utilized 
  - EEPROM from 24CXXX series with I2C connection - currently the MCU's internal EEPROM is utilized

# Design description

## Hardware

### MCU
The STM32L1 series features for this application relevant features like a large number of timers, specific low power modes, and an embedded EEPROM.

The MCU can be programmed and debugged via a custom pin header, that also exposes the serial port.

### LED Driver
To drive a sufficient number of LEDs for edge illumination without efficiency reducing current balancing resistors, a LED driver with a step-up topology was selected. A two cell battery increases the efficiency of the step-up LED driver.

### RC5 receiver and sender

A IR emitting diode is driven by a small MOSFET from the PWM output of a timer in the MCU.
The output of the IR receiver is sampled at a rate of about 4.5kHz by the MCU. The power supply of the IR receiver is cut in standby mode to prevent the battery to be drained prematurely.

### Brightness Sensor

The ambient brightness is sensed via an in expensive a broadly available phototransistor SFA 320. The enhance the dynamic range, the MCU can switch the bias current between two different levels.
The precision is fully sufficient to adjust the brightness of the power LEDs at power on and dim the stand by LED.

### Battery Charger

The battery charger follows the datasheet of the BQ2057 charging controller. The input voltage should be 9 V to avoid excessive power dissipation in the charging current regulating power transistor.
In newer hardware revision the MCu could switch off the charger during overvoltage conditions - However, the implementation in software is currently pending.
The status output is read by the MCU, that controls the status LED.
A temperature sensor is used to stop the charging is an overtemperature condition of the battery is detected.

## Software

TBA

## Known Limitations
- Very low power modes of MCU cannot be used, since to polarity of the encoder switch is not suitable
- The MCU does not switch the battery charger off, if the input voltage would lead to a thermal overload of the charging transistor
The batch of pcb made is missing the required hardware prerequisites, the latest version contains the required circuits, but has not been manufactured yet.
This PCB batch is also missing the MOSFET to shutdown the battery temperature and external voltage measurement. Due to the high leakage current of the charging transistor, the battery is drained with about 400µA also during power down.

# User Manual

The user interface is rather simplistic.

Turning the encoder increases or decreases the brightness.
The integrated push button is used to select functionality and change between on and off.
Detailed settings can be adjusted via the serial terminal.

Coming from standby a short button press switches the light on. A long button press activates the networked mode, where other compatible lighting appliances receive commands to follow the on and off status and the brightness settings.

During power on the short button press selects between the two individual LED strings and a mode where the brightness is modulated between the two channels.

When the button is pressed, the status led previews a for a short time the next selected functionality.

Standby can be reached via a long button pressed from power on mode.

| LED Color 	| Condition | Status   | 
| :------------ | :------ | :------- |
| red			|         | standby  |
| white			|         | power on |
| blue flashing |         | charging |
| green flash	| while adjusting brightness | maximum brightness reached |
| red flash		| while adjusting brightness | minimum brightness reached |
| white flash	| pressing the button short | selected first LED channel  |
| red flash		| pressing the button short | selected second LED channel |
| pink flash	| pressing the button short | selected fading mode	      |
| blue flash	| pressing the button long to power off | duration of flash corresponds to the charging of the battery |

The white flash is actually not visible, since the LED is already shining white.

# Configuration Manual

The serial port uses the following parameter 115200 Baud rate, data site 8, parity none, stop bits 1

The commands can be listed by sending the command "help".

## Available Commands

### Set Brightness

"bright"

set brightness values - [type] <-c channel_no> <-b brightness_value>

### Get External Brightness

"getextbright"

Get the external brightness from the phototransistor

### Infrared Remote Control

"remote"

set infrared remote parameters - <-a address> <-rm receiver mode> <-sm sender mode> <-sa send address> <-sd send data>

### Set Date and Time

"time" 

set time of RTC - [hour] [minute] [second]

"date"

set date of RTC - [2 digit year] [month] [day] [w] with w weekday

"timestamp"

get date and time from RTC

### Alarm

"alarmschedule"

set alarm schedule - [alarm no] <-w weekday> <-h hour> <-m minute>

"alarmsetting"

set alarm parameters - <-f time to fade-in light>
	
"alarm"

trigger, reset, set alarm skip count - <-a 1 | 0> <-s alarm skip count>

### Power and Battery

"power"

switch light set min external power and battery voltage and calibration- <-l 1 | 0>\r\n<-ub battery voltage> <-mub minimum battery voltage>\r\n<-up external power voltage> <-mup minimum external power voltage difference>"

### Reset Settings

"reset"

reset settings to factory defaults

### Status LED

"statusled"

flash status led  - [flash count]

### Fadelight

"fadelight"

mood light - <-f 1 | 0> time - <-t time> <-b brightness> <-mb maximum brightness>

### Version

"version"

show STM32 software version