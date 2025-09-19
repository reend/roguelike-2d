#include "renderer.h"
#include "render_helpers.h"
#include "ui_constants.h"
#include "graphics_constants.h"
#include "hero_selection.h"

void UpdateGameCamera(void) {
    game.cameraX = game.player.position.x * TILE_SIZE - SCREEN_WIDTH / 2;
    game.cameraY = game.player.position.y * TILE_SIZE - SCREEN_HEIGHT / 2;
    
    if (game.cameraX < 0) game.cameraX = 0;
    if (game.cameraY < 0) game.cameraY = 0;
    if (game.cameraX > MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH) 
        game.cameraX = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;
    if (game.cameraY > MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT) 
        game.cameraY = MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT;
}

void DrawDungeon(void) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Color tileColor;
            switch (game.map[y][x]) {
                case TILE_FLOOR:
                    tileColor = TILE_FLOOR_COLOR;
                    break;
                case TILE_WALL:
                    tileColor = TILE_WALL_COLOR;
                    break;
                default:
                    tileColor = BLACK;
                    break;
            }
            
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_BORDER_COLOR);
        }
    }
}

void DrawEnemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        float enemyX = game.enemies[i].position.x * TILE_SIZE;
        float enemyY = game.enemies[i].position.y * TILE_SIZE;
        
        SpriteInfo spriteInfo = GetEnemySpriteInfo(game.enemies[i].type);
        Rectangle enemyDest = {enemyX, enemyY, TILE_SIZE, TILE_SIZE};
        
        if (game.enemySprite.id > 0) {
            DrawTexturePro(game.enemySprite, spriteInfo.sourceRect, enemyDest, 
                          (Vector2){0, 0}, 0.0f, spriteInfo.tint);
        } else {
            Color enemyColor = GetEnemyColor(game.enemies[i].type);
            DrawCircle(enemyX + TILE_SIZE/ENEMY_CIRCLE_CENTER_DIVISOR, enemyY + TILE_SIZE/ENEMY_CIRCLE_CENTER_DIVISOR, TILE_SIZE/ENEMY_CIRCLE_RADIUS_DIVISOR, enemyColor);
        }
        
        DrawHealthBar(enemyX, enemyY, game.enemies[i].hp, game.enemies[i].maxHp);
    }
}

void DrawPlayer(void) {
    HeroType* heroType = GetHeroType(game.player.heroType);
    int spriteX = heroType ? heroType->spriteX : PLAYER_SPRITE_X;
    
    Rectangle heroSrc = {spriteX, SPRITE_Y, SPRITE_SIZE, SPRITE_SIZE};
    Rectangle heroDest = {
        game.player.position.x * TILE_SIZE,
        game.player.position.y * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };
    
    if (game.heroSprite.id > 0) {
        DrawTexturePro(game.heroSprite, heroSrc, heroDest, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangle(
            game.player.position.x * TILE_SIZE,
            game.player.position.y * TILE_SIZE,
            TILE_SIZE, TILE_SIZE, PLAYER_FALLBACK_COLOR
        );
    }
}

void DrawUI(void) {
    DrawUIPanel(MAIN_UI_PANEL_X, MAIN_UI_PANEL_Y, MAIN_UI_PANEL_WIDTH, MAIN_UI_PANEL_HEIGHT);
    DrawPlayerStats();
    
    DrawUIPanel(STATS_UI_PANEL_X, STATS_UI_PANEL_Y, STATS_UI_PANEL_WIDTH, STATS_UI_PANEL_HEIGHT);
    DrawGameStats();
    
    DrawControlsHelp();
    DrawSpriteStatus();
}

void DrawMessages(void) {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (!game.messages[i].active) continue;
        
        int y = MESSAGES_START_Y + i * MESSAGES_LINE_HEIGHT;
        DrawText(game.messages[i].text, UI_PADDING + UI_TEXT_OFFSET_X, y, UI_TEXT_SIZE, game.messages[i].color);
    }
}


void DrawGame(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D((Camera2D){{0, 0}, {-game.cameraX, -game.cameraY}, 0, 1.0f});
    
    DrawDungeon();
    DrawEnemies();
    DrawPlayer();
    
    EndMode2D();
    
    DrawUI();
    DrawMessages();
    
    if (game.gameOver) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, GAME_OVER_OVERLAY_ALPHA));
        DrawText("GAME OVER", SCREEN_WIDTH/2 - GAME_OVER_TITLE_OFFSET_X, SCREEN_HEIGHT/2 - GAME_OVER_TITLE_OFFSET_Y, GAME_OVER_TITLE_FONT_SIZE, RED);
        DrawText(TextFormat("You reached level %d", game.player.level), 
                SCREEN_WIDTH/2 - GAME_OVER_LEVEL_OFFSET_X, SCREEN_HEIGHT/2 + GAME_OVER_LEVEL_OFFSET_Y, GAME_OVER_TEXT_FONT_SIZE, WHITE);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - GAME_OVER_RESTART_OFFSET_X, SCREEN_HEIGHT/2 + GAME_OVER_RESTART_OFFSET_Y, GAME_OVER_TEXT_FONT_SIZE, WHITE);
    }
    
    EndDrawing();
}
