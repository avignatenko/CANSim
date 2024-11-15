#pragma once

#include "Common.h"

class MCP23017;

class Pin
{
public:
    Pin(uint8_t pin) : pin_(pin) {}

    virtual void pinMode(uint8_t mode) = 0;
    virtual uint8_t digitalRead() = 0;
    virtual uint8_t digitalWrite(uint8_t value) = 0;
    virtual void attachInterrupt(void (*userFunc)(void), int mode) = 0;

    uint8_t toInt() const { return pin_; }

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
    virtual void attachInterrupt(void (*userFunc)(void), int mode) override;
};

class MCP23X17Pin : public Pin
{
public:
    MCP23X17Pin(MCP23017& mcp, uint8_t pin) : Pin(pin), mcp_(mcp) {}

    virtual void pinMode(uint8_t mode) override;
    virtual uint8_t digitalRead() override;
    virtual uint8_t digitalWrite(uint8_t value) override;
    virtual void attachInterrupt(void (*userFunc)(void), int mode) {};

private:
    MCP23017& mcp_;
};