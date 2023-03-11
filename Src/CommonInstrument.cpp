#include "CommonInstrument.h"

#include "Pin.h"

void InstrumentBase::run()
{
    taskManager_.execute();
}

CommonInstrument::CommonInstrument(Pin& ledPin, Pin& buttonPin, byte canSPIPin, byte canIntPin)
    : taskErrorLed_(taskManager_, ledPin),
      taskButton_(taskManager_, buttonPin),
      taskCAN_(taskErrorLed_, taskManager_, canSPIPin, canIntPin, 0)
{
    taskButton_.setPressedCallback(fastdelegate::MakeDelegate(this, &CommonInstrument::onButtonPressed));
    taskCAN_.setReceiveCallback(fastdelegate::MakeDelegate(this, &CommonInstrument::onCANReceived));
}

void CommonInstrument::onButtonPressed(bool pressed, Pin& port)
{
    if (pressed)
        taskErrorLed_.addError(TaskErrorLed::ERROR_TEST_LED);
    else
        taskErrorLed_.removeError(TaskErrorLed::ERROR_TEST_LED);
}

void CommonInstrument::setup()
{
    // start services
    taskErrorLed_.start();
    taskButton_.start();
    taskCAN_.start();
}