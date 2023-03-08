#pragma once

#include "Common.h"

#include "Bounce3.h"

#include "Pin.h"
#include "FastDelegate.h"

class TaskButton : private Task
{
public:
    TaskButton(Scheduler& sh, Pin& btnPort);

    void start();

    void setPressedCallback(fastdelegate::FastDelegate2<bool,  Pin& > callback) { callback_ = callback; }

    bool pressed();
    Pin& pin();

private:
    bool Callback() override;

private:
    Bounce2::Button3 button_;
    fastdelegate::FastDelegate2<bool,  Pin& > callback_;
};