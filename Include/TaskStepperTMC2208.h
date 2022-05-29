#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperTMC2208 : private Task
{
public:
    TaskStepperTMC2208(Scheduler& sh, byte step, byte dir, byte reset, bool invertDir = false);

    void start();

    void stop();
    bool isRunning();
    void resetPosition(int32_t position);
    void setPosition(int32_t position);
    int32_t position() const;
    int32_t targetPosition() const;
    void setSpeed(float speed);
    float speed() const;

    void setAcceleration(float acceleration);

private:
    bool Callback() override;

private:
    AccelStepper* motor_;
};