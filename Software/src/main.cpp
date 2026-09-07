#include "Adafruit_BNO055.h"
#include "Motors.h"
#include "VL53L4CD.h"
#include <Wire.h>
#include "LRFs.h"
#include "Arduino_APDS9960.h"
#include "ColourSensor.h"
#include "ServoC.h"

//dropper test
ServoC DropperServo;
void Droppersetup() {
    DropperServo.attach(DROPPER);
}

void DropperL() {
    DropperServo.write(90);
}

void DropperR() {
    DropperServo.write(-90);
}

int main() {
    Droppersetup();
    DropperR();
}

//////////////////////////////////// Objects ///////////////////////////////////

Motors motor;
LRFs lrfs;
RGBsensors ColourSensor(Wire2);

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

    // Maze setup
    state = NAV;
}

void loop() {
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs
    ColourSensor.update();


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
    case DROPPER_LEFT:
        dropper_left();
        break;
    case DROPPER_RIGHT:
        dropper_right();
        break;
    default:
        state = NAV;
        break;
    }
}