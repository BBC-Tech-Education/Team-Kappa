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
#include "Adafruit_AS7341.h"

//////////////////////////////////// Objects ///////////////////////////////////

Motors motor;
LRFs lrfs;
// RGBsensors ColourSensor(Wire2);
Adafruit_AS7341 as7341;
Servo DropperServo;
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &Wire1);
// Mazemap maze;

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
uint8_t use_forward_lrfs;


float target_bearing = 0.0f;
float current_bearing = 0.0f;
unsigned long pause_start = millis();
// int rotations = 0;
// int x = 0;
// int y = 0;

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
String stateToName(int st);

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
    Serial.begin(115200);

    while (!Serial) {
        delay(1);
    }
    
    if (!as7341.begin()){
        Serial.println("Could not find AS7341");
        while (1) { delay(10); }
    }
    
    as7341.setATIME(100);
    as7341.setASTEP(999);
    as7341.setGain(AS7341_GAIN_256X);

    // Serial.println("Starting");
    // // Initialise all the sensors

    // motor.init();
    // lrfs.init();
    // Serial.println("Motors and LRFs initialised");


    // while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
    //     Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
    //     delay(1000);
    // }

    // // // Maze setup
    // state = NAV;

}




void loop() {
    // Serial.println(stateToName(state));
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs
    uint16_t readings[12];

    if (!as7341.readAllChannels(readings)){
        Serial.println("Error reading all channels!");
        return;
    }

    Serial.print("ADC0/F1 415nm : ");
    Serial.print(readings[0]);
    Serial.print("\t");
    Serial.print("ADC1/F2 445nm : ");
    Serial.print(readings[1]);
    Serial.print("\t");
    Serial.print("ADC2/F3 480nm : ");
    Serial.print(readings[2]);
    Serial.print("\t");
    Serial.print("ADC3/F4 515nm : ");
    Serial.print(readings[3]);
    Serial.print("\t");
    Serial.print("ADC0/F5 555nm : ");
    
    Serial.print(readings[6]);
    Serial.print("\t");
    Serial.print("ADC1/F6 590nm : ");
    Serial.print(readings[7]);
    Serial.print("\t");
    Serial.print("ADC2/F7 630nm : ");
    Serial.print(readings[8]);
    Serial.print("\t");
    Serial.print("ADC3/F8 680nm : ");
    Serial.print(readings[9]);
    Serial.print("\t");
    Serial.print("ADC4/Clear    : ");
    Serial.print(readings[10]);
    Serial.print("\t");
    Serial.print("ADC5/NIR      : ");
    Serial.println(readings[11]);

    Serial.println();

    // lrfs.update();


    // sensors_event_t event;
    // bno.getEvent(&event);
    // current_bearing = event.orientation.x;
    // if (current_bearing > 180.0f) {
    //     current_bearing -= 360.0f;
    // }

    // // FSMs
    // switch (state) {
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

String stateToName(int st) {
    switch (state) {
    case FORWARD:
        return "Forward";
        break;
    case ROTATE_L:
        return "Turn Left";
        break;
    case ROTATE_R:
        return "Turn Right";
        break;
    case ROTATE_180:
        return "Turn 180";
        break;
    case NAV:
        return "Nav State";
        break;
    case BT_BACK:
        return "BT";
        break;
    case BT_ROTATE:
        return "";
        break;
    case VICTIMS:
       return "V";
        break;
    case SILVER:
        return "Sil";
        break;
    case PAUSE:
        return "Pause";
        break;
    default:
        return "NAN";
        break;
    }
}

void forward()
{
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t back = (lrfs.get_value(LRF_BL) + lrfs.get_value(LRF_BR)) / 2;

    if ((use_forward_lrfs = 1) && (front < target_dist)) {
        pause_start = millis();
        state = PAUSE;
        return;
    } else if ((use_forward_lrfs = 0) && (back > target_dist)) {
        pause_start = millis();
        state = PAUSE;
        return; 
    }
    
    
    

    uint16_t leftlf = lrfs.get_value(LRF_LF);
    uint16_t leftlb = lrfs.get_value(LRF_LB);

    uint16_t left;
    if ((leftlf < 210) && (leftlb < 210)) {
         left = (leftlf + leftlb) / 2;
    } else if (leftlf < 210) {
        left = leftlf;
    } else if (leftlb < 210) {
        left = leftlb;
    } else {
        left = 110;
    }

    uint16_t rightlf = lrfs.get_value(LRF_RF);
    uint16_t rightlb = lrfs.get_value(LRF_RB);

    uint16_t right;
    if ((rightlf < 210) && (rightlb < 210)) {
        right = (rightlf + rightlb) / 2;
    } else if (rightlf < 210) {
        right = rightlf;
    } else if (rightlb < 210) {
        right = rightlb;
    } else {
        right = 110;
    }

    int16_t error = right - left;
    
    float bearing_adjustment = atanf((float)error / (float)TILE_DIST) * RAD_TO_DEG;
    if (bearing_adjustment > 20.0f) {
        bearing_adjustment = 20.0f;
    } else if (bearing_adjustment < -20.0f) {
        bearing_adjustment = -20.0f;
    }

    float adjusted_target_bearing = target_bearing + bearing_adjustment;

    float bearing_error = current_bearing - adjusted_target_bearing;
    // Serial.printf("LRF Error: %d\tbearing adjustment: %.2f\tbearing error: %.2f\n", error, bearing_adjustment, bearing_error);
    
    if (bearing_error <= -180.0f) {
        bearing_error += 360.0f;
    } else if (bearing_error > 180.0f) {
        bearing_error -= 360.0f;
    }

    float correction = bearing_error * BEARING_KP;

    motor.move(MOVE_SPEED - correction, MOVE_SPEED + correction);    
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
    uint16_t back = (lrfs.get_value(LRF_BL) + lrfs.get_value(LRF_BR)) / 2;

    if (left > TILE_DIST) {
        target_bearing -= 90.0f;
        state = ROTATE_L;
    } else if (front > TILE_DIST) {


        if (front > (back + TILE_DIST)) {
            use_forward_lrfs = 0;
            target_dist = back + TILE_DIST;

        
        } else {
            use_forward_lrfs = 1;
            target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);

        }

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
     if ((millis() - pause_start) > 250) {
        state = NAV;
    } else {
        motor.move(0.0f, 0.0f);
    }
}