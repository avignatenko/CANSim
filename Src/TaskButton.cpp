#include "TaskButton.h"


bool TaskButton::Callback()
{
    if (!callback_) return;

    button_.update();

    if (button_.pressed())
    {
        callback_(true, button_.getPin());
    }
    else if (button_.released())
    {
        callback_(false, button_.getPin());
    }

    return true;
}

TaskButton::TaskButton(Scheduler& sh, byte ledPort)
    : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false)
{
    button_.attach(ledPort, INPUT_PULLUP);
    button_.interval(20);
    button_.setPressedState(LOW);
}

void TaskButton::start()
{
    enable();
}

bool TaskButton::pressed()
{
    return button_.isPressed();
}

byte TaskButton::pin()
{
    return button_.getPin();
}