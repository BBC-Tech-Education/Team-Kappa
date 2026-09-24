#include "mapping.h"



Map::Map()
{

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
    for (uint8_t i = 0; i <4; i++) {
        map[current_tile_id].connected_tile_id[i] = 255;
    }
   
}

void Map::update(uint16_t front, uint16_t right, uint16_t back, uint16_t left)
{  
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

            neighbour_cell_id = find_tile(target_x, target_y);

            if (neighbour_cell_id == 255) {
                uint8_t id = tile_num++;

                map = (Tile_t*)realloc(map, sizeof(Tile_t) * tile_num);

                map[id].id = id;
                map[id].x = map[current_tile_id].x + direction_lookup[i].x;
                map[id].y = map[current_tile_id].y + direction_lookup[i].y;
                map[id].info = 0;
                map[id].connected_tile_id[(i + 2) % 4] = current_tile_id;
                map[current_tile_id].connected_tile_id[i] = id;

            } else {
                map[current_tile_id].connected_tile_id[i] = neighbour_cell_id;
                map[neighbour_cell_id].connected_tile_id[(i + 2) % 4] = current_tile_id;
            }
        }
    }
}

uint8_t Map::find_tile(int8_t target_x, int8_t target_y)
{
    for(uint8_t i = 0; i < tile_num; i++) {
        if ((map[i].x == target_x) && (map[i].y == target_y)) {
            return i;
        }
    }
    return 255;
}

uint8_t Map::navigate() {

    int8_t current_heading = target_bearing / 90;

    uint8_t rel_front = direction_lookup[(current_heading + 4) % 4].info;
    uint8_t rel_right = direction_lookup[(current_heading + 5) % 4].info;
    uint8_t rel_back = direction_lookup[(current_heading + 6) % 4].info
    uint8_t rel_left = direction_lookup[(current_heading + 7) % 4].info;

    if (map[current_tile_id].info & rel_left) {
        return state_left;
        target_bearing -= 90.0f;

    } else if (map[current_tile_id].info & rel_front) {
        return state_forward;

    } else if (map[current_tile_id].info & rel_right) {
        return state_right;
        target_bearing += 90.0f;

    } else {
        return state_180;
        target_bearing -= 180.0f;
    }

    if (target_bearing > 180.0f) {
        target_bearing -= 360.0f;

    } else if (target_bearing <= -180.0f) {
        target_bearing += 360.0f;
    }

}