#include "raylib.h"
#include "game_state.h"
#include "renderer.h"
#include "resources.h"
#include "hero_selection.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Roguelike RPG");
    SetTargetFPS(60);
    
    InitGame();
    
    while (!WindowShouldClose()) {
        if (!game.gameOver) {
            UpdateGame();
        } else if (IsKeyPressed(KEY_R)) {
            RestartGame();
        }
        
        if (game.inHeroSelection) {
            DrawHeroSelection();
        } else {
            DrawGame();
        }
    }
    
    UnloadSprites();
    CloseWindow();
    return 0;
}
