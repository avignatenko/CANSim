#include "TaskButtonMCP23017.h"

bool TaskButtonMCP23017::Callback()
{
    if (!callback_) return;

    button_.update();

    if (button_.changed()) callback_(button_.pressed(), button_.getPin());

    return true;
}

TaskButtonMCP23017::TaskButtonMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t port)
    : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false), button_(mcp), port_(port)
{
}

void TaskButtonMCP23017::start()
{
    button_.attach(port_, INPUT_PULLUP);
    button_.interval(20);
    button_.setPressedState(LOW);

    enable();
}

bool TaskButtonMCP23017::pressed()
{
    return button_.isPressed();
}

byte TaskButtonMCP23017::pin()
{
    return button_.getPin();
}