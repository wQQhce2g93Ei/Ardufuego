/*
  FireplaceRF.cpp - Library for transmitting to a Hearth & Home compatible RF fireplace.
  Created by Matthew A. Nichols, January 15, 2018.
  Released into the public domain.
*/

#include "FireplaceRF.h"

FireplaceRF::FireplaceRF(int pin)
{
  outPin = pin;
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, LOW);
}

void FireplaceRF::sendCommand(std::string command)
{
  auto commandData = commands.find(command);
  if (commandData != commands.end())
  {
    transmitCommand(commandData->second);
  }
}

void FireplaceRF::sendCommand(std::string command, int value)
{
  char buffer[5] = { 0 };
  std::string valueString = itoa(value, buffer, 10);

  sendCommand(command + valueString);
}

void FireplaceRF::transmit(const unsigned char message, const unsigned char messageSize, const unsigned short bitDelay, const unsigned short endDelay)
{
  for (unsigned char i = 1; i <= messageSize; ++i)
  {
    byte value = (message & (0x1 << (messageSize - i))) == 0 ? 0x0 : 0x1;
    digitalWrite(outPin, HIGH);
    delayMicroseconds(value > 0 ? value_1_length : value_0_length);
    digitalWrite(outPin, LOW);
    delayMicroseconds(bitDelay);
  }

  delayMicroseconds(endDelay);
}

void FireplaceRF::transmitCommand(FireplaceRF::CommandData command)
{
  noInterrupts();

  for (unsigned int x = 1; x <= message_repeat; x++)
  {
    transmit(header, number_bits_header, bit_pause_header, message_pause_header);

    for (int y = 0; y < 3; y++)
    {
      transmit(StartMessage[y], number_bits_body, bit_pause_body, message_pause_body);
    }

    transmit(command.command, number_bits_body, bit_pause_body, message_pause_body);
    transmit(command.commandEnc, number_bits_body, bit_pause_body, message_pause_body);
    transmit(~command.commandEnc, number_bits_body, bit_pause_body, message_pause_body);

    if (x < message_repeat)
    {
      unsigned int endDelay = message_pause;
      while (endDelay > arduino_max_delay)
      {
        delayMicroseconds(arduino_max_delay);
        endDelay = endDelay - arduino_max_delay;
      }

      delayMicroseconds(endDelay);
    }
  }

  interrupts();
}

void FireplaceRF::start()
{
  sendCommand("PowerOn");
  delay(1000);

  sendCommand("Fan", 0);
  delay(1000);

  sendCommand("Flame", 1);
}

void FireplaceRF::stop()
{
  sendCommand("Fan", 0);
  delay(1000);

  sendCommand("PowerOff");
}

void FireplaceRF::on()
{
  sendCommand("PowerOn");
}

void FireplaceRF::off()
{
  sendCommand("PowerOff");
}

void FireplaceRF::setFan(int speed)
{
  sendCommand("Fan", speed);
}

void FireplaceRF::setFlame(int level)
{
  sendCommand("Flame", level);
}

void FireplaceRF::setAux1(int level)
{
  sendCommand("AUX1", level);
}

void FireplaceRF::setAux2(int level)
{
  sendCommand("AUX2", level);
}
