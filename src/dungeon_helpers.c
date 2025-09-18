#include "dungeon_helpers.h"
#include "constants.h"
#include <stdlib.h>
#include <math.h>

PositionList CreatePositionList(int capacity) {
    PositionList list;
    list.positions = (Position*)malloc(capacity * sizeof(Position));
    list.count = 0;
    list.capacity = capacity;
    return list;
}

void AddPosition(PositionList* list, int x, int y) {
    if (list->count < list->capacity) {
        list->positions[list->count].x = x;
        list->positions[list->count].y = y;
        list->count++;
    }
}

Position GetRandomPosition(PositionList* list) {
    if (list->count == 0) {
        Position fallback = {MAP_WIDTH / 2, MAP_HEIGHT / 2};
        return fallback;
    }
    
    int index = GetRandomValue(0, list->count - 1);
    return list->positions[index];
}

void FreePositionList(PositionList* list) {
    if (list->positions) {
        free(list->positions);
        list->positions = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

PositionList FindValidPlayerPositions(void) {
    PositionList validPositions = CreatePositionList(MAP_WIDTH * MAP_HEIGHT);
    
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        for (int x = 1; x < MAP_WIDTH - 1; x++) {
            if (game.map[y][x] == TILE_FLOOR) {
                AddPosition(&validPositions, x, y);
            }
        }
    }
    
    return validPositions;
}

PositionList FindValidEnemyPositions(int playerX, int playerY) {
    PositionList validPositions = CreatePositionList(MAP_WIDTH * MAP_HEIGHT);
    
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        for (int x = 1; x < MAP_WIDTH - 1; x++) {
            if (game.map[y][x] == TILE_FLOOR) {
                int distanceX = abs(x - playerX);
                int distanceY = abs(y - playerY);
                
                if (distanceX >= PLAYER_DISTANCE_FROM_ENEMIES || 
                    distanceY >= PLAYER_DISTANCE_FROM_ENEMIES) {
                    AddPosition(&validPositions, x, y);
                }
            }
        }
    }
    
    return validPositions;
}
