#pragma once

#include <Bounce2.h>
#include "Pin.h"

class Bounce3 : public Debouncer
{
public:
    void attach(Pin& pin, int mode);
    void attach(Pin& pin);

    Bounce3(Pin& pin) : pin(pin) { attach(pin); }

    Bounce3(Pin& pin, unsigned long interval_millis) : pin(pin)
    {
        attach(pin);
        interval(interval_millis);
    }

    Bounce3(Pin& pin, uint8_t mode, unsigned long interval_millis) : pin(pin)
    {
        attach(pin, mode);
        interval(interval_millis);
    }

    inline Pin& getPin() const { return this->pin; };

protected:
    Pin& pin;

    virtual bool readCurrentState() { return pin.digitalRead(); }
    virtual void setPinMode(int pin, int mode) { this->pin.pinMode(mode); }
};

namespace Bounce2
{
// code declarations

class Button3 : public Bounce3
{
protected:
    bool stateForPressed = 1;  //
public:
    Button3(Pin& pin) : Bounce3(pin) {}

    Button3(Pin& pin, unsigned long interval_millis) : Bounce3(pin, interval_millis) {}

    Button3(Pin& pin, uint8_t mode, unsigned long interval_millis) : Bounce3(pin, mode, interval_millis) {}

    void setPressedState(bool state) { stateForPressed = state; }

    inline bool getPressedState() const { return stateForPressed; };

    inline bool isPressed() const { return read() == getPressedState(); };

    inline bool pressed() const { return changed() && isPressed(); };

    inline bool released() const { return changed() && !isPressed(); };
};
};  // namespace Bounce2
