#ifndef COMBAT_H
#define COMBAT_H

#include "game_types.h"

void InitCombatSystems(void);
void HandleMouseInput(void);
void ShootAtEnemy(int enemyId);
void UpdateProjectiles(float deltaTime);
void UpdateEffects(float deltaTime);
void CreateExplosionEffect(Vector2f position, Color color);
void CreateMuzzleFlashEffect(Vector2f position);
void CreateImpactEffect(Vector2f position);
int GetEnemyAtScreenPosition(int screenX, int screenY);

#endif
