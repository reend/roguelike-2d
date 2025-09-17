#ifndef PLAYER_H
#define PLAYER_H

#include "game_types.h"

void InitPlayer(void);
bool CanMoveTo(int x, int y);
void MovePlayer(int dx, int dy);
void AttackEnemy(int enemyId);
void HandlePlayerInput(void);
void LevelUpPlayer(void);

#endif
