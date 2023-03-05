#include "TaskButton.h"

bool TaskButton::Callback()
{
    if (!callback_) return;

    button_.update();

    if (button_.changed()) callback_(button_.pressed(), button_.getPin());

    return true;
}

TaskButton::TaskButton(Scheduler& sh, uint8_t port) : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false), port_(port) {}

void TaskButton::start()
{
    button_.attach(port_, INPUT_PULLUP);
    button_.interval(20);
    button_.setPressedState(LOW);

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