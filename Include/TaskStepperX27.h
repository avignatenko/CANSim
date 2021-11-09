#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperX27
{
public:
    static void init(Scheduler& sh, byte p1, byte p2, byte p3, byte p4);
    static TaskStepperX27& instance();

    void start();

    void setPosition(uint16_t position);

private:
    TaskStepperX27(Scheduler& sh, byte p1, byte p2, byte p3, byte p4);

    void loopStepperCallback();
    static void loopStepperCallbackStatic();

private:
    static TaskStepperX27* instance_;

    Task task_;

    AccelStepper* motor_;
};