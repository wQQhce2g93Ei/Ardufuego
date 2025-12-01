# ESPHome Setup
I made this five years ago and I don't recall all the bits to make this work. Please submit a PR if you implement this. Someone with a fresh experience should be able to make more sense out of what is/isn't required to get this setup.

Note: As far as I understand the protocol you need to know your remote's ID.
See this not in [FireplaceRF.h](/lib/FireplaceRF/FireplaceRF.h#L77).
```
    // YOUR TASK to use this with your fireplace!
    //   1. The first two start messages seem to vary by remote. Capture yours using an RF receiver.
    //   2. The "offset" for the second and third function messages needs to be found.
    //
    // The easiest way to do this is capture the Power On command. Once the RF signal is captured look for thick
    // and skinny lines. Thick lines are 1s and thin lines are 0. Look below for expected bits and X for unknown bits
    // that should come from the captured RF command.
    //
    // [header] | [Start1]  | [Start2]  | [Start3]  | [Function1] | [Function2] | [Function3] | [pause]
    //   1010   | XXXX XXXX | XXXX XXXX | 0011 0011 |  0000 0001  |  XXXX XXXX  |  XXXX XXXX  | [pause]
    //
    // Start1 -> first value in startMessage in hex (0xC4) or binary (0b11000100)
    // Start2 -> second value in startMessage in hex (0xB1) or binary (0b10110001)
    // Function2 -> set Function2 variable just below this in hex (0xFE) or binary (0b11111110)
```
I only have one of these fireplaces and one remote so I don't know the details of this. You can compare my value, the value of the repo I forked this code from, and your value and maybe figure out more details. Please make a PR if you make more progress on this front. Or link me to your fork if you do a bunch more work and make this a more native ESPHome climate component.

## Hardware
Make an ESP32 or ESP8266 based device with a 433MHz transmitter. The only important part here is the connection from the ESP GPIO to the 433MHz transmitter.

![When good ideas get out of hand](./esp8266.png "Hack Job")

## ESPHome Config
See [esphome.yaml](./esphome.yaml) for a version of yaml similar to what I use for this module. I do not recall off hand how custom_component in ESPHome works. I now see custom_component is replaced by external components. I do not know how that will work with this and I don't plan to change this now. Please submit a PR if you figure that part out. That'd be great.

Note: This requires an MQTT server setup in this device's ESPHome yaml as well as setup in Home Assistant.

## Home Assistant Config
See [ha-configuration.yaml](./ha-configuration.yaml) for the code I could find in my setup. Again this was five years ago so I don't recall all relevant details on how I made this work but this seems like most of it.

Note: I do use an external temperature sensor not included on my RF transmitter that's hooked into Home Assistant on its own completely separate from this project.

This is what it looks like in my current Home Assistant UI when setup as a climate object in addition I've broken out some of the other functions to edit manually.

![Home Assistant Fireplace UI](./ha-fireplace-panel.png "Fireplace")
