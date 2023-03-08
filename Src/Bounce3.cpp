
#include "Bounce3.h"

////////////
// BOUNCE //
////////////


void Bounce3::attach(Pin& pin) {
    this->pin = pin;
    
    // SET INITIAL STATE
    begin();
}

void Bounce3::attach(Pin& pin, int mode){
    pin.pinMode(mode);
    this->attach(pin);
}



