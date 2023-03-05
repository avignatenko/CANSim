#pragma once

#include "Common.h"

#include <Bounce2.h>

#include "FastDelegate.h"

class TaskButton : private Task
{
public:
    TaskButton(Scheduler& sh, uint8_t btnPort);

    void start();

    void setPressedCallback(fastdelegate::FastDelegate2<bool, byte> callback) { callback_ = callback; }

    bool pressed();
    byte pin();

private:
    bool Callback() override;

private:
    uint8_t port_;
    Bounce2::Button button_;
    fastdelegate::FastDelegate2<bool, byte> callback_;
};