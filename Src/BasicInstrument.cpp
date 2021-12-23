#include "BasicInstrument.h"
#include <EEPROM.h>

BasicInstrument::BasicInstrument(byte ledPin, byte buttonPin, byte canSPIPin, byte canIntPin)
    : taskErrorLed_(taskManager_, ledPin),
      taskButton_(taskManager_, buttonPin),
      taskCAN_(taskErrorLed_, taskManager_, canSPIPin, canIntPin, 0),
      taskMenu_(taskManager_)
{
    taskButton_.setPressedCallback(fastdelegate::MakeDelegate(this, &BasicInstrument::onButtonPressed));

    static SerialCommand s_cmdHelp("help", &BasicInstrument::cmdHelpCallback, false, this);
    static SerialCommand s_cmdAddr("var", &BasicInstrument::cmdVarCallback, false, this);

    taskMenu_.cmdLine().AddCommand(&s_cmdHelp);
    taskMenu_.cmdLine().AddCommand(&s_cmdAddr);

    taskMenu_.cmdLine().SetDefaultHandler(&BasicInstrument::cmdErrorCallback, this);

    varAddrIdx_ = addVar("addr");
    taskCAN_.setSimAddress(getVar(varAddrIdx_));

    vars_.reserve(20);
}

void BasicInstrument::setup()
{
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

void BasicInstrument::helpCallback(SerialCommands* sender)
{
    static const auto* a = F(R"=====( 
Help: 
 help - this help text
 var <name> [value] get/set device variable
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
    EEPROM.get(idx * sizeof(float), var);
    return var;
}

void BasicInstrument::setVar(byte idx, float value)
{
    EEPROM.put(idx * sizeof(float), value);
}

void BasicInstrument::onVarSet(int idx, float value)
{
    if (idx == varAddrIdx_)  // addr
    {
        taskCAN_.setSimAddress((uint16_t)value);
    }
}
