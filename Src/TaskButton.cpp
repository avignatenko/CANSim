#include "TaskButton.h"

bool TaskButtonBase::Callback()
{
    if (!callback_) return;

 
    if (buttonChanged()) callback_(buttonPressed(), pin());

    return true;
}

TaskButtonBase::TaskButtonBase(Scheduler& sh)
    : Task(5 * TASK_MILLISECOND, TASK_FOREVER, &sh, false)
{
}

void TaskButtonBase::start()
{
    initButton();

    enable();
}

////////

TaskButton::TaskButton(Scheduler& sh, uint8_t btnPort): TaskButtonBase(sh), port_(btnPort)
{

}

bool TaskButton::buttonPressed()
{
    return button_.pressed();
}

bool TaskButton::buttonChanged()
{
    return button_.changed();
}

void TaskButton::updateButton()
{
   button_.update();
}

void TaskButton::initButton()
{
    button_.attach(port_, INPUT_PULLUP);
    button_.interval(20);
    button_.setPressedState(LOW);
}

bool TaskButton::pressed()
{
    return button_.isPressed();
}

byte TaskButton::pin()
{
    return button_.getPin();
}

////////

TaskButtonMCP23017::TaskButtonMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t btnPort): TaskButtonBase(sh), button_(mcp), port_(btnPort)
{
    
}

bool TaskButtonMCP23017::buttonPressed()
{
    return button_.pressed();
}

bool TaskButtonMCP23017::buttonChanged()
{
    return button_.changed();
}

void TaskButtonMCP23017::updateButton()
{
   button_.update();
}

void TaskButtonMCP23017::initButton()
{
    button_.attach(port_, INPUT_PULLUP);
    button_.interval(20);
    button_.setPressedState(LOW);
}

bool TaskButtonMCP23017::pressed()
{
    return button_.isPressed();
}

byte TaskButtonMCP23017::pin()
{
    return button_.getPin();
}