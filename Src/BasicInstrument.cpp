#include "BasicInstrument.h"
#include <EEPROM.h>

const int kOffsetVars = 0;
const int kMaxVars = 20;

BasicInstrument::BasicInstrument(byte ledPin, byte buttonPin, byte canSPIPin, byte canIntPin)
    : taskErrorLed_(taskManager_, ledPin),
      taskButton_(taskManager_, buttonPin),
      taskCAN_(taskErrorLed_, taskManager_, canSPIPin, canIntPin, 0),
      taskMenu_(taskManager_)
{
    taskButton_.setPressedCallback(fastdelegate::MakeDelegate(this, &BasicInstrument::onButtonPressed));

    static SerialCommand s_cmdHelp("help", &BasicInstrument::cmdHelpCallback, false, this);
    static SerialCommand s_cmdAddr("var", &BasicInstrument::cmdVarCallback, false, this);
    static SerialCommand s_cmdLut("lut", &BasicInstrument::cmdLutCallback, false, this);

    taskMenu_.cmdLine().AddCommand(&s_cmdHelp);
    taskMenu_.cmdLine().AddCommand(&s_cmdAddr);
    taskMenu_.cmdLine().AddCommand(&s_cmdLut);

    taskMenu_.cmdLine().SetDefaultHandler(&BasicInstrument::cmdErrorCallback, this);

    varAddrIdx_ = addVar("addr");
    taskCAN_.setSimAddress(getVar(varAddrIdx_));

    vars_.reserve(kMaxVars);
}

void BasicInstrument::setup()
{
    // load luts
    for (byte i = 0; i < (byte)luts_.size(); ++i)
    {
        getLUT(i).load(lutOffset(i));
    }

    // start services
    taskErrorLed_.start();
    taskButton_.start();
    taskCAN_.start();
    taskMenu_.start();
}

void BasicInstrument::run()
{
    taskManager_.execute();
}

void BasicInstrument::onButtonPressed(bool pressed)
{
    if (pressed)
        taskErrorLed_.addError(TaskErrorLed::ERROR_TEST_LED);
    else
        taskErrorLed_.removeError(TaskErrorLed::ERROR_TEST_LED);
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
        sender->GetSerial()->println(F("Error: no such lut"));
        return;
    }

    LutAction action = lutAction(sender);

    if (action.cmd == LUTCommand::Invalid)
    {
        sender->GetSerial()->println("Error: unknown command");
        return;
    }

    onLutAction(idx, action);
}

int BasicInstrument::lutOffset(int idx)
{
    StoredLUT& lut = getLUT(idx);
    int offset = kOffsetVars + sizeof(float) * kMaxVars;
    for (int i = 0; i < idx; ++i)
    {
        offset += getLUT(i).maxSize();
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
        Serial.println("OK");
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Clear)
    {
        lut.clear();
        Serial.println("OK");
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Set || action.cmd == BasicInstrument::LUTCommand::SetX)
    {
        if (action.pos < 0) action.pos = posForLut(lutIdx);

        if (lut.size() == lut.maxSize())
        {
            Serial.println("Error: max LUT capacity reached");
            return;
        }

        if (action.cmd == BasicInstrument::LUTCommand::Set)
            lut.addValue(action.posl, action.pos);
        else
            lut.addValue(action.pos, action.posl);

        Serial.print("Set ");
        Serial.print(action.posl);
        Serial.print("->");
        Serial.print(action.pos);
        Serial.println(" OK");

        Serial.println(lut);
        return;
    }

    if (action.cmd == BasicInstrument::LUTCommand::Remove)
    {
        lut.removeValue(action.posl);

        Serial.print("Remove ");
        Serial.print(action.posl);
        Serial.println(" OK");

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
            return;
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
            return;
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
)=====");

    Stream* s = sender->GetSerial();
    s->println(a);

    s->println(F("Variables:"));
    for (int i = 0; i < (int)vars_.size(); ++i)
    {
        s->print(' ');
        s->println(vars_[i].name);
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

byte BasicInstrument::addVar(const char* name)
{
    vars_.push_back(Var());
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
    onVarSet(idx, value);
}

void BasicInstrument::onVarSet(int idx, float value)
{
    if (idx == varAddrIdx_)  // addr
    {
        taskCAN_.setSimAddress((uint16_t)value);
    }
}

byte BasicInstrument::addLUT(const char* name, byte maxSize)
{
    Lut lut(name, maxSize);
    luts_.push_back(lut);
}

StoredLUT& BasicInstrument::getLUT(byte idx)
{
    return luts_[idx].lut;
}