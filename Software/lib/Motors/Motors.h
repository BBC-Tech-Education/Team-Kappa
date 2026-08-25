#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include <Servo.h>
#include <Pins.h>

class Motors {
    public:
        Motors();
        void init();
        void move(float left, float right); // Function to control the 

    private:
        Servo servo[4];
        uint8_t pin[4] = {MOTOR_FL, MOTOR_BL, MOTOR_FR, MOTOR_BR};
      
};

#endif