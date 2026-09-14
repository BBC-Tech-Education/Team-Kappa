#include <iostream>
#include <map>

using namespace std;

// Maze Cell definitions
const uint8_t NORTH = 0x01; // 0000 0001 wall north
const uint8_t EAST  = 0x02; // 0000 0010 wall east
const uint8_t SOUTH = 0x04; // 0000 0100 wall south
const uint8_t WEST  = 0x08; // 0000 1000 wall west
const uint8_t VISITED = 0x10; // 0001 0000 cell has been visited
const uint8_t VICTIM = 0x20; // 0010 0000 cell has victim
const uint8_t SILVER = 0x40; // 0100 0000 cell is silver tiled

using Mazemap = map<pair<int, int>, uint8_t>;

void add_cell_definition(Mazemap& maze, int x, int y, uint8_t cell_definition) {
    maze[{x, y}] |= cell_definition;

    switch (cell_definition) {
        case NORTH:
            maze[{x, y + 1}] |= SOUTH;
            break;
        case EAST:
            maze[{x + 1, y}] |= WEST;
            break;
        case SOUTH:
            maze[{x, y - 1}] |= NORTH;
            break;
        case WEST:
            maze[{x - 1, y}] |= EAST;
            break;
        default:
            break;
    }
    
}
//PLANNED REPLACEMENT OF NAV STATE
int main() {
    Mazemap maze;

    //LRFS
    uint16_t left = (lrfs.get_value(LRF_LB) + lrfs.get_value(LRF_LF)) / 2;
    uint16_t front = (lrfs.get_value(LRF_FL) + lrfs.get_value(LRF_FR)) / 2;
    uint16_t right = (lrfs.get_value(LRF_RF) + lrfs.get_value(LRF_RB)) / 2;
    uint16_t back = (lrfs.get_value(LRF_BR) + lrfs.get_value(LRF_BL)) / 2;

    
    //test coordinates
    int x = -4;
    int y = 2;
    //test conditions to that cell
    add_cell_definition(maze, x, y, VISITED);
    add_cell_definition(maze, x, y, WEST);
    //print out all cell definitions
    for (auto& cell : maze) {
        int i = cell.first.first;
        int j = cell.first.second;
        uint8_t definitions = cell.second;

        cout << "Cell (" << i << ", " << j << ") has definitions: ";
        if (definitions & NORTH) cout << "NORTH wall,";
        if (definitions & EAST) cout << "EAST wall, ";
        if (definitions & SOUTH) cout << "SOUTH wall, ";
        if (definitions & WEST) cout << "WEST wall, ";
        if (definitions & VISITED) cout << "VISITED, ";
        if (definitions & VICTIM) cout << "VICTIM";

        cout << endl;
    }
    //ROTATION TEST
    for (int i = 0; i<10; i++) {
        int rotations = i;

        cout << "Rotations: " << rotations << " Direction:";

        switch (rotations % 4) {
            case 0:
                cout << "Facing NORTH" << endl; //IF FACING THIS WAY, LRF VALUE OF XYZ MEANS XYZ ETC FOR CELL DEFINITIONS
                break;
            case 1:
                cout << "Facing WEST" << endl;
                break;
            case 2:
                cout << "Facing SOUTH" << endl;
                break;
            case 3:
                cout << "Facing EAST" << endl;
                break;
            default:
                break;
        //what are odds rotations gets really high?
        }
    }
    return 0;
}

