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
}

void UnloadSprites(void) {
    UnloadTexture(game.heroSprite);
}
