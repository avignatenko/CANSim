#pragma once

#include "Common.h"

class Adafruit_MCP23X17;

class Pin
{
public:
    Pin(uint8_t pin) : pin_(pin) {}

    virtual void pinMode(uint8_t mode) = 0;
    virtual uint8_t digitalRead() = 0;
    virtual uint8_t digitalWrite(uint8_t value) = 0;

protected:
    uint8_t pin_;
};

class ArduinoPin : public Pin
{
public:
    ArduinoPin(uint8_t pin) : Pin(pin) {}

    virtual void pinMode(uint8_t mode) override;
    virtual uint8_t digitalRead() override;
    virtual uint8_t digitalWrite(uint8_t value) override;
};

class MCP23X17Pin : public Pin
{
    public:

    MCP23X17Pin(Adafruit_MCP23X17& mcp, uint8_t pin) : Pin(pin), mcp_(mcp) {}

    virtual void pinMode(uint8_t mode) override;
    virtual uint8_t digitalRead() override;
    virtual uint8_t digitalWrite(uint8_t value) override;
private:
    Adafruit_MCP23X17& mcp_;
};