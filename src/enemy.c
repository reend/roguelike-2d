#include "enemy.h"
#include "player.h"
#include "player_stats.h"
#include "constants.h"
#include <stdlib.h>
#include <math.h>

void InitEnemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }
}

void SpawnEnemyAt(int x, int y, int type) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) {
            game.enemies[i].position.x = x;
            game.enemies[i].position.y = y;
            game.enemies[i].type = type;
            game.enemies[i].hp = ENEMY_BASE_HP + type * ENEMY_HP_PER_TYPE;
            game.enemies[i].maxHp = game.enemies[i].hp;
            game.enemies[i].damage = ENEMY_BASE_DAMAGE + type * ENEMY_DAMAGE_PER_TYPE;
            game.enemies[i].expValue = ENEMY_BASE_EXP + type * ENEMY_EXP_PER_TYPE;
            game.enemies[i].active = true;
            break;
        }
    }
}

void SpawnEnemies(void) {
    for (int i = 0; i < INITIAL_ENEMY_COUNT; i++) {
        int x, y;
        int attempts = 0;
        
        do {
            x = GetRandomValue(1, MAP_WIDTH - 2);
            y = GetRandomValue(1, MAP_HEIGHT - 2);
            attempts++;
        } while ((game.map[y][x] != TILE_FLOOR || 
                 (abs(x - (int)game.player.position.x) < PLAYER_DISTANCE_FROM_ENEMIES && 
                  abs(y - (int)game.player.position.y) < PLAYER_DISTANCE_FROM_ENEMIES)) && 
                 attempts < MAX_SPAWN_ATTEMPTS);
        
        if (attempts < MAX_SPAWN_ATTEMPTS) {
            int type = GetRandomValue(0, 2);
            SpawnEnemyAt(x, y, type);
        }
    }
}

void UpdateEnemies(void) {
    if (game.playerTurn) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        int dx = 0, dy = 0;
        float playerX = game.player.position.x;
        float playerY = game.player.position.y;
        
        if (game.enemies[i].position.x < playerX) dx = 1;
        else if (game.enemies[i].position.x > playerX) dx = -1;
        
        if (game.enemies[i].position.y < playerY) dy = 1;
        else if (game.enemies[i].position.y > playerY) dy = -1;
        
        int newX = (int)game.enemies[i].position.x + dx;
        int newY = (int)game.enemies[i].position.y + dy;
        
        if (newX == (int)playerX && newY == (int)playerY) {
            int damage = game.enemies[i].damage - game.player.defense;
            if (damage < MIN_DAMAGE) damage = MIN_DAMAGE;
            TakeDamage(damage);
        } else if (CanMoveTo(newX, newY)) {
            bool blocked = false;
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (j != i && game.enemies[j].active && 
                    (int)game.enemies[j].position.x == newX && 
                    (int)game.enemies[j].position.y == newY) {
                    blocked = true;
                    break;
                }
            }
            
            if (!blocked) {
                game.enemies[i].position.x = newX;
                game.enemies[i].position.y = newY;
            }
        }
    }
    
    game.playerTurn = true;
}
