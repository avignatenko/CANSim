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

void TaskErrorLedBase::ledOwn(bool on)
{
    led(on);
    ledOn_ = on;
}

bool TaskErrorLedBase::Callback()
{
    // blink
    ledOwn(!ledOn_);

    return true;
}

TaskErrorLedBase::TaskErrorLedBase(Scheduler& sh) : Task(TASK_IMMEDIATE, TASK_FOREVER, &sh, false) {}

void TaskErrorLedBase::start()
{
    initLed();

    // test 1sec on
    ledOwn(true);
    ::delay(1000);
    ledOwn(false);
}

void TaskErrorLedBase::addError(int error)
{
    int newError = error_ | error;
    if (error_ == newError) return;
    error_ = newError;
    updateDelay();
}

void TaskErrorLedBase::removeError(int error)
{
    int newError = error_ & ~error;
    if (error_ == newError) return;
    error_ = newError;
    updateDelay();
}

void TaskErrorLedBase::removeAllErrors()
{
    if (error_ == 0) return;
    error_ = 0;
    updateDelay();
}

int TaskErrorLedBase::error()
{
    return error_;
}

void TaskErrorLedBase::updateDelay()
{
    if (error_ == ERROR_OK)
    {
        ledOwn(false);
        disable();
        return;
    }

    int highestBit = getHighestBit(error_);
    if (highestBit == 0)  // on forever
    {
        ledOwn(true);
        disable();
        return;
    }

    int delayMs = 1000 / highestBit;

    enableIfNot();
    setInterval(delayMs * TASK_MILLISECOND);
}

///////////////

TaskErrorLed::TaskErrorLed(Scheduler& sh, uint8_t pin) : TaskErrorLedBase(sh), pin_(pin) {}

void TaskErrorLed::initLed()
{
    pinMode(pin_, OUTPUT);
}

void TaskErrorLed::led(bool on)
{
    digitalWrite(pin_, on ? HIGH : LOW);
}

///////////////

TaskErrorLedMCP23017::TaskErrorLedMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t pin)
    : TaskErrorLedBase(sh), mcp_(mcp), pin_(pin)
{
}

void TaskErrorLedMCP23017::initLed()
{
    mcp_.pinMode(pin_, OUTPUT);
}

void TaskErrorLedMCP23017::led(bool on)
{
    mcp_.digitalWrite(pin_, on ? HIGH : LOW);
}
