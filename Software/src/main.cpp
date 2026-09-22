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

///////////////////////////// Maze Mapping Prototype ///////////////////////////

// const uint8_t NORTH = 0x01; // 0000 0001 wall north
// const uint8_t EAST  = 0x02; // 0000 0010 wall east
// const uint8_t SOUTH = 0x04; // 0000 0100 wall south
// const uint8_t WEST  = 0x08; // 0000 1000 wall west
// const uint8_t VISITED = 0x10; // 0001 0000 cell has been visited
// const uint8_t VICTIM = 0x20; // 0010 0000 cell has victim
// const uint8_t SILVER = 0x40; // 0100 0000 cell is silver tiled

// static const uint8_t direction_lookup[4][4] = {
//         {NORTH, WEST, SOUTH, EAST}, // Facing NORTH
//         {WEST, SOUTH, EAST, NORTH}, // Facing WEST
//         {SOUTH, EAST, NORTH, WEST}, // Facing SOUTH
//         {EAST, NORTH, WEST, SOUTH}  // Facing EAST
//     };

// struct Move_direction_flag {
//     int dx, dy;
//     uint8_t wall;
// };

// static const Move_direction_flag move_direction[4] = {
//     {0, 1, NORTH},
//     {-1, 0, WEST},
//     {0, -1, SOUTH},
//     {1, 0, EAST}
// }; 

// using Mazemap = map<pair<int, int>, uint8_t>;

// // Function to add a cell definition to the mazemap
// void add_cell_definition(Mazemap& maze, int x, int y, uint8_t cell_definition) {
//     maze[{x, y}] |= cell_definition;

//     switch (cell_definition) {
//         case NORTH:
//             maze[{x, y + 1}] |= SOUTH;
//             break;
//         case EAST:
//             maze[{x + 1, y}] |= WEST;
//             break;
//         case SOUTH:
//             maze[{x, y - 1}] |= NORTH;
//             break;
//         case WEST:
//             maze[{x - 1, y}] |= EAST;
//             break;
//         default:
//             break;
//     }
// }
// // Function to recognize new cells without walls and add them to the mazemap
// void recognise_cell_definitions(Mazemap& maze, int x, int y, uint8_t cell_definitions) {
    
//     switch (cell_definitions) {
//             case (NORTH):
//                 maze[{x, y + 1}] = maze[{x, y + 1}];
//                 break;
//             case (EAST):
//                 maze[{x + 1, y}] = maze[{x + 1, y}];
//                 break;
//             case (SOUTH):  
//                 maze[{x, y - 1}] = maze[{x, y - 1}];
//                 break;
//             case (WEST):
//                 maze[{x - 1, y}] = maze[{x - 1, y}];
//                 break;
//             default:
//                 break;
//     }

// }

// //Function to process sensors and update maze accordingly
// void process_direction(Mazemap& maze, int x, int y, int& distance, uint8_t direction) {
//     if (distance < 150) {
//         add_cell_definition(maze, x, y, direction);
//     } else {
//         recognise_cell_definitions(maze, x, y, direction);
//     }
// }

// //Function to check if it's possible to move to that cell
// bool can_move(Mazemap& maze, int x, int y, const Move_direction_flag& move_direction) {
//     pair<int, int> cell = {x,y};
//     if (maze.find(cell) == maze.end()) {
//         return false;
//     } else {
//     return ((maze[cell] & move_direction.wall) == 0);
//     }
// }

// //FInd path to next cell
// vector<pair<int,int>> path_next_cell(Mazemap& maze, int x, int y) {

//     map<pair<int,int>,pair<int,int>> parent;
//     map<pair<int,int>, bool> visited_in_search;
//     queue<pair<int,int>> search_queue;

//     //starting conditions of search

//     pair<int,int> start = {x,y};
//     search_queue.push(start);
//     bool found_unvisted_cell = false;
//     pair<int, int> next_cell;

//     //search maze
//     while(!search_queue.empty()) {
//         pair<int,int> current_cell = search_queue.front();
//         search_queue.pop();
        
//         //checking if current cell has found target
//         if (!(maze[current_cell] & VISITED)) {
//             found_unvisted_cell = true;
//             next_cell = current_cell;
//             break;
//         }
//         //check neighbour cells of current cell
//         for (const Move_direction_flag& direction : move_direction) {
//             //if can move to that cell
//             if (can_move(maze, current_cell.first, current_cell.second, direction)) {
//                 pair<int, int> neighbour_cell = {current_cell.first + direction.dx, current_cell.second + direction.dy};
//                 //if not found before
//                 if (!visited_in_search[neighbour_cell]) {
//                     visited_in_search[neighbour_cell] = true;
//                     parent[neighbour_cell] = current_cell;
//                     search_queue.push(neighbour_cell);
//                 }
//             }
//         }
//     }

//     //create vector of path
//     vector<pair<int, int>> path;
//     if (found_unvisted_cell) {
//         for (pair<int, int> cell = next_cell; cell != start; cell = parent[cell] ) {
//             path.push_back(cell);
//         }
//         reverse(path.begin(), path.end());
//     }
//     return path;
// }

/////////////////////////////// Global Variables ///////////////////////////////

uint8_t state;
uint16_t target_dist;
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
    Serial.println("Starting");
    // Initialise all the sensors
    // ColourSensor.init();
    motor.init();
    lrfs.init();
    Serial.println("Motors and LRFs initialised");

    // DropperServo.attach(DROPPER);
    // delay(100);
    // DropperServo.write(90);
    // delay(500);
    while(!bno.begin(OPERATION_MODE_IMUPLUS)) {
        Serial.println("No BNO055 detected. Check your wiring or I2C ADDR.");
        delay(1000);
    }

    // // Maze setup
    // state = NAV;

}




void loop() {
    Serial.printf("State: %d\n", state);
    // READ ALL OF THE SENSORS
    // IMU, Colour, LRFs

    lrfs.update();

    // ColourSensor.update();

    sensors_event_t event;
    bno.getEvent(&event);
    current_bearing = event.orientation.x;
    if (current_bearing > 180.0f) {
        current_bearing -= 360.0f;
    }

    // FSMs
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
    
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;

    if (front < target_dist) {
        pause_start = millis();
        state = PAUSE;
        return;
    }

    uint16_t left = (lrfs.get_value(LRF_LF) + lrfs.get_value(LRF_LB)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;

    if (left > 240) {
        left = 220 - right;
    } else if (right > 240) {
        right = 220 - left;
    }

    int16_t error = right - left;
    
    float bearing_adjustment = atanf((float)error / (float)TILE_DIST) * RAD_TO_DEG;
    if (bearing_adjustment > 20.0f) {
        bearing_adjustment = 20.0f;
    } else if (bearing_adjustment < -20.0f) {
        bearing_adjustment = -20.0f;
    }

    float bearing_error = current_bearing - (target_bearing + bearing_adjustment);
    // Serial.printf("LRF Error: %d\tbearing adjustment: %.2f\tbearing error: %.2f\n", error, bearing_adjustment, bearing_error);
    
    float correction = bearing_error * BEARING_KP;
    motor.move(MOVE_SPEED - correction, MOVE_SPEED + correction); 
}

void rotate_left()
{
    if (fabs(current_bearing - target_bearing) < ROTATION_MARGIN) {
        uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
        // uint8_t front_direction = direction_lookup[rotations % 4][0];
        // switch(front_direction) {
        //     case(NORTH):
        //         y++;
        //         break;
        //     case(EAST):
        //         x++;
        //         break;
        //     case(SOUTH):
        //         y--;
        //         break;
        //     case(WEST):
        //         x--;
        //         break;
        //     default;
        //         break;
    
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
        // uint8_t front_direction = direction_lookup[rotations % 4][0];
        // switch(front_direction) {
        //     case(NORTH):
        //         y++;
        //         break;
        //     case(EAST):
        //         x++;
        //         break;
        //     case(SOUTH):
        //         y--;
        //         break;
        //     case(WEST):
        //         x--;
        //         break;
        //     default;
        //         break;
        // }
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
        // uint8_t front_direction = direction_lookup[rotations % 4][0];
        // switch(front_direction) {
        //     case(NORTH):
        //         y++;
        //         break;
        //     case(EAST):
        //         x++;
        //         break;
        //     case(SOUTH):
        //         y--;
        //         break;
        //     case(WEST):
        //         x--;
        //         break;
        //     default;
        //         break;
        // }
    } else {
        motor.move(ROTATE_SPEED, -ROTATE_SPEED);
    }

}

void navigation()
{
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;

    // uint8_t front_direction = direction_lookup[rotations % 4][0];
    // uint8_t left_direction = direction_lookup[rotations % 4][1];
    // uint8_t back_direction = direction_lookup[rotations % 4][2];
    // uint8_t right_direction = direction_lookup[rotations % 4][3];

    // process_direction(maze, x, y, front, front_direction);
    // process_direction(maze, x, y, left, left_direction);
    // process_direction(maze, x, y, right, right_direction);

    if (left > TILE_DIST) {
        target_bearing -= 90.0f;
        state = ROTATE_L;
        // rotations--;
    
    } else if (front > TILE_DIST) {
        target_dist = max(front - TILE_DIST, MIN_TARGET_DIST);
        state = FORWARD;
        // switch(front_direction) {
        //     case(NORTH):
        //         y++;
        //         break;
        //     case(EAST):
        //         x++;
        //         break;
        //     case(SOUTH):
        //         y--;
        //         break;
        //     case(WEST):
        //         x--;
        //         break;
        //     default;
        //         break;
            
    } else if (right > TILE_DIST) {
        target_bearing += 90.0f;
        state = ROTATE_R;
        // rotations++;
    
    } else {
        target_bearing -= 180.0f;
        state = ROTATE_180;
        // rotations += 2;
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