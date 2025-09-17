#ifndef DUNGEON_H
#define DUNGEON_H

#include "game_types.h"

void GenerateDungeon(void);
void PlacePlayerInDungeon(void);
bool IsWalkable(int x, int y);

#endif
