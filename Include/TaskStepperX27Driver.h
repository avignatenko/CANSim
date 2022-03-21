#pragma once

#include "Common.h"

class AccelStepper;

class TaskStepperX27Driver: private Task
{
public:
    TaskStepperX27Driver(Scheduler& sh, byte step, byte dir, byte reset, bool limit = true);

    void start();

    void stop();
    void resetPosition(int16_t position);
    void setPosition(int16_t position);
    int16_t position() const;
    int16_t targetPosition() const;
    uint16_t totalSteps() const;

    void setMaxSpeed(int maxSpeed);
    void setMaxAcceleration(int maxAcceleration);

private:
    bool Callback() override;

private:
    AccelStepper* motor_;
    bool limit_;
    int maxSpeed_;
    int maxAcceleration_;
};