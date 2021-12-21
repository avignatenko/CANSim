#pragma once

#include "Common.h"

class SwitecX25;

class TaskStepperX27: private Task
{
public:
    TaskStepperX27(Scheduler& sh, byte p1, byte p2, byte p3, byte p4);

    void start();

    void setPosition(uint16_t position);
    uint16_t totalSteps() const;

private:
  
    bool Callback() override;
  
private:
    SwitecX25* motor_;
};