/*
  FireplaceRF - Library for transmitting to a Hearth & Home compatible RF fireplace.
  Created by Matthew A. Nichols, January 15, 2018.
  Released into the public domain.
*/

#ifndef FireplaceRF_h
#define FireplaceRF_h

#include "Arduino.h"
#include <unordered_map>

class FireplaceRF
{
  public:
    FireplaceRF(int pin);
    void start();
    void stop();
    void on();
    void off();
    void setFan(int speed);
    void setFlame(int level);
    void setAux1(int level);
    void setAux2(int level);

  private:
    typedef struct CommandData
    {
      unsigned char command;
      unsigned char commandEnc;

      CommandData(const int commandId)
      {
        command = commandId;
        commandEnc = (commandId + Function2 - 1) % 256;
      }
    } CommandData;

    // Data in the messages is represented as binary/decimal where it makes it easiest.
    // The RF representation is as follows.
    // 0 = HIGH signal for 380µs
    // 1 = HIGH signal for 880µs
    // Every bit is followed by a well known LOW signal period (420µs in the header, 700µs in the rest of the command)
    //
    // Each button press sends the same command 10 times. Between commands there is a empty period of 90,900µs
    //
    //      [Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command] [Command]
    //
    // Each command is made up of a header, 3 start messages, and 3 function messages
    //      [Header] [Start1] [Start2] [Start3] [Function1] [Function1] [Function1]
    //
    // The header value is binary 1010 followed by an empty period of 830µs (1250µs when combined with the LOW after the last bit)
    //      1 0 1 0 [pause]
    //
    // The message segments are 8 bits followed by an empty period of 820µs (1520µs when combined with LOW after last bit)
    //      0 1 0 1 0 1 0 1 [pause]
    //
    // The first 3 body segments do not change between functions and are referenced as "start" segments
    //      [Header] [Start1] [Start2] [Start3] [Function1] [Function2] [Function3]
    //
    // The second 3 body segments carry the button function and value data and are referenced as "function" segments
    //      [Header] [Start1] [Start2] [Start3] [Function1] [Function2] [Function3]
    //
    // Each of the 3 function segments repeats the same data in different formats
    //      [Function1] [Function1] [Function1]
    //
    // The first 5 bit pairs of each body segment represent the button function and the last 3 bits represent the value.
    // Example Flame 5
    //      [Func] [Func] [Func] [Func] [Func] [Value] [Value] [Value] [pause]
    //         0      0      1      1      0      1       0       1    [pause]
    //     simplified to hex 0x35 (3 for the command and 5 for the flame value)
    //     simplified to decimal as 53 as seen below
    //
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
    const unsigned short StartMessage[3] = { 0b11000100, 0b10110001, 0b00110011 };
    static const unsigned short Function2 = 0b11111110;

    // HIGH and LOW delay values.
    const unsigned short value_1_length = 880;
    const unsigned short value_0_length = 380;

    // Fixed bit delay as the second value in each header pair
    const unsigned short bit_pause_header = 420;

    // Combined with the bit_pause_header, the delay after each header segment
    const unsigned short message_pause_header = 830;

    // Number of 2 bit pairs in the header segment
    const unsigned short number_bits_header = 4;

    // Fixed bit delay for the second value in each body pair
    const unsigned short bit_pause_body = 700;

    // Combined with the bit_pause_body, the delay after each body segment
    const unsigned short message_pause_body = 820;

    // Combined with the bit_pause_body, the delay after each message
    const unsigned int message_pause = 90900;

    // Message is repeated multiple times
    const unsigned int message_repeat = 10;

    // Number of 2 bit pairs in the body segment of the message
    const unsigned long number_bits_body = 8;

    // Max reliable delay for Arduino delayMicroseconds
    const unsigned long arduino_max_delay = 16383;

    const unsigned short header = 0b1010;

    std::unordered_map<std::string, int> commands = {
      {"PowerOn",  0x01},
      {"PowerOff", 0x02},
      {"PilotOn",  0x12},
      {"PilotOff", 0x13},
      {"Flame1",   0x31},
      {"Flame2",   0x32},
      {"Flame3",   0x33},
      {"Flame4",   0x34},
      {"Flame5",   0x35},
      {"Fan0",     0x40},
      {"Fan1",     0x41},
      {"Fan2",     0x42},
      {"Fan3",     0x43},
      {"AUX10",   0x50},
      {"AUX11",   0x51},
      {"AUX12",   0x52},
      {"AUX13",   0x53},
      {"AUX20",   0x60},
      {"AUX21",   0x61},
    };

    int outPin;
    byte pinState;

    void transmit(const unsigned char message, const unsigned char messageSize, const unsigned short bitDelay, const unsigned short endDelay);
    void transmitCommand(CommandData command);
    void sendCommand(std::string command);
    void sendCommand(std::string command, int value);
};

#endif
