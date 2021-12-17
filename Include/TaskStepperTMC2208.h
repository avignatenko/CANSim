#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperTMC2208
{
public:
    static void init(Scheduler& sh, byte step, byte dir, byte reset, float speed, float acceleration);
    static TaskStepperTMC2208& instance();

    void start();

    void resetPosition(int32_t position);
    void setPosition(int32_t position);
    int32_t position() const;

private:
    TaskStepperTMC2208(Scheduler& sh, byte step, byte dir, byte reset, float speed, float acceleration);

    void loopStepperCallback();
    static void loopStepperCallbackStatic();

private:
    static TaskStepperTMC2208* instance_;

    Task task_;

    AccelStepper* motor_;
};