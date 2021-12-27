#pragma once

#include "Common.h"

#include "FastDelegate.h"

namespace Bounce2
{
class Button;
}

class TaskButton : private Task
{
public:
    TaskButton(Scheduler& sh, byte btnPort);

    void start();

    void setPressedCallback(fastdelegate::FastDelegate1<bool> callback) { callback_ = callback; }

private:
    bool Callback() override;

private:
    Bounce2::Button* button_;
    fastdelegate::FastDelegate1<bool> callback_;
};