#include "combat.h"
#include <stdlib.h>
#include <math.h>

void InitCombatSystems(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        game.projectiles[i].active = false;
    }
    
    for (int i = 0; i < MAX_EFFECTS; i++) {
        game.effects[i].active = false;
    }
    
    game.selectedEnemy = -1;
    game.isTargeting = false;
}

int GetEnemyAtScreenPosition(int screenX, int screenY) {
    int worldX = (screenX + game.cameraX) / TILE_SIZE;
    int worldY = (screenY + game.cameraY) / TILE_SIZE;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game.enemies[i].active && 
            (int)game.enemies[i].position.x == worldX && 
            (int)game.enemies[i].position.y == worldY) {
            return i;
        }
    }
    return -1;
}

void HandleMouseInput(void) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        int enemyId = GetEnemyAtScreenPosition((int)mousePos.x, (int)mousePos.y);
        
        if (enemyId != -1) {
            game.selectedEnemy = enemyId;
            game.isTargeting = true;
        } else {
            game.selectedEnemy = -1;
            game.isTargeting = false;
        }
    }
    
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && game.selectedEnemy != -1) {
        ShootAtEnemy(game.selectedEnemy);
        game.selectedEnemy = -1;
        game.isTargeting = false;
    }
}

void ShootAtEnemy(int enemyId) {
    if (!game.enemies[enemyId].active) return;
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game.projectiles[i].active) {
            game.projectiles[i].start.x = game.player.position.x;
            game.projectiles[i].start.y = game.player.position.y;
            game.projectiles[i].end.x = game.enemies[enemyId].position.x;
            game.projectiles[i].end.y = game.enemies[enemyId].position.y;
            game.projectiles[i].current = game.projectiles[i].start;
            game.projectiles[i].speed = 800.0f;
            game.projectiles[i].damage = game.player.strength + GetRandomValue(-3, 3);
            game.projectiles[i].targetEnemyId = enemyId;
            game.projectiles[i].lifetime = 0.0f;
            game.projectiles[i].active = true;
            game.projectiles[i].color = (Color){255, 255, 100, 255};
            
            CreateMuzzleFlashEffect((Vector2f){game.player.position.x, game.player.position.y});
            break;
        }
    }
}

void UpdateProjectiles(float deltaTime) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game.projectiles[i].active) continue;
        
        game.projectiles[i].lifetime += deltaTime;
        
        float dx = game.projectiles[i].end.x - game.projectiles[i].start.x;
        float dy = game.projectiles[i].end.y - game.projectiles[i].start.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance > 0) {
            float normalX = dx / distance;
            float normalY = dy / distance;
            
            game.projectiles[i].current.x += normalX * game.projectiles[i].speed * deltaTime / TILE_SIZE;
            game.projectiles[i].current.y += normalY * game.projectiles[i].speed * deltaTime / TILE_SIZE;
            
            float currentDist = sqrtf(
                (game.projectiles[i].current.x - game.projectiles[i].start.x) * 
                (game.projectiles[i].current.x - game.projectiles[i].start.x) +
                (game.projectiles[i].current.y - game.projectiles[i].start.y) * 
                (game.projectiles[i].current.y - game.projectiles[i].start.y)
            );
            
            if (currentDist >= distance || game.projectiles[i].lifetime > 2.0f) {
                int targetId = game.projectiles[i].targetEnemyId;
                if (targetId >= 0 && targetId < MAX_ENEMIES && game.enemies[targetId].active) {
                    game.enemies[targetId].hp -= game.projectiles[i].damage;
                    
                    CreateImpactEffect((Vector2f){game.enemies[targetId].position.x, game.enemies[targetId].position.y});
                    
                    if (game.enemies[targetId].hp <= 0) {
                        game.player.exp += game.enemies[targetId].expValue;
                        game.player.gold += GetRandomValue(2, 8);
                        
                        CreateExplosionEffect(
                            (Vector2f){game.enemies[targetId].position.x, game.enemies[targetId].position.y},
                            RED
                        );
                        
                        game.enemies[targetId].active = false;
                        
                        if (game.player.exp >= game.player.expToNext) {
                            game.player.level++;
                            game.player.exp -= game.player.expToNext;
                            game.player.expToNext = game.player.level * 100;
                            game.player.maxHp += 20;
                            game.player.hp = game.player.maxHp;
                            game.player.strength += 2;
                            game.player.defense += 1;
                        }
                    }
                }
                
                game.projectiles[i].active = false;
            }
        }
    }
}

void CreateMuzzleFlashEffect(Vector2f position) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!game.effects[i].active) {
            game.effects[i].position = position;
            game.effects[i].type = 0;
            game.effects[i].lifetime = 0.0f;
            game.effects[i].maxLifetime = 0.1f;
            game.effects[i].size = 16.0f;
            game.effects[i].color = (Color){255, 255, 200, 255};
            game.effects[i].active = true;
            break;
        }
    }
}

void CreateImpactEffect(Vector2f position) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!game.effects[i].active) {
            game.effects[i].position = position;
            game.effects[i].type = 1;
            game.effects[i].lifetime = 0.0f;
            game.effects[i].maxLifetime = 0.3f;
            game.effects[i].size = 8.0f;
            game.effects[i].color = (Color){255, 150, 0, 255};
            game.effects[i].active = true;
            break;
        }
    }
}

void CreateExplosionEffect(Vector2f position, Color color) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!game.effects[i].active) {
            game.effects[i].position = position;
            game.effects[i].type = 2;
            game.effects[i].lifetime = 0.0f;
            game.effects[i].maxLifetime = 0.5f;
            game.effects[i].size = 4.0f;
            game.effects[i].color = color;
            game.effects[i].active = true;
            break;
        }
    }
}

void UpdateEffects(float deltaTime) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!game.effects[i].active) continue;
        
        game.effects[i].lifetime += deltaTime;
        
        if (game.effects[i].lifetime >= game.effects[i].maxLifetime) {
            game.effects[i].active = false;
            continue;
        }
        
        float progress = game.effects[i].lifetime / game.effects[i].maxLifetime;
        
        switch (game.effects[i].type) {
            case 0:
                game.effects[i].size = 16.0f * (1.0f - progress);
                game.effects[i].color.a = (unsigned char)(255 * (1.0f - progress));
                break;
            case 1:
                game.effects[i].size = 8.0f + 16.0f * progress;
                game.effects[i].color.a = (unsigned char)(255 * (1.0f - progress));
                break;
            case 2:
                game.effects[i].size = 4.0f + 32.0f * progress;
                game.effects[i].color.a = (unsigned char)(255 * (1.0f - progress));
                break;
        }
    }
}
