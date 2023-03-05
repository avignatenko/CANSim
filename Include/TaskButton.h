#pragma once

#include "Common.h"

#include "Bounce2MCP23017.h"

#include <Bounce2.h>
#include "FastDelegate.h"

class TaskButtonBase : private Task
{
public:
    TaskButtonBase(Scheduler& sh);

    void start();

    void setPressedCallback(fastdelegate::FastDelegate2<bool, byte> callback) { callback_ = callback; }

    virtual bool pressed() = 0;
    virtual byte pin() = 0;

protected:
    virtual void initButton() = 0;
    virtual void updateButton() = 0;
    virtual bool buttonChanged() = 0;
    virtual bool buttonPressed() = 0;
private:
    bool Callback() override;

private:
    fastdelegate::FastDelegate2<bool, byte> callback_;
};

class TaskButton : public TaskButtonBase
{
public:
    TaskButton(Scheduler& sh, uint8_t btnPort);

    virtual bool pressed() override;
    virtual byte pin() override;

protected:
    virtual void initButton() override;
    virtual void updateButton() override;
    virtual bool buttonChanged() override;
    virtual bool buttonPressed() override;

private:
    uint8_t port_;
    Bounce2::Button button_;
};

class TaskButtonMCP23017 : public TaskButtonBase
{
public:
    TaskButtonMCP23017(Scheduler& sh, Adafruit_MCP23X17& mcp, uint8_t btnPort);

    virtual bool pressed() override;
    virtual byte pin() override;

protected:
    virtual void initButton() override;
    virtual void updateButton() override;
    virtual bool buttonChanged() override;
    virtual bool buttonPressed() override;

private:
    uint8_t port_;
    Bounce2::ButtonMCP23017 button_;
};