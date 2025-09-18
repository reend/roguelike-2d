#ifndef PLAYER_H
#define PLAYER_H

#include "game_types.h"

void InitPlayer(void);
bool CanMoveTo(int x, int y);
bool MovePlayer(int dx, int dy);
void AttackEnemy(int enemyId);
void HandlePlayerInput(void);

#endif
