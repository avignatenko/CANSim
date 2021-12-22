#pragma once

#include "Common.h"

#include <Array.h>
#include <SerialCommands.h>

class TaskMenu : private Task
{
public:
    TaskMenu(Scheduler& sh);
    void start();

    SerialCommands& cmdLine() { return cmdLine_; }

private:
    virtual bool Callback() override;

private:
    char serial_command_buffer_[32];
    SerialCommands cmdLine_;
};