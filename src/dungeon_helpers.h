#ifndef DUNGEON_HELPERS_H
#define DUNGEON_HELPERS_H

#include "game_types.h"

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position* positions;
    int count;
    int capacity;
} PositionList;

PositionList CreatePositionList(int capacity);
void AddPosition(PositionList* list, int x, int y);
Position GetRandomPosition(PositionList* list);
void FreePositionList(PositionList* list);

PositionList FindValidPlayerPositions(void);
PositionList FindValidEnemyPositions(int playerX, int playerY);

#endif
