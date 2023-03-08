#include "Pin.h"
         
#include <MCP23017.h>

void ArduinoPin::pinMode(uint8_t mode) { ::pinMode(pin_, mode);}
uint8_t ArduinoPin::digitalRead() { return ::digitalRead(pin_);}
uint8_t ArduinoPin::digitalWrite(uint8_t value) { ::digitalWrite(pin_, value);}


void MCP23X17Pin::pinMode(uint8_t mode) { mcp_.pinMode(pin_, mode);}
uint8_t MCP23X17Pin::digitalRead() { return mcp_.digitalRead(pin_);}
uint8_t MCP23X17Pin::digitalWrite(uint8_t value) { mcp_.digitalWrite(pin_, value);}