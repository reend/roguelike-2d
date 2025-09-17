#include "player.h"
#include <stdlib.h>

void InitPlayer(void) {
    game.player.position.x = MAP_WIDTH / 2;
    game.player.position.y = MAP_HEIGHT / 2;
    game.player.level = 1;
    game.player.hp = 100;
    game.player.maxHp = 100;
    game.player.mana = 50;
    game.player.maxMana = 50;
    game.player.exp = 0;
    game.player.expToNext = 100;
    game.player.strength = 10;
    game.player.defense = 5;
    game.player.magic = 5;
    game.player.gold = 0;
    game.player.alive = true;
}

bool CanMoveTo(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    return game.map[y][x] == TILE_FLOOR;
}

void MovePlayer(int dx, int dy) {
    int newX = (int)game.player.position.x + dx;
    int newY = (int)game.player.position.y + dy;
    
    if (CanMoveTo(newX, newY)) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game.enemies[i].active && 
                (int)game.enemies[i].position.x == newX && 
                (int)game.enemies[i].position.y == newY) {
                AttackEnemy(i);
                return;
            }
        }
        
        game.player.position.x = newX;
        game.player.position.y = newY;
    }
}

void LevelUpPlayer(void) {
    game.player.level++;
    game.player.exp -= game.player.expToNext;
    game.player.expToNext = game.player.level * 100;
    game.player.maxHp += 20;
    game.player.hp = game.player.maxHp;
    game.player.strength += 2;
    game.player.defense += 1;
}

void AttackEnemy(int enemyId) {
    if (!game.enemies[enemyId].active) return;
    
    int damage = game.player.strength + GetRandomValue(-2, 2);
    game.enemies[enemyId].hp -= damage;
    
    if (game.enemies[enemyId].hp <= 0) {
        game.player.exp += game.enemies[enemyId].expValue;
        game.player.gold += GetRandomValue(1, 5);
        game.enemies[enemyId].active = false;
        
        if (game.player.exp >= game.player.expToNext) {
            LevelUpPlayer();
        }
    } else {
        int enemyDamage = game.enemies[enemyId].damage - game.player.defense;
        if (enemyDamage < 1) enemyDamage = 1;
        game.player.hp -= enemyDamage;
        
        if (game.player.hp <= 0) {
            game.player.alive = false;
            game.gameOver = true;
        }
    }
}

void HandlePlayerInput(void) {
    if (game.gameOver) return;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        MovePlayer(0, -1);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        MovePlayer(0, 1);
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        MovePlayer(-1, 0);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        MovePlayer(1, 0);
    }
}
