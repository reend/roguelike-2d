#ifndef ENEMY_H
#define ENEMY_H

#include "game_types.h"

void InitEnemies(void);
void SpawnEnemies(void);
void UpdateEnemies(float deltaTime);
void SpawnEnemyAt(int x, int y, int type);

#endif
