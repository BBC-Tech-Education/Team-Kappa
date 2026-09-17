#include <Adafruit_BNO055.h>
#include "Motors.h"
#include <VL53L4CD.h>
#include <Wire.h>
#include "LRFs.h"
#include "Arduino_APDS9960.h"
#include "Arduino.h"
#include "ColourSensor.h"
#include "Servo.h"
#include "Config.h"


//////////////////////////////////// Objects ///////////////////////////////////

Motors motor;
LRFs lrfs;
RGBsensors ColourSensor(Wire2);
Servo DropperServo;
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &Wire1);

////////////////////////////////////// FSM /////////////////////////////////////

typedef enum {
    FORWARD,
    ROTATE_L,
    ROTATE_R,
    ROTATE_180,
    NAV,
    BT_BACK,
    BT_ROTATE,
    VICTIMS,
    SILVER,
    PAUSE,
} State;


/////////////////////////////// Global Variables ///////////////////////////////

uint8_t state;
uint16_t target_dist;
float target_bearing = 0.0f;
float current_bearing = 0.0f;
unsigned long pause_start = millis();
bool left_wall;
bool right_wall;
bool front_wall;
bool back_wall;


////////////////////////////// Function Prototypes /////////////////////////////

void forward();
void rotate_left();
void rotate_right();
void rotate_180();
void navigation();
void black_tile_backwards();
void black_tile_rotate();
void victims();
void silver_tile();
void pause();

void dropper_left() {
    delay(100);
    DropperServo.write(180); //turns motor right
    delay(500);
    DropperServo.write(80);
    delay(500);
    DropperServo.write(90);
}
void dropper_right() {
    delay(100);
    DropperServo.write(0); //turns motor left
    delay(500);
    DropperServo.write(100);
    delay(500);
    DropperServo.write(90);
}


void setup() {
    delay(5000);
    // Initialise all the sensors
    ColourSensor.init();
    // motor.init();
    // lrfs.init();

    // DropperServo.attach(DROPPER);
    // delay(100);
    // DropperServo.write(90);
    // delay(500);
    // while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
    //     Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
    //     delay(1000);
    // }

    // // Maze setup
    // state = NAV;

}




void loop() {
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs

    // lrfs.update();
    ColourSensor.update();
    if (ColourSensor.detect_green()) {
        Serial.println("Green works");
    } else if (ColourSensor.detect_red()) {
         Serial.println("Red Works.");
    } else {
        Serial.println("No victim detected.");
    }
    // sensors_event_t event;
    // bno.getEvent(&event);
    // current_bearing = event.orientation.x;
    // if (current_bearing > 180.0f) {
    //     current_bearing -= 360.0f;
    // }


    // Serial.print("State: ");
    // Serial.println(state);
    // Serial.print("\t");




    // // Serial.print("\tCurrent Bearing: ");
    // // Serial.println(current_bearing);


    // // FSMs
    // switch (state)
    // {
    // case FORWARD:
    //     forward();
    //     break;
    // case ROTATE_L:
    //     rotate_left();
    //     break;
    // case ROTATE_R:
    //     rotate_right();
    //     break;
    // case ROTATE_180:
    //     rotate_180();
    //     break;
    // case NAV:
    //     navigation();
    //     break;
    // case BT_BACK:
    //     black_tile_backwards();
    //     break;
    // case BT_ROTATE:
    //     black_tile_rotate();
    //     break;
    // case VICTIMS:
    //     victims();
    //     break;
    // case SILVER:
    //     silver_tile();
    //     break;
    // case PAUSE:
    //     pause();
    //     break;
    // default:
    //     state = NAV;
    //     break;
    // }
}


void forward()
{
    
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t left = (lrfs.get_value(LRF_LF) + lrfs.get_value(LRF_LB)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;
    int Error = left - right;

    Serial.print("Left LRF Difference: ");
    Serial.print(left);
    Serial.print ("\t");

    float correction;
    // float correction = Kp * Error;
    if (front < target_dist) {
        pause_start = millis();
        state = PAUSE;

    } else {
        if ((left < 200) && (right < 200)) {
            correction = Kp * Error;
            motor.move(MOVE_SPEED - correction, MOVE_SPEED + correction);

        } else if (right < 200) {
            correction = Kp * (TARGET_WALL_DIST - right);
            motor.move(MOVE_SPEED - correction, MOVE_SPEED + correction);

        } else if (left < 200) {
            correction = Kp *(TARGET_WALL_DIST - left);
            motor.move(MOVE_SPEED + correction, MOVE_SPEED - correction);

        } else {
            motor.move(MOVE_SPEED, MOVE_SPEED);
        }
    }

}

void rotate_left()
{
    if (fabs(current_bearing - target_bearing) < ROTATION_MARGIN) {
        uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
    } else {
        motor.move(-ROTATE_SPEED, ROTATE_SPEED);
    }
}

void rotate_right()
{
    if (fabs(current_bearing - target_bearing) < ROTATION_MARGIN) {
        uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
    } else {
        motor.move(ROTATE_SPEED, -ROTATE_SPEED);
    }
}

void rotate_180()
{
    if (fabs(current_bearing - target_bearing) < ROTATION_MARGIN) {
        uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
    } else {
        motor.move(ROTATE_SPEED, -ROTATE_SPEED);
    }

}

void navigation()
{
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;

    if (left > TILE_DIST) {
        target_bearing -= 90.0f;
        state = ROTATE_L;
    } else if (front > TILE_DIST) {
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
    } else if (right > TILE_DIST) {
        target_bearing += 90.0f;
        state = ROTATE_R;
    } else {
        target_bearing -= 180.0f;
        state = ROTATE_180;
    }

    if (target_bearing > 180.0f) {
        target_bearing -= 360.0f;
    } else if (target_bearing <= -180.0f) {
        target_bearing += 360.0f;
    }
}

void black_tile_backwards()
{

}

void black_tile_rotate()
{

}


void victims()
{
    
}


void silver_tile()
{

}


void pause()
{
    // if ((millis() - pause_start) > 250) {
        state = NAV;
    // } else {
    //     motor.move(0.0f, 0.0f);
    // }
}