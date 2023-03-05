#pragma once

#include <Adafruit_MCP23X17.h>
#include <Bounce2.h>

/**
@brief The Debouncer:BounceMCP23017 class. Links the Deboucing class to a hardware pin.  This class is odly named, but
it will be kept that so it stays compatible with previous code.

     */
class BounceMCP23017 : public Debouncer
{
public:
    /*!
        @brief  Create an instance of the Bounce class.

        @code

        // Create an instance of the Bounce class.
        Bounce() button;

        @endcode
    */
    BounceMCP23017(Adafruit_MCP23X17& mcp);

    /*!
        @brief  Attach to a pin and sets that pin's mode (INPUT, INPUT_PULLUP or OUTPUT).

        @param    pin
                  The pin that is to be debounced.
        @param    mode
                  A valid Arduino pin mode (INPUT, INPUT_PULLUP or OUTPUT).
    */
    void attach(int pin, int mode);

    /**
    Attach to a pin for advanced users. Only attach the pin this way once you have previously set it up. Otherwise use
    attach(int pin, int mode).
    */
    void attach(int pin);

    BounceMCP23017(Adafruit_MCP23X17& mcp, uint8_t pin, unsigned long interval_millis) : BounceMCP23017(mcp)
    {
        attach(pin);
        interval(interval_millis);
    }

    /**
     @brief Return pin that this Bounce is attached to

     @return integer identifier of the coupled pin
     */
    inline int getPin() const { return this->pin; };

protected:
    Adafruit_MCP23X17& mcp;
    uint8_t pin;

    virtual bool readCurrentState() { return mcp.digitalRead(pin); }
    virtual void setPinMode(int pin, int mode) { mcp.pinMode(pin, mode); }
};

/**
     @brief The Debouncer:Bounce:Button class. The Button class matches an electrical state to a physical action.
     */
namespace Bounce2
{
// code declarations

class ButtonMCP23017 : public BounceMCP23017
{
protected:
    bool stateForPressed = 1;  //
public:
    /*!
@brief  Create an instance of the Button class. By default, the pressed state is matched to a HIGH electrical level.

@code

// Create an instance of the Button class.
ButtonMCP23017() button;

@endcode
*/
    ButtonMCP23017(Adafruit_MCP23X17& mcp) : BounceMCP23017(mcp) {}

    /*!
    @brief Set the electrical state (HIGH/LOW) that corresponds to a physical press. By default, the pressed state is
    matched to a HIGH electrical level.

    @param    state
              The electrical state (HIGH/LOW) that corresponds to a physical press.

*/
    void setPressedState(bool state) { stateForPressed = state; }

    /*!
    @brief Get the electrical state (HIGH/LOW) that corresponds to a physical press.
    */
    inline bool getPressedState() const { return stateForPressed; };

    /*!
    @brief Returns true if the button is currently physically pressed.
    */
    inline bool isPressed() const { return read() == getPressedState(); };

    /*!
    @brief Returns true if the button was physically pressed
*/
    inline bool pressed() const { return changed() && isPressed(); };

    /*!
@brief Returns true if the button was physically released
*/
    inline bool released() const { return changed() && !isPressed(); };
};
};  // namespace Bounce2
