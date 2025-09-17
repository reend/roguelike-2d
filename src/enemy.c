#include "enemy.h"
#include "player.h"
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
            game.enemies[i].hp = 30 + type * 20;
            game.enemies[i].maxHp = game.enemies[i].hp;
            game.enemies[i].damage = 5 + type * 5;
            game.enemies[i].expValue = 10 + type * 5;
            game.enemies[i].speed = 2.0f;
            game.enemies[i].lastMove = 0;
            game.enemies[i].active = true;
            break;
        }
    }
}

void SpawnEnemies(void) {
    for (int i = 0; i < 5; i++) {
        int x, y;
        int attempts = 0;
        
        do {
            x = GetRandomValue(1, MAP_WIDTH - 2);
            y = GetRandomValue(1, MAP_HEIGHT - 2);
            attempts++;
        } while ((game.map[y][x] != TILE_FLOOR || 
                 (abs(x - (int)game.player.position.x) < 3 && 
                  abs(y - (int)game.player.position.y) < 3)) && 
                 attempts < 100);
        
        if (attempts < 100) {
            int type = GetRandomValue(0, 2);
            SpawnEnemyAt(x, y, type);
        }
    }
}

void UpdateEnemies(float deltaTime) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        game.enemies[i].lastMove += deltaTime;
        if (game.enemies[i].lastMove < game.enemies[i].speed) continue;
        
        game.enemies[i].lastMove = 0;
        
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
            if (damage < 1) damage = 1;
            game.player.hp -= damage;
            
            if (game.player.hp <= 0) {
                game.player.alive = false;
                game.gameOver = true;
            }
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
}
