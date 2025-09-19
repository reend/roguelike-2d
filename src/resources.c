#include "resources.h"
#include <stdio.h>

void LoadSprites(void) {
    game.heroSprite = LoadTexture("assets/heroes.png");
    if (game.heroSprite.id > 0) {
        SetTextureFilter(game.heroSprite, TEXTURE_FILTER_POINT);
        printf("Hero sprite loaded successfully! Size: %dx%d\n", 
               game.heroSprite.width, game.heroSprite.height);
    } else {
        printf("Failed to load hero sprite from assets/heroes.png\n");
    }
    
    game.enemySprite = LoadTexture("assets/enemies.png");
    if (game.enemySprite.id > 0) {
        SetTextureFilter(game.enemySprite, TEXTURE_FILTER_POINT);
        printf("Enemy sprite loaded successfully! Size: %dx%d\n", 
               game.enemySprite.width, game.enemySprite.height);
    } else {
        printf("Failed to load enemy sprite from assets/enemies.png\n");
    }
}

void UnloadSprites(void) {
    UnloadTexture(game.heroSprite);
    UnloadTexture(game.enemySprite);
}
