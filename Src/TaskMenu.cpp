#include "TaskMenu.h"

#include <SerialCommands.h>

void TaskMenu::loopMenuCallbackStatic()
{
    TaskMenu& me = TaskMenu::instance();
    me.loopMenuCallback();
}

void TaskMenu::loopMenuCallback()
{
    int received = Serial.peek();
    if (received >= 0) Serial.print((char)received);

    cmdLine_->ReadSerial();
}

TaskMenu* TaskMenu::instance_ = nullptr;

void TaskMenu::cmdSimAddressCallback(SerialCommands* sender)
{
    const char* addrStr = sender->Next();
    if (!addrStr) addrStr = "0";
    int addr = atoi(addrStr);

    uint16_t newAddr = 0;
    if (TaskMenu::instance().simAddressCallback_) newAddr = TaskMenu::instance().simAddressCallback_(addr);
    if (newAddr)
    {
        Serial.println();
        Serial.println(newAddr);
    }
}

void TaskMenu::helpCallback(SerialCommands* sender)
{
    static const auto* a = F(R"=====( 
Help: 
 help - this help text
 addr <address> - change sim address for this device
)=====");

    Stream* s = sender->GetSerial();
    s->println(a);
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

TaskMenu::TaskMenu(Scheduler& sh) : task_(TASK_IMMEDIATE, TASK_FOREVER, &loopMenuCallbackStatic, &sh, false)
{
    static char s_serial_command_buffer[32];
    cmdLine_ = new SerialCommands(&Serial, s_serial_command_buffer, sizeof(s_serial_command_buffer), "\r\n", " ");

    static SerialCommand s_cmdHelp("help", &TaskMenu::cmdHelpCallback);
    static SerialCommand s_cmdAddr("addr", &TaskMenu::cmdSimAddressCallback);

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
    task_.enable();
}

void TaskMenu::setSimAddressCallback(SimAddressCallback callback)
{
    simAddressCallback_ = callback;
}

TaskMenu& TaskMenu::instance()
{
    return *instance_;
}
