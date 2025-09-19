#include "game_state.h"
#include "player.h"
#include "enemy.h"
#include "dungeon.h"
#include "renderer.h"
#include "resources.h"
#include "hero_selection.h"

GameState game = {0};

void InitGame(void) {
    LoadSprites();
    InitHeroTypes();
    StartHeroSelection();
}

void StartActualGame(void) {
    InitPlayer();
    
    game.currentLevel = 1;
    game.gameOver = false;
    game.playerTurn = true;
    game.cameraX = 0;
    game.cameraY = 0;
    
    InitEnemies();
    
    for (int i = 0; i < MAX_ITEMS; i++) {
        game.items[i].active = false;
    }
    
    for (int i = 0; i < MAX_MESSAGES; i++) {
        game.messages[i].active = false;
    }
    
    GenerateDungeon();
    PlacePlayerInDungeon();
    SpawnEnemies();
}

void UpdateGame(void) {
    if (game.inHeroSelection) {
        UpdateHeroSelection();
        if (!game.inHeroSelection) {
            StartActualGame();
        }
        return;
    }
    
    if (game.gameOver) return;
    
    HandlePlayerInput();
    UpdateEnemies();
    UpdateGameCamera();
}

void RestartGame(void) {
    UnloadSprites();
    InitGame();
}
