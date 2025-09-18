#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include "game_types.h"

void InitPlayerStats(void);
void LevelUpPlayer(void);
void AddExperience(int exp);
void AddGold(int gold);
bool TakeDamage(int damage);

#endif
