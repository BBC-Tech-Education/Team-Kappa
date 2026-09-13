#include <iostream>
#include <map>
#include <cstdint>

using namespace std;

// Maze Cell definitions
const uint8_t NORTH = 0x01; // 0000 0001 wall north
const uint8_t EAST  = 0x02; // 0000 0010 wall east
const uint8_t SOUTH = 0x04; // 0000 0100 wall south
const uint8_t WEST  = 0x08; // 0000 1000 wall west
const uint8_t VISITED = 0x10; // 0001 0000 cell has been visited
const uint8_t VICTIM = 0x20; // 0010 0000 cell has victim

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

int main() {
    Mazemap maze;
    
    int x = -4;
    int y = 2;

    add_cell_definition(maze, x, y, VISITED);
    add_cell_definition(maze, x, y, WEST);

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
    return 0;
}

