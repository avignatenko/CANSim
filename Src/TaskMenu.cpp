#include "TaskMenu.h"

#include <SerialCommands.h>

bool TaskMenu::Callback()
{
    int received = Serial.peek();
    if (received >= 0) Serial.print((char)received);

    cmdLine_->ReadSerial();
}

TaskMenu* TaskMenu::instance_ = nullptr;

void TaskMenu::cmdVarCallback(SerialCommands* sender)
{
    const char* nameStr = sender->Next();
    if (!nameStr)
    {
        sender->GetSerial()->println(F("Error: no variable name"));
        return;
    }

    // find this variable
    TaskMenu& me = TaskMenu::instance();
    int idx = -1;
    for (int i = 0; i < (int)me.vars_.size(); ++i)
    {
        if (strcmp(me.vars_[i].name, nameStr) == 0)
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
        if (me.vars_[idx].getter)
        {
            float value = me.vars_[idx].getter();
            sender->GetSerial()->println(value);
            return;
        }
    }

    float value = atof(valueStr);
    if (me.vars_[idx].setter)
    {
        me.vars_[idx].setter(value);

        float value = me.vars_[idx].getter();
        sender->GetSerial()->println(value);
    }
}

void TaskMenu::helpCallback(SerialCommands* sender)
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

void TaskMenu::cmdHelpCallback(SerialCommands* sender)
{
    TaskMenu::instance().helpCallback(sender);
}

// Callback in case of an error
void TaskMenu::errorCallback(SerialCommands* sender, const char* command)
{
    sender->GetSerial()->print(F("Unrecognized command ["));
    sender->GetSerial()->print(command);
    sender->GetSerial()->println(F("]"));
}

TaskMenu::TaskMenu(Scheduler& sh) : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false)
{
    static char s_serial_command_buffer[32];
    cmdLine_ = new SerialCommands(&Serial, s_serial_command_buffer, sizeof(s_serial_command_buffer), "\r\n", " ");

    static SerialCommand s_cmdHelp("help", &TaskMenu::cmdHelpCallback);
    static SerialCommand s_cmdAddr("var", &TaskMenu::cmdVarCallback);

    cmdLine_->AddCommand(&s_cmdHelp);
    cmdLine_->AddCommand(&s_cmdAddr);

    cmdLine_->SetDefaultHandler(&TaskMenu::errorCallback);

    Serial.println(F("Welcome to gauge terminal! Type 'help' for list of commands."));
}

void TaskMenu::init(Scheduler& sh)
{
    instance_ = new TaskMenu(sh);
}

void TaskMenu::start()
{
    enable();
}

void TaskMenu::addVar(const char* name, VarGetCallback get, VarSetCallback set)
{
    Var v;
    v.name = name;
    v.getter = get;
    v.setter = set;

    vars_.push_back(v);
}

TaskMenu& TaskMenu::instance()
{
    return *instance_;
}
