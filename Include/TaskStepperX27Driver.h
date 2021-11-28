#pragma once

#include "Common.h"

class SwitecX12;

class TaskStepperX27Driver
{
public:
    static void init(Scheduler& sh, byte step, byte dir, byte reset);
    static TaskStepperX27Driver& instance();

    void start();

    void setPosition(uint16_t position);
    uint16_t position() const;
    uint16_t totalSteps() const;

private:
    TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset);

    void loopStepperCallback();
    static void loopStepperCallbackStatic();

private:
    static TaskStepperX27Driver* instance_;

    Task task_;

    SwitecX12* motor_;
    byte resetPin_;
};