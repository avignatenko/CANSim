#pragma once

#include "Common.h"

#include "StoredLUT.h"
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

    byte addLUT(const char* name, byte maxSize);
    StoredLUT& getLUT(byte idx);

    virtual int32_t posForLut(int idx) { return -1; }

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
    void lutCallback(SerialCommands* sender);

    static void cmdErrorCallback(SerialCommands* sender, const char* command, void* data);
    static void cmdVarCallback(SerialCommands* sender, void* data);
    static void cmdLutCallback(SerialCommands* sender, void* data);
    static void cmdHelpCallback(SerialCommands* sender, void* data);

    enum class LUTCommand
    {
        Invalid,
        Show,
        Load,
        Save,
        Clear,
        Set,
        SetX,
        Remove
    };

    struct LutAction
    {
        LUTCommand cmd = LUTCommand::Invalid;
        float posl = -1.0;
        int16_t pos = -1;
    };

    LutAction lutAction(SerialCommands* sender);
    void onLutAction(byte lutIdx, LutAction action);
    int lutOffset(int idx);

private:
    struct Var
    {
        const char* name = "";
    };

    std::vector<Var> vars_;

    struct Lut
    {
        Lut(const char* name0, byte maxSize) : name(name0), lut(maxSize) {}

        const char* name = "";
        StoredLUT lut;
    };

    std::vector<Lut> luts_;
};