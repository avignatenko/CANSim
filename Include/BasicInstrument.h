#pragma once

#include "Common.h"

#include "TaskButton.h"
#include "TaskCAN.h"
#include "TaskErrorLed.h"
#include "TaskMenu.h"

#include "FastDelegate.h"

class BasicInstrument
{
public:
    BasicInstrument(byte ledPin, byte buttonPin, byte canSPIPin, byte canIntPin);
    virtual void setup();
    virtual void run();

protected:
    byte addVar(const char* name);
    float getVar(byte idx);
    void setVar(byte idx, float value);

    virtual void onVarSet(int idx, float value);

protected:
    byte varAddrIdx_ = 0;

    Scheduler taskManager_;
    TaskErrorLed taskErrorLed_;
    TaskButton taskButton_;
    TaskCAN taskCAN_;
    TaskMenu taskMenu_;

private:
    void onButtonPressed(bool pressed);

    void helpCallback(SerialCommands* sender);
    void errorCallback(SerialCommands* sender, const char* command);
    void varCallback(SerialCommands* sender);

    static void cmdErrorCallback(SerialCommands* sender, const char* command, void* data);
    static void cmdVarCallback(SerialCommands* sender, void* data);
    static void cmdHelpCallback(SerialCommands* sender, void* data);

private:
    struct Var
    {
        const char* name = "";
    };

    Array<Var, 20> vars_;
};