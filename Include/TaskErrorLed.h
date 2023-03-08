#pragma once

#include "Common.h"
#include "Pin.h"

class TaskErrorLed : private Task
{
public:
    TaskErrorLed(Scheduler& sh, Pin& ledPort);

    enum Error
    {
        ERROR_OK = 0,
        ERROR_TEST_LED = (1 << 0),
        ERROR_CAN = (1 << 1),
        ERROR_HOST = (1 << 2)
    };

    void start();
    void addError(int error);
    void removeError(int error);
    void removeAllErrors();
    int error();

protected:
    virtual bool Callback() override;

private:
    void updateDelay();

    void led(bool on);

private:
    Pin& ledPort_;
    byte error_ = 0;
    bool ledOn_ = false;
};