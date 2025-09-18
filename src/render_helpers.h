#ifndef RENDER_HELPERS_H
#define RENDER_HELPERS_H

#include "game_types.h"
#include "raylib.h"

typedef struct {
    Rectangle sourceRect;
    Color tint;
} SpriteInfo;

SpriteInfo GetEnemySpriteInfo(int enemyType);
Color GetEnemyColor(int enemyType);
void DrawHealthBar(float x, float y, int currentHp, int maxHp);
void DrawUIPanel(int x, int y, int width, int height);
void DrawPlayerStats(void);
void DrawGameStats(void);
void DrawControlsHelp(void);
void DrawSpriteStatus(void);
bool IsValidEnemyIndex(int index);

#endif
