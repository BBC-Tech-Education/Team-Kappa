#include <Adafruit_BNO055.h>
#include "Motors.h"
#include <VL53L4CD.h>
#include <Wire.h>
#include "LRFs.h"
#include <Arduino_APDS9960.h>
#include "ColourSensor.h"




//////////////////////////////////// Objects ///////////////////////////////////

Motors motor;
LRFs lrfs;
RGBsensors ColourSensor(Wire2);
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
    SILVER
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
    lrfs.init();
    while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
        delay(1000);
    }
    delay(500);
    bno.setExtCrystalUse(true);
    delay(500);

    // Maze setup
    state = NAV;
}

void loop() {
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs
    ColourSensor.update();
    lrfs.update();
    while (!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected.");
        delay(1000);
    }
    delay(500);
    bno.setExtCrystalUse(true);
    delay(500);

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

}

void rotate_left()
{

}

void rotate_right()
{

}

void rotate_180()
{

}

void navigation()
{

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
