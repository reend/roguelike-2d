#include "dungeon.h"
#include "dungeon_helpers.h"
#include <stdlib.h>

void GenerateDungeon(void) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                game.map[y][x] = TILE_WALL;
            } else if ((x % 4 == 0 && y % 4 == 0) || GetRandomValue(0, 100) < 20) {
                game.map[y][x] = TILE_WALL;
            } else {
                game.map[y][x] = TILE_FLOOR;
            }
        }
    }
}

void PlacePlayerInDungeon(void) {
    PositionList validPositions = FindValidPlayerPositions();
    
    if (validPositions.count > 0) {
        Position playerPos = GetRandomPosition(&validPositions);
        game.player.position.x = playerPos.x;
        game.player.position.y = playerPos.y;
    } else {
        game.player.position.x = MAP_WIDTH / 2;
        game.player.position.y = MAP_HEIGHT / 2;
    }
    
    FreePositionList(&validPositions);
}

bool IsWalkable(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    return game.map[y][x] == TILE_FLOOR;
}
