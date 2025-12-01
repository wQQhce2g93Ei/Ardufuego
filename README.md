```
                          (
   (           (          )\ )
   )\    (     )\ )   (  (()/(   (     (   (  (
((((_)(  )(   (()/(  ))\  /(_)) ))\   ))\  )\))(  (
 )\ _ )\(()\   ((_))/((_)(_))_|/((_) /((_)((_))\  )\
 (_)_\(_)((_)  _| |(_))( | |_ (_))( (_))   (()(_)((_)
  / _ \ | '_|/ _` || || || __|| || |/ -_) / _` |/ _ \
 /_/ \_\|_|  \__,_| \_,_||_|   \_,_|\___| \__, |\___/
                                          |___/
```
# Ardufuego
##### An Arduino RF controller that emulates Heat N Glo fireplace remotes
###### *Broken links subject to "you'll have that."*

## Purpose
- Control the fireplace via common home automation technologies (MQTT)
- End reliance on poorly designed, battery hogging remote
- Automate start and end patterns (fan speed, flame height)

## Introduction
This project emulates the protocol for an RC300 Heat & Glo fireplace remote. Functionality is currently limited to controlling the on / off state, flame height, fan speed, AUX1, AUX2, and pilot setting. Convenience functions have been added to provide optimal start up and shut down procedures.

## Hardware Setup
- RF 433 Mhz Transmitter and Receiver: Ex. Gowoops via Amazon
<img src="/images/RF.png" width="500">

- Arduino: Ex. Keyestudio Mega via Amazon
<img src="/images/mega.jpg" width="500">

- Breadboard and jumpers
- 5v Power supply

## Message Format
### General
Data in the messages is represented as binary/decimal where it makes it easiest.
The RF representation is as follows.

* 0 = HIGH signal for 380µs
* 1 = HIGH signal for 880µs

Every bit is followed by a well known LOW signal period (420µs in the header, 700µs in the rest of the command)

Every remote control button press sends the same command 10 times.

### Sample RF Capture (On command)
Sample command captured in [Universal Radio Hacker](https://github.com/jopohl/urh)
<img src="/images/rf-capture-on-command.jpg" width="1000">

### Command Format
Between commands there is a 90,900µs empty period

`[Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command]`

Each command is made up of a header, 3 start messages, and 3 function messages

`[Header] [Start1] [Start2] [Start3] [Function1] [Function1] [Function1]`

### Header Segment
The header value is binary 1010 followed by a 830µs empty period (1250µs when combined with the LOW after the last bit)

`1 0 1 0 [pause]`

### Message Segment
The message segments are 8 bits followed by a 820µs empty period (1520µs when combined with LOW after last bit)

Example message segment: `0 1 0 1 0 1 0 1 [pause]`

#### Start Segments
The first 3 body segments do not change between functions and are referenced as "start" segments

`[Header] [Start1] [Start2] [Start3] [Function1] [Function2] [Function3]`

#### Function Segments
The second 3 body segments carry the button function and value data and are referenced as "function" segments

`[Header] [Start1] [Start2] [Start3] [Function1] [Function2] [Function3]`

Each of the 3 function segments repeats the same data in different formats

`[Function1] [Function1] [Function1]`

The first 5 bit pairs of each function segment represent the action (on/off/flame height/fan speed/etc) and the last 3 bits represent the value.

Example:
Flame 5

```
[----------------------- Function1 ----------------------]
[Func] [Func] [Func] [Func] [Func] [Value] [Value] [Value] [pause]
   0      0      1      1      0      1       0       1    [pause]

```
This can be simplified to hex 0x35 (3 for the command and 5 for the flame value)

### Retrieve Remote Control Information
This is the important and necessary bit to use this with your fireplace.

1. The first two start messages seem to vary by remote. Capture yours using an RF receiver
1. The "offset" for the second and third function messages needs to be found.


The easiest way to do this is capture the Power On command. Once the RF signal is captured look for vertical bars. Thick bars are 1s and thin bars are 0. Look below for expected bits and X for unknown bits
that should be transcribed from the captured RF command.

```
[header] | [Start1]  | [Start2]  | [Start3]  | [Function1] | [Function2] | [Function3] | [pause]
  1010   | XXXX XXXX | XXXX XXXX | 0011 0011 |  0000 0001  |  XXXX XXXX  |  XXXX XXXX  | [pause]
```

In FireplaceRF.h update the following values.
* Start1 -> put in first value in startMessage in hex (0xC4) or binary (0b11000100)
* Start2 -> second value in startMessage in hex (0xB1) or binary (0b10110001)
* Function2 -> set Function2 variable just below this in hex (0xFE) or binary (0b11111110)

## Software Setup
The following files are significant to this project:
- FireplaceRF
  - Library to include in Arduino sketches for controlling a fireplace via RF

- Examples/simple.ino
  - An example Arduino sketch that imports and utilizes FireplaceRF

- Examples/RF_Tester.ino
  - An example Arduino sketch that reads an RF signal and prints the timings to the serial monitor

- Examples/RF_Transmitter.ino
  - An example Arduino sketch that transmits an RF signal

## Getting Started
1. Connect ground pin of the RF TX board to ground
2. Connect VCC pin of the RF TX board to 5V
3. Connect data pin of the RF TX board to TX pin (D0 in test sketch)
4. Import the Ardufuego project into PlatformIO
5. Upload to board

## FireplaceRF API
```
FireplaceRF fireplace(x);
```
Creates a FireplaceRF to transmit on pin x where x is an int that corresponds to an interrupt pin.
```
fireplace.on();
```
Turns the fireplace on.
```
fireplace.off();
```
Turns the fireplace off.
```
fireplace.setFan(x);
```
Set the fan speed to x where x is an int from 0 to 3
```
fireplace.setFlame(x);
```
Set the flame height to x where x is an int from 1 to 5
```
fireplace.setAux1(x);
```
Set the AUX1 setting to x where x is an int from 0 to 3
```
fireplace.setAux2(x);
```
Set the AUX2 setting to x where x is an int from 0 to 1 (OFF/ON)
```
fireplace.start();
```
fireplace on
flame height 1
fan speed 0
```
fireplace.stop();
```
fan speed 0
fireplace off

## Credits
### Authors
* Matthew Nichols

### License
Apache License, Version 2.0 Apache License Version 2.0, January 2004 http://www.apache.org/licenses/
