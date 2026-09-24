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
}

uint8_t Map::update(uint16_t front, uint16_t right, uint16_t back, uint16_t left)
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

    // Figure out which way to go next

    // Return the next direction
}

void Map::create_tiles()
{
    for(uint8_t i = 0; i < 4; i++)  {
        if (!(map[current_tile_id].info & direction_lookup[i].info)) {
            
            int8_t target_x = map[current_tile_id].x + direction_lookup[i].x;
            int8_t target_y = map[current_tile_id].y + direction_lookup[i].y;

            if (find_tile(target_x, target_y) == tile_num) {
                tile_num++;
                map = (Tile_t*)realloc(map, sizeof(Tile_t) * tile_num);
                map[tile_num - 1].x = map[current_tile_id].x + direction_lookup[i].x;
                map[tile_num - 1].y = map[current_tile_id].y + direction_lookup[i].y;
                map[tile_num - 1].info = 0;
            }
        }
    }
}

uint8_t Map::find_tile(int8_t target_x, int8_t target_y) 
{
    for(uint8_t i = 0; i < tile_num; i++) {
        if ((map[i].x == target_x) & (map[i].y == target_y)) {
            return i;
        } 
    }
    return tile_num;
}

void Map::get_target_heading(float bearing) {
    float target_bearing = bearing;
}



