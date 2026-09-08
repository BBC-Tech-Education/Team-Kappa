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
Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS_B, &Wire);

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
} State;


/////////////////////////////// Global Variables ///////////////////////////////

uint8_t state;
uint16_t target_dist;
float target_bearing = 0.0f;
float current_bearing = 0.0f;


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



void setup() {
    // Initialise all the sensors
    ColourSensor.init();
    motor.init();
    lrfs.init();

    while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
        delay(1000);
    }

    // Maze setup
    state = NAV;
}




void loop() {
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs
    lrfs.update();
    ColourSensor.update();
    sensors_event_t event;
    bno.getEvent(&event);
    current_bearing = event.orientation.x;
    if (current_bearing > 180.0f) {
        current_bearing -= 360.0f;
    }


    // FSMs
    switch (state)
    {
    case FORWARD:
        forward();
        break;
    case ROTATE_L:
        rotate_left();
        break;
    case ROTATE_R:
        rotate_right();
        break;
    case ROTATE_180:
        rotate_180();
        break;
    case NAV:
        navigation();
        break;
    case BT_BACK:
        black_tile_backwards();
        break;
    case BT_ROTATE:
        black_tile_rotate();
        break;
    case VICTIMS:
        victims();
        break;
    case SILVER:
        silver_tile();
        break;
    default:
        state = NAV;
        break;
    }
}


void forward()
{
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;

    if (front < target_dist) {
        motor.move(0.0f, 0.0f);
        delay(500);
        state = NAV;
    } else {
        motor.move(MOVE_SPEED, MOVE_SPEED);
    }
}

void rotate_left()
{
    if (fabs(current_bearing - target_bearing) < 5.0f) {
        uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
        target_dist = front - 300;
        motor.move(0.0f, 0.0f);
        delay(500);
        state = FORWARD;
    } else {
        // Rotate left
    }
}

void rotate_right()
{

}

void rotate_180()
{

}

void navigation()
{
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;

    if (left > 300) {
        target_bearing -= 90.0f;
        state = ROTATE_L;
    } else if (front > 300) {
        target_dist = front - 300;
        state = FORWARD;
    } else if (right > 300) {
        target_bearing += 90.0f;
        state = ROTATE_R;
    } else {
        target_bearing -= 180.0f;
        state = ROTATE_180;
    }

    if (target_bearing > 180.0f) {
        target_bearing -= 360.0f;
    } else if (target_bearing < -90.0f) {
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
