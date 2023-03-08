#include "TaskButton.h"


bool TaskButton::Callback()
{
    if (!callback_) return;

    button_.update();

    if (button_.changed()) callback_(button_.pressed(), button_.getPin());

    return true;
}

TaskButton::TaskButton(Scheduler& sh, Pin& ledPort)
    : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false), button_(ledPort)
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

Pin& TaskButton::pin()
{
    return button_.getPin();
}