#include "BasicInstrument.h"

BasicInstrument::BasicInstrument(byte ledPin, byte buttonPin, byte canSPIPin, byte canIntPin)
    : taskErrorLed_(taskManager_, ledPin),
      taskButton_(taskManager_, buttonPin),
      taskCAN_(taskErrorLed_, taskManager_, canSPIPin, canIntPin, 17)  // fixme
{
    taskButton_.setPressedCallback(fastdelegate::MakeDelegate(this, &BasicInstrument::onButtonPressed));
}

void BasicInstrument::setup()
{
    taskErrorLed_.start();
    taskButton_.start();
    taskCAN_.start();
}

void BasicInstrument::run()
{
    taskManager_.execute();
}

void BasicInstrument::onButtonPressed(bool pressed)
{
    if (pressed)
        taskErrorLed_.addError(TaskErrorLed::ERROR_TEST_LED);
    else
        taskErrorLed_.removeError(TaskErrorLed::ERROR_TEST_LED);
}