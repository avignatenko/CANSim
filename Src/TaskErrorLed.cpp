#include "TaskErrorLed.h"

namespace
{
int getHighestBit(int value)
{
    int r = 0;
    while (value >>= 1) r++;
    return r;
}
}  // namespace

void TaskErrorLed::led(bool on)
{
    digitalWrite(ledPort_, on ? HIGH : LOW);
    ledOn_ = on;
}

bool TaskErrorLed::Callback()
{
    // blink
    led(!ledOn_);

    return true;
}

TaskErrorLed::TaskErrorLed(Scheduler& sh, byte ledPort)
    : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false), ledPort_(ledPort)
{
    pinMode(ledPort_, OUTPUT);
}

void TaskErrorLed::start()
{
    // test 2sec on
    led(true);
    delay(2000);
    led(false);
}

void TaskErrorLed::addError(int error)
{
    int newError = error_ | error;
    if (error_ == newError) return;
    error_ = newError;
    updateDelay();
}

void TaskErrorLed::removeError(int error)
{
    int newError = error_ & ~error;
    if (error_ == newError) return;
    error_ = newError;
    updateDelay();
}

void TaskErrorLed::removeAllErrors()
{
    if (error_ == 0) return;
    error_ = 0;
    updateDelay();
}

int TaskErrorLed::error()
{
    return error_;
}

void TaskErrorLed::updateDelay()
{
    if (error_ == ERROR_OK)
    {
        led(false);
        disable();
        return;
    }

    int highestBit = getHighestBit(error_);
    if (highestBit == 0)  // on forever
    {
        led(true);
        disable();
        return;
    }

    int delayMs = 1000 / highestBit;

    enableIfNot();
    setInterval(delayMs * TASK_MILLISECOND);
}
