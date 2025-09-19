#include "render_helpers.h"
#include "ui_constants.h"
#include "graphics_constants.h"

SpriteInfo GetEnemySpriteInfo(int enemyType) {
    SpriteInfo info;
    
    switch (enemyType) {
        case 0:
            info.sourceRect = (Rectangle){ENEMY_TYPE_0_SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE};
            info.tint = ENEMY_TYPE_0_TINT;
            break;
        case 1:
            info.sourceRect = (Rectangle){ENEMY_TYPE_1_SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE};
            info.tint = ENEMY_TYPE_1_TINT;
            break;
        case 2:
            info.sourceRect = (Rectangle){ENEMY_TYPE_2_SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE};
            info.tint = ENEMY_TYPE_2_TINT;
            break;
        default:
            info.sourceRect = (Rectangle){ENEMY_TYPE_0_SPRITE_X, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE};
            info.tint = ENEMY_TYPE_0_TINT;
            break;
    }
    
    return info;
}

Color GetEnemyColor(int enemyType) {
    switch (enemyType) {
        case 0: return ENEMY_TYPE_0_COLOR;
        case 1: return ENEMY_TYPE_1_COLOR;
        case 2: return ENEMY_TYPE_2_COLOR;
        default: return ENEMY_TYPE_0_COLOR;
    }
}

void DrawHealthBar(float x, float y, int currentHp, int maxHp) {
    if (maxHp <= 0) return;
    
    float healthPercent = (float)currentHp / maxHp;
    int healthBarWidth = (int)(HEALTH_BAR_WIDTH * healthPercent);
    
    int barX = (int)(x + HEALTH_BAR_OFFSET_X);
    int barY = (int)(y - HEALTH_BAR_OFFSET_Y);
    
    DrawRectangle(barX, barY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, DARKGRAY);
    DrawRectangle(barX, barY, healthBarWidth, HEALTH_BAR_HEIGHT, RED);
}

void DrawUIPanel(int x, int y, int width, int height) {
    DrawRectangle(x, y, width, height, UI_BACKGROUND_COLOR);
    DrawRectangleLines(x, y, width, height, UI_BORDER_COLOR);
}

void DrawPlayerStats(void) {
    int x = MAIN_UI_PANEL_X + UI_TEXT_OFFSET_X;
    int y = MAIN_UI_PANEL_Y + UI_TEXT_OFFSET_Y;
    
    DrawText(TextFormat("Level: %d", game.player.level), x, y, UI_TEXT_SIZE, WHITE);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("HP: %d/%d", game.player.hp, game.player.maxHp), x, y, UI_TEXT_SIZE, RED);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("MP: %d/%d", game.player.mana, game.player.maxMana), x, y, UI_TEXT_SIZE, BLUE);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("EXP: %d/%d", game.player.exp, game.player.expToNext), x, y, UI_TEXT_SIZE, GREEN);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("Gold: %d", game.player.gold), x, y, UI_TEXT_SIZE, GOLD);
}

void DrawGameStats(void) {
    int x = STATS_UI_PANEL_X + UI_TEXT_OFFSET_X;
    int y = STATS_UI_PANEL_Y + UI_TEXT_OFFSET_Y;
    
    DrawText("Stats:", x, y, UI_TEXT_SIZE, WHITE);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("STR: %d", game.player.strength), x, y, UI_SMALL_TEXT_SIZE, WHITE);
    DrawText(TextFormat("DEF: %d", game.player.defense), x + STATS_SECOND_COLUMN_OFFSET, y, UI_SMALL_TEXT_SIZE, WHITE);
    y += UI_LINE_HEIGHT;
    DrawText(TextFormat("MAG: %d", game.player.magic), x, y, UI_SMALL_TEXT_SIZE, WHITE);
    DrawText(TextFormat("Floor: %d", game.currentLevel), x + STATS_SECOND_COLUMN_OFFSET, y, UI_SMALL_TEXT_SIZE, WHITE);
}

void DrawControlsHelp(void) {
    int y = SCREEN_HEIGHT - HELP_TEXT_Y_OFFSET;
    
    DrawText("WASD/Arrows - Move", UI_PADDING, y, UI_SMALL_TEXT_SIZE, LIGHTGRAY);
    y += HELP_TEXT_LINE_SPACING;
    
    Color turnColor = game.playerTurn ? TURN_INDICATOR_PLAYER_COLOR : TURN_INDICATOR_ENEMY_COLOR;
    const char* turnText = game.playerTurn ? "Your Turn" : "Enemy Turn";
    DrawText(turnText, UI_PADDING, y, UI_SMALL_TEXT_SIZE, turnColor);
    y += HELP_TEXT_LINE_SPACING;
    
    DrawText("R - Restart", UI_PADDING, y + HELP_TEXT_LINE_SPACING, UI_SMALL_TEXT_SIZE, LIGHTGRAY);
}

void DrawSpriteStatus(void) {
    int y = SCREEN_HEIGHT - HELP_TEXT_LINE_SPACING;
    
    if (game.heroSprite.id > 0) {
        DrawText(TextFormat("Hero sprite loaded: %dx%d", 
                          game.heroSprite.width, game.heroSprite.height), 
                UI_PADDING, y, UI_TINY_TEXT_SIZE, GREEN);
    } else {
        DrawText("Hero sprite: FAILED to load assets/heroes.png", 
                UI_PADDING, y, UI_TINY_TEXT_SIZE, RED);
    }
}

bool IsValidEnemyIndex(int index) {
    return index >= 0 && index < MAX_ENEMIES;
}
