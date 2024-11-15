#pragma once

#include "Common.h"

#include "CommonInstrument.h"
#include "Pin.h"
#include "StoredLUT.h"

#include "TaskMenu.h"

#include "FastDelegate.h"



class BasicInstrument : public CommonInstrument
{
public:
    BasicInstrument(Pin& ledPin, Pin& buttonPin, Pin& canSPIPin, Pin& canIntPin);
    virtual void setup();

protected:
    byte addVar(const char* name);
    float getVar(byte idx);
    virtual void setVar(byte idx, float value);

    byte addLUT(const char* name, byte maxSize);
    StoredLUT& getLUT(byte idx);
    virtual int32_t posForLut(byte idxLut) { return -1; }

    byte addPos(const char* name);
    virtual void setPos(byte idx, int32_t value, bool absolute = true) {}
    virtual int32_t pos(byte idxPos) { return 0; }
    virtual void setLPos(byte idx, float value, bool absolute = true) {}

protected:
    byte varAddrIdx_ = 0;

    TaskMenu taskMenu_;

private:
    void helpCallback(SerialCommands* sender);
    static void cmdHelpCallback(SerialCommands* sender, void* data);

    void errorCallback(SerialCommands* sender, const char* command);
    static void cmdErrorCallback(SerialCommands* sender, const char* command, void* data);

    void varCallback(SerialCommands* sender);
    static void cmdVarCallback(SerialCommands* sender, void* data);

    void lutCallback(SerialCommands* sender);
    static void cmdLutCallback(SerialCommands* sender, void* data);

    void posCallback(SerialCommands* sender);
    static void cmdPosCallback(SerialCommands* sender, void* data);

    void lPosCallback(SerialCommands* sender);
    static void cmdLPosCallback(SerialCommands* sender, void* data);

    void actCallback(SerialCommands* sender);
    static void cmdActCallback(SerialCommands* sender, void* data);

    void cCWCallback(SerialCommands* sender);
    static void cmdCCWCallback(SerialCommands* sender, void* data);

    void cCWFastCallback(SerialCommands* sender);
    static void cmdCCWFastCallback(SerialCommands* sender, void* data);

    void cWCallback(SerialCommands* sender);
    static void cmdCWCallback(SerialCommands* sender, void* data);

    void cWFastCallback(SerialCommands* sender);
    static void cmdCWFastCallback(SerialCommands* sender, void* data);

    enum class LUTCommand : byte
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
        int32_t pos = -1;
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
        const char* name = "";
        StoredLUT lut;
    };

    std::vector<Lut> luts_;
    std::vector<const char*> poss_;
    byte activePos_ = 0;
};