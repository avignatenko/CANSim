// Please read Bounce2.h for information about the liscence and authors


#include "Bounce2MCP23017.h"

////////////
// BOUNCE //
////////////


BounceMCP23017::BounceMCP23017(Adafruit_MCP23X17& mcp)
    : mcp(mcp), pin(0)
{}

void BounceMCP23017::attach(int pin) {
    this->pin = pin;
    
    // SET INITIAL STATE
    begin();
}

void BounceMCP23017::attach(int pin, int mode){
    mcp.pinMode(pin, mode);
    this->attach(pin);
}



