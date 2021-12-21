#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperX27Driver: private Task
{
public:
    TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset);

    void start();

    void setPosition(int16_t position);
    int16_t position() const;
    uint16_t totalSteps() const;

private:
    bool Callback() override;

private:
    AccelStepper* motor_;
};