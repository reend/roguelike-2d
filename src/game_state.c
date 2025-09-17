#include "game_state.h"
#include "player.h"
#include "enemy.h"
#include "dungeon.h"
#include "renderer.h"
#include "resources.h"

GameState game = {0};

void InitGame(void) {
    InitPlayer();
    
    game.currentLevel = 1;
    game.gameOver = false;
    game.cameraX = 0;
    game.cameraY = 0;
    
    InitEnemies();
    
    for (int i = 0; i < MAX_ITEMS; i++) {
        game.items[i].active = false;
    }
    
    GenerateDungeon();
    PlacePlayerInDungeon();
    SpawnEnemies();
    LoadSprites();
}

void UpdateGame(void) {
    if (game.gameOver) return;
    
    float deltaTime = GetFrameTime();
    
    HandlePlayerInput();
    UpdateEnemies(deltaTime);
    UpdateGameCamera();
}

void RestartGame(void) {
    UnloadSprites();
    InitGame();
}
