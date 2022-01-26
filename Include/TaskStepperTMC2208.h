#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperTMC2208 : private Task
{
public:
    TaskStepperTMC2208(Scheduler& sh, byte step, byte dir, byte reset);

    void start();

    void stop();
    void resetPosition(int32_t position);
    void setPosition(int32_t position);
    int32_t position() const;
    int32_t targetPosition() const;
    void setSpeed(float speed);
    void setAcceleration(float acceleration);

private:
    bool Callback() override;

private:
    AccelStepper* motor_;
};