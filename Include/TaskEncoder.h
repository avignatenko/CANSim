#pragma once

#include "Common.h"

#include "FastDelegate.h"

class RotaryEncoder;

class TaskEncoder : private Task
{
public:
    TaskEncoder(Scheduler& sh, byte pinA, byte pinB);

    void start();

    // -1 - CCW, 1 - CW
    void setRotationCallback(fastdelegate::FastDelegate2<int8_t, long> callback) { callback_ = callback; }

private:
    bool Callback() override;

private:
    RotaryEncoder* encoder_;
    fastdelegate::FastDelegate2<int8_t, long> callback_;
};