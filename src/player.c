#include "player.h"
#include "player_stats.h"
#include "constants.h"
#include <stdlib.h>

void InitPlayer(void) {
    game.player.position.x = MAP_WIDTH / 2;
    game.player.position.y = MAP_HEIGHT / 2;
    InitPlayerStats();
}

bool CanMoveTo(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    return game.map[y][x] == TILE_FLOOR;
}

bool MovePlayer(int dx, int dy) {
    if (!game.playerTurn) return false;
    
    int newX = (int)game.player.position.x + dx;
    int newY = (int)game.player.position.y + dy;
    
    if (CanMoveTo(newX, newY)) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game.enemies[i].active && 
                (int)game.enemies[i].position.x == newX && 
                (int)game.enemies[i].position.y == newY) {
                AttackEnemy(i);
                return true;
            }
        }
        
        game.player.position.x = newX;
        game.player.position.y = newY;
        return true;
    }
    return false;
}


void AttackEnemy(int enemyId) {
    if (!game.enemies[enemyId].active) return;
    
    int damage = game.player.strength + GetRandomValue(-MELEE_DAMAGE_VARIANCE, MELEE_DAMAGE_VARIANCE);
    game.enemies[enemyId].hp -= damage;
    
    if (game.enemies[enemyId].hp <= 0) {
        AddExperience(game.enemies[enemyId].expValue);
        AddGold(GetRandomValue(ENEMY_GOLD_MIN, ENEMY_GOLD_MAX));
        game.enemies[enemyId].active = false;
    } else {
        int enemyDamage = game.enemies[enemyId].damage - game.player.defense;
        if (enemyDamage < MIN_DAMAGE) enemyDamage = MIN_DAMAGE;
        TakeDamage(enemyDamage);
    }
}

void HandlePlayerInput(void) {
    if (game.gameOver || !game.playerTurn) return;
    
    bool moved = false;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        moved = MovePlayer(0, -1);
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        moved = MovePlayer(0, 1);
    }
    else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        moved = MovePlayer(-1, 0);
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        moved = MovePlayer(1, 0);
    }
    
    if (moved) {
        game.playerTurn = false;
    }
}
