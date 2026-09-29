#include "Adafruit_BNO055.h"
#include "colour_sensor.h"
#include "Config.h"
#include "LRFs.h"
#include "Motors.h"
#include "VL53L4CD.h"
#include "Servo.h"
#include <Arduino.h>
#include "mapping.h"

//////////////////////////////////// Objects ///////////////////////////////////

Motors motor;
LRFs lrfs;
ColourSensor colour;
Servo dropper_servo;
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &Wire1);
Map maze;

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

uint8_t harmed_victim_cond;
uint8_t unharmed_victim_cond;
uint8_t black_tile_cond;
uint8_t silver_tile_cond;
uint8_t harmed_victim_counter;
uint8_t unharmed_victim_counter;

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
void dropper_left();
void dropper_right();
String stateToName(int st);



/////////////////////////////////// Functions //////////////////////////////////


void setup()
{
    delay(2000);

    // Initialise all the sensors

    motor.init();
    lrfs.init();
    colour.init();
    maze.init();
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    dropper_servo.attach(DROPPER);
    delay(100);
    dropper_servo.write(90);


    while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
        delay(1000);
    }

    // // Maze setup
    state = NAV;
}




void loop()
{
    // READ ALL OF THE SENSORS
    static sensors_event_t event;
    // // delay(2000);
    // // dropper_left();
 
    
    lrfs.update();
    colour.update();
    bno.getEvent(&event);

    current_bearing = event.orientation.x;
    if (current_bearing > 180.0f) {
        current_bearing -= 360.0f;
    }
    Serial.println(stateToName(state));
    // // Serial.printf("R: %d, G: %d, B: %d, S: %d\n", colour.detect_red(), colour.detect_green(), colour.detect_black(), colour.detect_silver());


    // FSM
    switch (state) {
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
    case PAUSE:
        pause();
        break;
    default:
        state = NAV;
        break;
    }
}

void forward()
{

    // TRANSITIONS TO PAUSE STATE
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t back = (lrfs.get_value(LRF_BL) + lrfs.get_value(LRF_BR)) / 2;

    if (black_tile_cond == 1) {
        state = BT_BACK;
        
    }

    if ((use_forward_lrfs = 1) && (front < target_dist)) {
        pause_start = millis();
        state = PAUSE;
        return;
    } else if ((use_forward_lrfs = 0) && (back > target_dist)) {
        pause_start = millis();
        state = PAUSE;
        return; 
    } 
    
    
    
    // ERROR CODE: CREATING WALLS IF NOT THERE, USING WALLS IF THERE
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


    // CENTERING AND ALIGNMENT CODE
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

    
    // ACTUAL LINE THAT MOVES MOTORS
    motor.move(MOVE_SPEED - correction, MOVE_SPEED + correction);    

    // Colour detection

    if (colour.detect_green() == 1) {
        unharmed_victim_cond = 1;
    } else if (colour.detect_red() == 1) {
        harmed_victim_cond = 1;
    } else if (colour.detect_black() == 1) {
        black_tile_cond = 1;
        harmed_victim_cond = 0;
        unharmed_victim_cond = 0;
    } else if (colour.detect_silver() == 1) {
        silver_tile_cond = 1;
    }

    // Serial.printf("Green: %d\tRed: %d\tBlack: %d\n", unharmed_victim_cond, harmed_victim_cond, black_tile_cond);
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
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;
    uint16_t back = (lrfs.get_value(LRF_BL) + lrfs.get_value(LRF_BR)) / 2;
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;


    maze.update(front, right, back, left);
    uint8_t next_state = maze.navigate();
    switch(next_state) {
    case(state_forward):
        if (front > (back + TILE_DIST)) {
        use_forward_lrfs = 0;
        target_dist = back + TILE_DIST;    
        } else {
            use_forward_lrfs = 1;
            target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        }
        state = FORWARD;
        break;

    case(state_left):
        target_bearing -= 90.0f;
        state = ROTATE_L;
        break;

    case(state_right):
        target_bearing += 90.0f;
        state = ROTATE_R;
        break;

    case(state_180):

        target_bearing -= 180.0f;
        state = ROTATE_180;
        break;
    }

    if (target_bearing > 180.0f) {
         target_bearing -= 360.0f;
    } else if (target_bearing <= -180.0f) {
        target_bearing += 360.0f;
    }

}


void black_tile_backwards()
{
    // 
}

void black_tile_rotate()
{
    // Only left state or 80 state.

}

void victims()
{   
    uint8_t vic_seen_bef = maze.map_victim();
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;
    
    switch(vic_seen_bef) {
    case(NEW_VIC):
        if (harmed_victim_cond == 1) {
            harmed_victim_cond = 0;
            harmed_victim_counter++;
            digitalWrite(LED_RED, HIGH);
            delay(3000);
            digitalWrite(LED_RED, LOW);

            if (right < TARGET_WALL_DIST)
                dropper_right();
            else {
                dropper_left();
            }
            state = NAV;

        } else if (unharmed_victim_cond == 1) {
            unharmed_victim_cond = 0;
            unharmed_victim_counter++;
            digitalWrite(LED_GREEN, HIGH);
            delay(3000);
            digitalWrite(LED_GREEN, LOW);
            state = NAV;
        }
        maze.victim_update();
    
    case(SEEN_VIC):
        unharmed_victim_cond = 0;
        harmed_victim_cond = 0;
        state = NAV;
    }
}


void silver_tile()
{

}


void pause()
{
    if ((millis() - pause_start) > 250) {
        if (unharmed_victim_cond == 1 || harmed_victim_cond == 1) {
        state = VICTIMS;
        } else {
        state = NAV;
        }
    } else {
        motor.move(0.0f, 0.0f);
    }
}

void dropper_left() {
    delay(100);
    dropper_servo.write(180); //turns motor right
    delay(500);
    dropper_servo.write(75);
    delay(500);
    dropper_servo.write(90);
}

void dropper_right() {
    delay(100);
    dropper_servo.write(0); //turns motor left
    delay(500);
    dropper_servo.write(105);
    delay(500);
    dropper_servo.write(90);
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