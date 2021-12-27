#include "TaskButton.h"

#include <Bounce2.h>

bool TaskButton::Callback()
{
    if (!callback_) return;

    button_->update();

    if (button_->pressed())
    {
        callback_(true);
    }
    else if (button_->released())
    {
        callback_(false);
    }

    return true;
}

TaskButton::TaskButton(Scheduler& sh, byte ledPort)
    : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false), button_(new Bounce2::Button())
{
    button_->attach(ledPort, INPUT_PULLUP);
    button_->interval(20);
    button_->setPressedState(LOW);
}

void TaskButton::start()
{
    enable();
}
