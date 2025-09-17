#include "renderer.h"

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
                    tileColor = (Color){64, 64, 64, 255};
                    break;
                case TILE_WALL:
                    tileColor = (Color){32, 32, 32, 255};
                    break;
                default:
                    tileColor = BLACK;
                    break;
            }
            
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, 
                             (Color){48, 48, 48, 255});
        }
    }
}

void DrawEnemies(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        Rectangle enemySrc;
        Color enemyTint = WHITE;
        
        switch (game.enemies[i].type) {
            case 0: 
                enemySrc = (Rectangle){32, 0, 32, 32};
                enemyTint = WHITE;
                break;
            case 1: 
                enemySrc = (Rectangle){64, 0, 32, 32};
                enemyTint = (Color){255, 200, 200, 255};
                break;
            case 2: 
                enemySrc = (Rectangle){96, 0, 32, 32};
                enemyTint = (Color){200, 200, 255, 255};
                break;
            default: 
                enemySrc = (Rectangle){32, 0, 32, 32};
                enemyTint = WHITE;
                break;
        }
        
        Rectangle enemyDest = {
            game.enemies[i].position.x * TILE_SIZE,
            game.enemies[i].position.y * TILE_SIZE,
            TILE_SIZE, TILE_SIZE
        };
        
        if (game.heroSprite.id > 0) {
            DrawTexturePro(game.heroSprite, enemySrc, enemyDest, (Vector2){0, 0}, 0.0f, enemyTint);
        } else {
            Color enemyColor;
            switch (game.enemies[i].type) {
                case 0: enemyColor = RED; break;
                case 1: enemyColor = PURPLE; break;
                case 2: enemyColor = MAROON; break;
                default: enemyColor = RED; break;
            }
            
            DrawCircle(
                game.enemies[i].position.x * TILE_SIZE + TILE_SIZE/2,
                game.enemies[i].position.y * TILE_SIZE + TILE_SIZE/2,
                TILE_SIZE/3, enemyColor
            );
        }
        
        float hpPercent = (float)game.enemies[i].hp / game.enemies[i].maxHp;
        int barWidth = TILE_SIZE - 4;
        DrawRectangle(
            game.enemies[i].position.x * TILE_SIZE + 2,
            game.enemies[i].position.y * TILE_SIZE - 6,
            barWidth, 4, DARKGRAY
        );
        DrawRectangle(
            game.enemies[i].position.x * TILE_SIZE + 2,
            game.enemies[i].position.y * TILE_SIZE - 6,
            barWidth * hpPercent, 4, RED
        );
        
        if (game.selectedEnemy == i) {
            DrawCircleLines(
                game.enemies[i].position.x * TILE_SIZE + TILE_SIZE/2,
                game.enemies[i].position.y * TILE_SIZE + TILE_SIZE/2,
                TILE_SIZE/2 + 4, YELLOW
            );
            DrawCircleLines(
                game.enemies[i].position.x * TILE_SIZE + TILE_SIZE/2,
                game.enemies[i].position.y * TILE_SIZE + TILE_SIZE/2,
                TILE_SIZE/2 + 6, GOLD
            );
        }
    }
}

void DrawPlayer(void) {
    Rectangle heroSrc = {0, 0, 32, 32};
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
            TILE_SIZE, TILE_SIZE, BLUE
        );
    }
}

void DrawUI(void) {
    DrawRectangle(10, 10, 200, 120, Fade(BLACK, 0.7f));
    DrawRectangleLines(10, 10, 200, 120, WHITE);
    
    DrawText(TextFormat("Level: %d", game.player.level), 20, 20, 16, WHITE);
    DrawText(TextFormat("HP: %d/%d", game.player.hp, game.player.maxHp), 20, 40, 16, RED);
    DrawText(TextFormat("MP: %d/%d", game.player.mana, game.player.maxMana), 20, 60, 16, BLUE);
    DrawText(TextFormat("EXP: %d/%d", game.player.exp, game.player.expToNext), 20, 80, 16, GREEN);
    DrawText(TextFormat("Gold: %d", game.player.gold), 20, 100, 16, GOLD);
    
    DrawRectangle(10, 140, 200, 80, Fade(BLACK, 0.7f));
    DrawRectangleLines(10, 140, 200, 80, WHITE);
    
    DrawText("Stats:", 20, 150, 16, WHITE);
    DrawText(TextFormat("STR: %d", game.player.strength), 20, 170, 14, WHITE);
    DrawText(TextFormat("DEF: %d", game.player.defense), 100, 170, 14, WHITE);
    DrawText(TextFormat("MAG: %d", game.player.magic), 20, 190, 14, WHITE);
    DrawText(TextFormat("Floor: %d", game.currentLevel), 100, 190, 14, WHITE);
    
    DrawText("WASD/Arrows - Move", 10, SCREEN_HEIGHT - 100, 14, LIGHTGRAY);
    DrawText("Left Click - Select Enemy", 10, SCREEN_HEIGHT - 80, 14, LIGHTGRAY);
    DrawText("Right Click - Shoot at Selected", 10, SCREEN_HEIGHT - 60, 14, LIGHTGRAY);
    DrawText("R - Restart", 10, SCREEN_HEIGHT - 40, 14, LIGHTGRAY);
    
    if (game.selectedEnemy != -1 && game.selectedEnemy < MAX_ENEMIES && 
        game.enemies[game.selectedEnemy].active) {
        Enemy* target = &game.enemies[game.selectedEnemy];
        DrawText(TextFormat("Target: HP %d/%d DMG %d", 
                target->hp, target->maxHp, target->damage), 
                10, SCREEN_HEIGHT - 120, 14, YELLOW);
    }
    
    if (game.heroSprite.id > 0) {
        DrawText(TextFormat("Hero sprite loaded: %dx%d", 
                          game.heroSprite.width, game.heroSprite.height), 
                10, SCREEN_HEIGHT - 20, 12, GREEN);
    } else {
        DrawText("Hero sprite: FAILED to load assets/heroes.png", 10, SCREEN_HEIGHT - 20, 12, RED);
    }
}

void DrawProjectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game.projectiles[i].active) continue;
        
        Vector2 start = {
            game.projectiles[i].start.x * TILE_SIZE + TILE_SIZE/2,
            game.projectiles[i].start.y * TILE_SIZE + TILE_SIZE/2
        };
        Vector2 current = {
            game.projectiles[i].current.x * TILE_SIZE + TILE_SIZE/2,
            game.projectiles[i].current.y * TILE_SIZE + TILE_SIZE/2
        };
        
        DrawLineEx(start, current, 3.0f, game.projectiles[i].color);
        DrawCircle(current.x, current.y, 4, game.projectiles[i].color);
        DrawCircle(current.x, current.y, 2, WHITE);
    }
}

void DrawEffects(void) {
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!game.effects[i].active) continue;
        
        Vector2 center = {
            game.effects[i].position.x * TILE_SIZE + TILE_SIZE/2,
            game.effects[i].position.y * TILE_SIZE + TILE_SIZE/2
        };
        
        switch (game.effects[i].type) {
            case 0:
                DrawCircleGradient(center.x, center.y, game.effects[i].size, 
                                 game.effects[i].color, Fade(game.effects[i].color, 0));
                break;
            case 1:
                DrawRing(center, game.effects[i].size - 4, game.effects[i].size, 
                        0, 360, 8, game.effects[i].color);
                DrawCircleGradient(center.x, center.y, game.effects[i].size/2, 
                                 game.effects[i].color, Fade(game.effects[i].color, 0));
                break;
            case 2:
                DrawCircleGradient(center.x, center.y, game.effects[i].size, 
                                 game.effects[i].color, Fade(RED, 0));
                DrawRing(center, game.effects[i].size/2, game.effects[i].size, 
                        0, 360, 16, ORANGE);
                break;
        }
    }
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D((Camera2D){{0, 0}, {-game.cameraX, -game.cameraY}, 0, 1.0f});
    
    DrawDungeon();
    DrawEnemies();
    DrawPlayer();
    DrawProjectiles();
    DrawEffects();
    
    EndMode2D();
    
    DrawUI();
    
    if (game.gameOver) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, 48, RED);
        DrawText(TextFormat("You reached level %d", game.player.level), 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 20, WHITE);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 30, 20, WHITE);
    }
    
    EndDrawing();
}
