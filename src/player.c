#include "player.h"
#include "player_stats.h"
#include "constants.h"
#include "game_constants.h"
#include <stdlib.h>

void InitPlayer(void) {
    game.player.position.x = MAP_WIDTH / MAP_CENTER_DIVISOR;
    game.player.position.y = MAP_HEIGHT / MAP_CENTER_DIVISOR;
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
    if (enemyId < 0 || enemyId >= MAX_ENEMIES) return;
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
        moved = MovePlayer(MOVE_UP_X, MOVE_UP_Y);
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        moved = MovePlayer(MOVE_DOWN_X, MOVE_DOWN_Y);
    }
    else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        moved = MovePlayer(MOVE_LEFT_X, MOVE_LEFT_Y);
    }
    else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        moved = MovePlayer(MOVE_RIGHT_X, MOVE_RIGHT_Y);
    }
    
    if (moved) {
        game.playerTurn = false;
    }
}
