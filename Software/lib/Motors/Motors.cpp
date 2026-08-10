#include "Motors.h"

Motors::Motors() {}

void Motors::init()
{
    for (uint8_t i = 0; i < 4; i++) {
        servo[i].attach (pin[i]);
    }  
}

void Motors::move(float left, float right){ //Left = left pair of motors, right = right pair of motors
    uint8_t l = (uint8_t)(0.9f * left + 90.0f); //the equation converts %speed to value between 0 and 180 to control motor speed
    uint8_t r = (uint8_t)(0.9f * right + 90.0f); //same thing but for right set of motors

    servo[0].write(l);
    servo[1].write(l);
    servo[2].write(r);
    servo[3].write(r);
}  

