#include "TaskMenu.h"

#include <SerialCommands.h>

TaskMenu::TaskMenu(Scheduler& sh)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false),
      cmdLine_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ")
{
}

bool TaskMenu::Callback()
{
    int received = Serial.peek();
    if (received >= 0) Serial.print((char)received);

    cmdLine_.ReadSerial();

    return true;
}

void TaskMenu::start()
{
    Serial.println(F("Welcome to gauge terminal! Type 'help' for list of commands."));
    enable();
}
