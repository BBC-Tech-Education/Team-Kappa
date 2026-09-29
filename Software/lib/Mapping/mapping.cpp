#include "mapping.h"



Map::Map()
{

}

uint8_t Map::map_victim() 
{
    if (map[current_tile_id].info & VIC_BMSK) {
        return SEEN_VIC;
    } else {
        return NEW_VIC;
    }
}

void Map::victim_update()
{
    map[current_tile_id].info |= VIC_BMSK;
}

void Map::found_black_tile()
{
    int8_t current_heading = target_bearing / 90;
    map[current_tile_id].info |= BLACK_BMSK;
    current_tile_id = map[current_tile_id].connected_tile_id[(current_heading + 6) % 4];
}


void Map::init()
{
    current_tile_id = 0;
    tile_num = 1;
    target_bearing = 0.0f;

    map = (Tile_t*)malloc(sizeof(Tile_t) * tile_num);

    map[current_tile_id].id = current_tile_id;
    map[current_tile_id].x = 0;
    map[current_tile_id].y = 0;
    map[current_tile_id].info = 0;
    for (uint8_t i = 0; i < 4; i++) {
        map[current_tile_id].connected_tile_id[i] = 255;
    }
   
}

void Map::update(uint16_t front, uint16_t right, uint16_t back, uint16_t left)
{  
    if (target_bearing > 180.0f) {
        target_bearing -= 360.0f;

    } else if (target_bearing <= -180.0f) {
        target_bearing += 360.0f;
    }
    uint8_t abs_walls = 0;

    int8_t current_heading = target_bearing / 90;
    if (front < CHECK_WALL_DIST) {
        abs_walls |= direction_lookup[(current_heading + 4) % 4].info;
    }
    if (right < CHECK_WALL_DIST) {
        abs_walls |= direction_lookup[(current_heading + 5) % 4].info;
    }
    if (back < CHECK_WALL_DIST) {
        abs_walls |= direction_lookup[(current_heading + 6) % 4].info;
    }
    if (left < CHECK_WALL_DIST) {
        abs_walls |= direction_lookup[(current_heading + 7) % 4].info;
    }
   
    map[current_tile_id].info |= (abs_walls & WALL_BMSK);

    map[current_tile_id].info |= VIS_BMSK;

    // Serial.print("Current Cell: ");
    // Serial.print(map[current_tile_id].id);
    // Serial.print("\t");

    // if (map[current_tile_id].info & WALL_N_BMSK) {
    //     Serial.print("N \t");
    // }
    // if (map[current_tile_id].info & WALL_E_BMSK) {
    //     Serial.print("E \t");
    // }
    // if (map[current_tile_id].info & WALL_S_BMSK) {
    //     Serial.print("S \t");
    // }
    // if (map[current_tile_id].info & WALL_W_BMSK) {
    //     Serial.print("W \t");
    // }
    // Serial.print("\n");
    create_tiles();


    //follow left wall

    // Figure out which way to go next

    // Return the next direction
}

void Map::create_tiles()
{
    for(uint8_t i = 0; i < 4; i++)  {
        if (!(map[current_tile_id].info & direction_lookup[i].info)) {
           
            int8_t target_x = map[current_tile_id].x + direction_lookup[i].x;
            int8_t target_y = map[current_tile_id].y + direction_lookup[i].y;

            uint8_t neighbour_cell_id = find_tile(target_x, target_y);

            if (neighbour_cell_id == 255) { //create tile
                uint8_t id = tile_num++;
                // Serial.print("New Tile id: ");
                // Serial.println(id);
                // Serial.print("Tile Num: ");
                // Serial.println(tile_num);
                map = (Tile_t*)realloc(map, sizeof(Tile_t) * tile_num);

                map[id].id = id;
                map[id].x = map[current_tile_id].x + direction_lookup[i].x;
                map[id].y = map[current_tile_id].y + direction_lookup[i].y;
                map[id].info = 0;
                for (uint8_t i = 0; i < 4; i++) {
                    map[id].connected_tile_id[i] = 255;
                }
                
                map[id].connected_tile_id[(i + 2) % 4] = current_tile_id;
                map[current_tile_id].connected_tile_id[i] = id;
                // Serial.print("New cell connected at ");
                // Serial.print((i + 2) % 4);
                // Serial.println(" with current");
                // Serial.print("Current cell connected at ");
                // Serial.print(i);
                // Serial.println(" with new cell");

            } else { //connect tiles

                map[current_tile_id].connected_tile_id[i] = neighbour_cell_id;
                map[neighbour_cell_id].connected_tile_id[(i + 2) % 4] = current_tile_id;
                // Serial.print("Neighbour cell connected at ");
                // Serial.print((i + 2) % 4);
                // Serial.println(" with current");
                // Serial.print("Current cell connected at ");
                // Serial.print(i);
                // Serial.println(" with neighbour");
            }
        }
    }
}

uint8_t Map::find_tile(int8_t target_x, int8_t target_y)
{
    for(uint8_t i = 0; i < tile_num; i++) {
        if ((map[i].x == target_x) && (map[i].y == target_y)) {
            // Serial.print("Found tile id ");
            // Serial.println(i);
            return i;
        }
    }
    return 255;
}

uint8_t Map::navigate() {
    int8_t current_heading = target_bearing / 90;

    uint8_t rel_front = direction_lookup[(current_heading + 4) % 4].info;
    uint8_t rel_right = direction_lookup[(current_heading + 5) % 4].info;
    uint8_t rel_back = direction_lookup[(current_heading + 6) % 4].info;
    uint8_t rel_left = direction_lookup[(current_heading + 7) % 4].info;

    if (!(map[current_tile_id].info & rel_left)) {
        target_bearing -= 90.0f;
        current_tile_id = map[current_tile_id].connected_tile_id[(current_heading + 7) % 4];
        // Serial.println("state_left");
        // Serial.print("New current id: ");
        // Serial.println(current_tile_id);
        return state_left;

    } else if (!(map[current_tile_id].info & rel_front)) {
        current_tile_id = map[current_tile_id].connected_tile_id[(current_heading + 4) % 4];
        // Serial.println("State_forward");
        // Serial.print("New current id: ");
        // Serial.println(current_tile_id);
        return state_forward;

    } else if (!(map[current_tile_id].info & rel_right)) {
        target_bearing += 90.0f;
        current_tile_id = map[current_tile_id].connected_tile_id[(current_heading + 5) % 4];
        // Serial.println("State_right");
        // Serial.print("New current id: ");
        // Serial.println(current_tile_id);
        return state_right;

    } else {
        target_bearing -= 180.0f;
        current_tile_id = map[current_tile_id].connected_tile_id[(current_heading + 6) % 4];
        // Serial.println("State_180");
        // Serial.print("New current id: ");
        // Serial.println(current_tile_id);
        return state_180;
    }



}