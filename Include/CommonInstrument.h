#pragma once

#include "TaskButton.h"
#include "TaskCAN.h"
#include "TaskErrorLed.h"

class Pin;

class InstrumentBase
{
public:
    void run();

protected:
    Scheduler taskManager_;
};

class CommonInstrument : public InstrumentBase
{
public:
    CommonInstrument(Pin& ledPin, Pin& buttonPin, byte canSPIPin, byte canIntPin);
    virtual void setup();

protected:
    virtual void onButtonPressed(bool pressed,  Pin&  port);
    virtual void onCANReceived(byte priority, byte port, uint16_t srcAddress, uint16_t dstAddress, byte len,
                               byte* payload)
    {
    }

protected:
    TaskErrorLed taskErrorLed_;
    TaskButton taskButton_;
    TaskCAN taskCAN_;
};