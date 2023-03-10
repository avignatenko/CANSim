#include "BasicInstrument.h"
#include <EEPROM.h>

const int kOffsetVars = 0;
const int kMaxVars = 20;



BasicInstrument::BasicInstrument(Pin& ledPin, Pin& buttonPin, byte canSPIPin, byte canIntPin)
    : CommonInstrument(ledPin, buttonPin, canSPIPin, canIntPin), taskMenu_(taskManager_)
{
    taskMenu_.cmdLine().SetDefaultHandler(&BasicInstrument::cmdErrorCallback, this);

    // fixme: move to member
    static SerialCommand s_cmdHelp("help", &BasicInstrument::cmdHelpCallback, false, this);
    static SerialCommand s_cmdAddr("var", &BasicInstrument::cmdVarCallback, false, this);
    static SerialCommand s_cmdLut("lut", &BasicInstrument::cmdLutCallback, false, this);
    static SerialCommand s_cmdPos("pos", &BasicInstrument::cmdPosCallback, false, this);
    static SerialCommand s_cmdLPos("lpos", &BasicInstrument::cmdLPosCallback, false, this);
    static SerialCommand s_cmdAct("act", &BasicInstrument::cmdActCallback, false, this);
    static SerialCommand s_cmdPosCCW("q", &BasicInstrument::cmdCCWCallback, true, this);
    static SerialCommand s_cmdPosCCWFast("Q", &BasicInstrument::cmdCCWFastCallback, true, this);
    static SerialCommand s_cmdPosCW("w", &BasicInstrument::cmdCWCallback, true, this);
    static SerialCommand s_cmdPosCWFast("W", &BasicInstrument::cmdCWFastCallback, true, this);

    taskMenu_.cmdLine().AddCommand(&s_cmdHelp);
    taskMenu_.cmdLine().AddCommand(&s_cmdAddr);
    taskMenu_.cmdLine().AddCommand(&s_cmdLut);

    taskMenu_.cmdLine().AddCommand(&s_cmdPos);
    taskMenu_.cmdLine().AddCommand(&s_cmdLPos);
    taskMenu_.cmdLine().AddCommand(&s_cmdAct);
    taskMenu_.cmdLine().AddCommand(&s_cmdPosCCW);
    taskMenu_.cmdLine().AddCommand(&s_cmdPosCCWFast);
    taskMenu_.cmdLine().AddCommand(&s_cmdPosCW);
    taskMenu_.cmdLine().AddCommand(&s_cmdPosCWFast);

    varAddrIdx_ = addVar("addr");
    taskCAN_.setSimAddress(getVar(varAddrIdx_));

    vars_.reserve(kMaxVars);
}

void BasicInstrument::setup()
{
    CommonInstrument::setup();

    // load luts
    for (byte i = 0; i < (byte)luts_.size(); ++i)
    {
        getLUT(i).load(lutOffset(i));
    }

    // start services
    taskMenu_.start();
}

void BasicInstrument::cmdVarCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->varCallback(sender);
}

void BasicInstrument::varCallback(SerialCommands* sender)
{
    auto& s = *sender->GetSerial();

    const char* nameStr = sender->Next();
    if (!nameStr)
    {
        // list of variables and their values
        for (int i = 0; i < (int)vars_.size(); ++i)
        {
            s.print(vars_[i].name);
            s.print(' ');
            s.println(getVar(i));
        }
        return;
    }

    // find this variable
    int idx = -1;
    for (int i = 0; i < (int)vars_.size(); ++i)
    {
        if (strcmp(vars_[i].name, nameStr) == 0)
        {
            idx = i;
            break;
        }
    }
    if (idx < 0)
    {
        sender->GetSerial()->println(F("Error: no such variable"));
        return;
    }

    const char* valueStr = sender->Next();

    if (!valueStr)
    {
        s.println(getVar(idx));
        return;
    }

    float value = atof(valueStr);
    setVar(idx, value);
    s.println(getVar(idx));
}

void BasicInstrument::lutCallback(SerialCommands* sender)
{
    auto& s = *sender->GetSerial();

    const char* nameStr = sender->Next();
    if (!nameStr)
    {
        // list all luts with their values
        for (byte i = 0; i < (byte)luts_.size(); ++i)
        {
            s.println();
            s.print(luts_[i].name);
            s.print(F(" max: "));
            s.println(luts_[i].lut.maxSize());
            s.println(luts_[i].lut);
        }
        return;
    }
    // find this lut
    int idx = -1;
    for (int i = 0; i < (int)luts_.size(); ++i)
    {
        if (strcmp(luts_[i].name, nameStr) == 0)
        {
            idx = i;
            break;
        }
    }
    if (idx < 0)
    {
        s.println(F("Error: no such lut"));
        return;
    }

    LutAction action = lutAction(sender);

    if (action.cmd == LUTCommand::Invalid)
    {
        s.println(F("Error: unknown command"));
        return;
    }

    onLutAction(idx, action);
}

int BasicInstrument::lutOffset(int idx)
{
    int offset = kOffsetVars + sizeof(float) * kMaxVars;
    for (int i = 1; i <= idx; ++i)
    {
        offset += getLUT(i - 1).maxSize() * sizeof(float) * 2;
    }
    return offset;
}

void BasicInstrument::onLutAction(byte lutIdx, LutAction action)
{
    StoredLUT& lut = getLUT(lutIdx);
    if (action.cmd == BasicInstrument::LUTCommand::Show)
    {
        Serial.println(lut);
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Load)
    {
        lut.load(lutOffset(lutIdx));
        Serial.println(lut);
        Serial.println("OK");
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Save)
    {
        lut.save(lutOffset(lutIdx));
        Serial.println(F("OK"));
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Clear)
    {
        lut.clear();
        Serial.println(F("OK"));
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Set || action.cmd == BasicInstrument::LUTCommand::SetX)
    {
        if (action.pos < 0) action.pos = posForLut(lutIdx);

        if (lut.size() == lut.maxSize())
        {
            Serial.println(F("Error: max LUT capacity reached"));
            return;
        }

        if (action.cmd == BasicInstrument::LUTCommand::Set)
            lut.addValue(action.posl, action.pos);
        else
            lut.addValue(action.pos, action.posl);

        Serial.print(F("Set "));
        Serial.print(action.posl);
        Serial.print(F("->"));
        Serial.print(action.pos);
        Serial.println(F(" OK"));

        Serial.println(lut);
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Remove)
    {
        lut.removeValue(action.posl);

        Serial.print(F("Remove "));
        Serial.print(action.posl);
        Serial.println(F(" OK"));

        Serial.println(lut);
        return;
    }
}
BasicInstrument::LutAction BasicInstrument::lutAction(SerialCommands* sender)
{
    const char* commandStr = sender->Next();

    LutAction action;

    if (!commandStr || strcmp(commandStr, "show") == 0) action.cmd = LUTCommand::Show;
    if (strcmp(commandStr, "save") == 0) action.cmd = LUTCommand::Save;
    if (strcmp(commandStr, "load") == 0) action.cmd = LUTCommand::Load;
    if (strcmp(commandStr, "clear") == 0) action.cmd = LUTCommand::Clear;
    if (strcmp(commandStr, "set") == 0 || strcmp(commandStr, "setx") == 0)
    {
        action.cmd = (strcmp(commandStr, "set") == 0) ? LUTCommand::Set : LUTCommand::SetX;
        const char* poslStr = sender->Next();
        if (!poslStr)
        {
            sender->GetSerial()->println(F("Error: set needs logical position"));
            return action;
        }

        action.posl = atof(poslStr);

        const char* posStr = sender->Next();
        if (posStr) action.pos = atoi(posStr);
    }
    if (strcmp(commandStr, "rm") == 0)
    {
        action.cmd = LUTCommand::Remove;
        const char* poslStr = sender->Next();
        if (!poslStr)
        {
            sender->GetSerial()->println(F("Error: rm needs logical position"));
            return action;
        }

        action.posl = atof(poslStr);
    }

    return action;
}

void BasicInstrument::cmdLutCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->lutCallback(sender);
}

void BasicInstrument::helpCallback(SerialCommands* sender)
{
    static const auto* a = F(R"=====( 
Help: 
 help - this help text
 var [name] [value] get/set device variable
 lut [name] [command] [value1] [value2] update lut(s)
 act [name] activate motor to use with pos/lpos
 pos [value] get/set motor position
 lpos [name_lut] get/set logical motor position
 q,Q,w,W - (single key commands) move motor needle (q,w - slow, Q,W - fast)
)=====");

    Stream* s = sender->GetSerial();
    s->println(a);

    s->println(F("Variables:"));
    for (int i = 0; i < (int)vars_.size(); ++i)
    {
        s->print(' ');
        s->println(vars_[i].name);
    }

    s->println(F("LUTs:"));
    for (int i = 0; i < (int)luts_.size(); ++i)
    {
        s->print(' ');
        s->println(luts_[i].name);
    }

    s->println(F("POSs:"));
    for (int i = 0; i < (int)poss_.size(); ++i)
    {
        s->print(' ');
        s->println(poss_[i]);
    }
}

void BasicInstrument::cmdHelpCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->helpCallback(sender);
}

// Callback in case of an error
void BasicInstrument::errorCallback(SerialCommands* sender, const char* command)
{
    sender->GetSerial()->print(F("Unrecognized command ["));
    sender->GetSerial()->print(command);
    sender->GetSerial()->println(F("]"));
}

void BasicInstrument::cmdErrorCallback(SerialCommands* sender, const char* command, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->errorCallback(sender, command);
}

void BasicInstrument::posCallback(SerialCommands* sender)
{
    const char* commandStr = sender->Next();
    if (commandStr)
    {
        int32_t pos = atoi(commandStr);

        setPos(activePos_, pos);
    }
    else
    {
        int32_t posValue = pos(activePos_);
        sender->GetSerial()->println(posValue);
    }
}

void BasicInstrument::cmdPosCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->posCallback(sender);
}

void BasicInstrument::lPosCallback(SerialCommands* sender)
{
    const char* commandStr = sender->Next();
    float pos = atof(commandStr);

    setLPos(activePos_, pos);
}

void BasicInstrument::cmdLPosCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->lPosCallback(sender);
}

void BasicInstrument::actCallback(SerialCommands* sender)
{
    const char* commandStr = sender->Next();
    if (commandStr)
    {
        for (int i = 0; i < (int)poss_.size(); ++i)
        {
            if (strcmp(commandStr, poss_[i]) == 0)
            {
                activePos_ = i;
                return;
            }
        }

        sender->GetSerial()->println(F("Pos not found"));
    }
    else
    {
        sender->GetSerial()->println(activePos_);
    }
}

void BasicInstrument::cmdActCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->actCallback(sender);
}

void BasicInstrument::cCWCallback(SerialCommands* sender)
{
    setPos(activePos_, -1, false);
}
void BasicInstrument::cmdCCWCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->cCWCallback(sender);
}

void BasicInstrument::cCWFastCallback(SerialCommands* sender)
{
    setPos(activePos_, -10, false);
}
void BasicInstrument::cmdCCWFastCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->cCWFastCallback(sender);
}

void BasicInstrument::cWCallback(SerialCommands* sender)
{
    setPos(activePos_, 1, false);
}
void BasicInstrument::cmdCWCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->cWCallback(sender);
}

void BasicInstrument::cWFastCallback(SerialCommands* sender)
{
    setPos(activePos_, 10, false);
}
void BasicInstrument::cmdCWFastCallback(SerialCommands* sender, void* data)
{
    auto* me = reinterpret_cast<BasicInstrument*>(data);
    me->cWFastCallback(sender);
}

byte BasicInstrument::addVar(const char* name)
{
    vars_.resize(vars_.size() + 1);
    vars_.back().name = name;
    return vars_.size() - 1;
}

float BasicInstrument::getVar(byte idx)
{
    float var;
    EEPROM.get(kOffsetVars + idx * sizeof(float), var);
    return var;
}

void BasicInstrument::setVar(byte idx, float value)
{
    EEPROM.put(kOffsetVars + idx * sizeof(float), value);
    if (idx == varAddrIdx_)  // addr
    {
        taskCAN_.setSimAddress((uint16_t)value);
    }
}

byte BasicInstrument::addLUT(const char* name, byte maxSize)
{
    luts_.resize(luts_.size() + 1);
    luts_.back().name = name;
    luts_.back().lut.setMaxSize(maxSize);
    return luts_.size() - 1;
}

StoredLUT& BasicInstrument::getLUT(byte idx)
{
    return luts_[idx].lut;
}

byte BasicInstrument::addPos(const char* name)
{
    poss_.resize(poss_.size() + 1);
    poss_.back() = name;
    return poss_.size() - 1;
}