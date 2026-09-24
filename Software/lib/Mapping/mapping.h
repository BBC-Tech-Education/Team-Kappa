#ifndef MAPPING_H
#define MAPPING_H

#include <Arduino.h>
#include <vector>

#define WALL_N_BMSK (1U << 0)
#define WALL_E_BMSK (1U << 1)
#define WALL_S_BMSK (1U << 2)
#define WALL_W_BMSK (1U << 3)
#define WALL_BMSK   0x0FU
#define VIS_BMSK    (1U << 4)
#define VIC_BMSK    (1U << 5)
#define BLACK_BMSK  (1U << 6)

#define CHECK_WALL_DIST 150

class Map {
public:
    Map();
    void init();
    uint8_t update(uint16_t front, uint16_t right, uint16_t back, uint16_t left);

    void found_black_tile();
    void found_victim();
    void get_target_heading(float target_bearing);

private:

    void create_tiles();
    uint8_t find_tile(int8_t target_x, int8_t target_y);

    typedef struct {
        uint8_t id;
        int8_t x;
        int8_t y;
        uint8_t info;
    } Tile_t;

    static constexpr Tile_t direction_lookup[4] = {
        {.x = 0, .y = 1, .info = WALL_N_BMSK},
        {.x = 1, .y = 0, .info = WALL_E_BMSK},
        {.x = 0, .y = -1, .info = WALL_S_BMSK},
        {.x = -1, .y = 0, .info = WALL_W_BMSK}
    }; 

    uint8_t current_tile_id;
    uint8_t tile_num;
    Tile_t* map;
    float target_bearing;
};

#endif