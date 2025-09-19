#include "player_stats.h"
#include "constants.h"

void InitPlayerStats(void) {
    game.player.level = 1;
    game.player.exp = 0;
    game.player.expToNext = PLAYER_BASE_EXP_TO_NEXT;
    game.player.gold = 0;
    game.player.alive = true;
}

void LevelUpPlayer(void) {
    game.player.level++;
    game.player.exp -= game.player.expToNext;
    game.player.expToNext = game.player.level * PLAYER_BASE_EXP_TO_NEXT;
    game.player.maxHp += PLAYER_HP_PER_LEVEL;
    game.player.hp = game.player.maxHp;
    game.player.strength += PLAYER_STRENGTH_PER_LEVEL;
    game.player.defense += PLAYER_DEFENSE_PER_LEVEL;
}

void AddExperience(int exp) {
    game.player.exp += exp;
    if (game.player.exp >= game.player.expToNext) {
        LevelUpPlayer();
    }
}

void AddGold(int gold) {
    game.player.gold += gold;
}

bool TakeDamage(int damage) {
    game.player.hp -= damage;
    if (game.player.hp <= 0) {
        game.player.alive = false;
        game.gameOver = true;
        return true;
    }
    return false;
}
