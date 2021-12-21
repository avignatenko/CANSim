#pragma once

#include "Common.h"

#include "TaskButton.h"
#include "TaskCAN.h"
#include "TaskErrorLed.h"

class BasicInstrument
{
public:
    BasicInstrument(byte ledPin, byte buttonPin, byte canSPIPin, byte canIntPin);
    virtual void setup();
    virtual void run();

protected:


protected:
    Scheduler taskManager_;
    TaskErrorLed taskErrorLed_;
    TaskButton taskButton_;
    TaskCAN taskCAN_;

private:
    void onButtonPressed(bool pressed);

private:
};