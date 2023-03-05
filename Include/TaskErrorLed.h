#pragma once

#include "Common.h"

#include <Adafruit_MCP23X17.h>

class TaskErrorLedBase : private Task
{
public:
    TaskErrorLedBase(Scheduler& sh);

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

    virtual void initLed() = 0;
    virtual void led(bool on) = 0;

private:
    void updateDelay();
    void ledOwn(bool on);

private:
    byte error_ = 0;
    bool ledOn_ = false;
};

class TaskErrorLed : public TaskErrorLedBase
{
public:
    TaskErrorLed(Scheduler& sh, uint8_t pin);

protected:
    virtual void initLed() override;
    virtual void led(bool on) override;

private:
    uint8_t pin_;
};

class TaskErrorLedMCP23017 : public TaskErrorLedBase
{
public:
    TaskErrorLedMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t pin);

protected:
    virtual void initLed() override;
    virtual void led(bool on) override;

private:
    Adafruit_MCP23X17& mcp_;
    uint8_t pin_;
};