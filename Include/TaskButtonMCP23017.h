#pragma once

#include "Common.h"

#include "FastDelegate.h"
#include "Bounce2MCP23017.h"

class Adafruit_MCP23X17;

class TaskButtonMCP23017 : private Task
{
public:
    TaskButtonMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t port);

    void start();

    void setPressedCallback(fastdelegate::FastDelegate2<bool, byte> callback) { callback_ = callback; }

    bool pressed();
    byte pin();

private:
    bool Callback() override;

private:
    uint8_t port_;
    Bounce2::ButtonMCP23017 button_;
    fastdelegate::FastDelegate2<bool, byte> callback_;
};