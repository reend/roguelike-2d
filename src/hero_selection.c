#include "hero_selection.h"
#include "player_stats.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

HeroType heroTypes[MAX_HERO_TYPES];

void InitHeroTypes(void) {
    strcpy(heroTypes[HERO_WARRIOR].name, "Warrior");
    strcpy(heroTypes[HERO_WARRIOR].description, "Strong melee fighter with high HP and defense");
    heroTypes[HERO_WARRIOR].baseHp = 120;
    heroTypes[HERO_WARRIOR].baseMana = 30;
    heroTypes[HERO_WARRIOR].baseStrength = 15;
    heroTypes[HERO_WARRIOR].baseDefense = 8;
    heroTypes[HERO_WARRIOR].baseMagic = 3;
    heroTypes[HERO_WARRIOR].spriteX = 0;

    strcpy(heroTypes[HERO_MAGE].name, "Mage");
    strcpy(heroTypes[HERO_MAGE].description, "Master of magic with high mana and spell power");
    heroTypes[HERO_MAGE].baseHp = 80;
    heroTypes[HERO_MAGE].baseMana = 100;
    heroTypes[HERO_MAGE].baseStrength = 6;
    heroTypes[HERO_MAGE].baseDefense = 4;
    heroTypes[HERO_MAGE].baseMagic = 15;
    heroTypes[HERO_MAGE].spriteX = 128;

    strcpy(heroTypes[HERO_ROGUE].name, "Rogue");
    strcpy(heroTypes[HERO_ROGUE].description, "Agile assassin with balanced stats and speed");
    heroTypes[HERO_ROGUE].baseHp = 100;
    heroTypes[HERO_ROGUE].baseMana = 60;
    heroTypes[HERO_ROGUE].baseStrength = 12;
    heroTypes[HERO_ROGUE].baseDefense = 6;
    heroTypes[HERO_ROGUE].baseMagic = 8;
    heroTypes[HERO_ROGUE].spriteX = 160;

    strcpy(heroTypes[HERO_PALADIN].name, "Paladin");
    strcpy(heroTypes[HERO_PALADIN].description, "Holy warrior with balanced combat and magic");
    heroTypes[HERO_PALADIN].baseHp = 110;
    heroTypes[HERO_PALADIN].baseMana = 70;
    heroTypes[HERO_PALADIN].baseStrength = 11;
    heroTypes[HERO_PALADIN].baseDefense = 7;
    heroTypes[HERO_PALADIN].baseMagic = 10;
    heroTypes[HERO_PALADIN].spriteX = 192;
}

void StartHeroSelection(void) {
    game.inHeroSelection = true;
    game.selectedHeroIndex = 0;
}

void UpdateHeroSelection(void) {
    if (!game.inHeroSelection) return;

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        game.selectedHeroIndex = (game.selectedHeroIndex - 1 + MAX_HERO_TYPES) % MAX_HERO_TYPES;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        game.selectedHeroIndex = (game.selectedHeroIndex + 1) % MAX_HERO_TYPES;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        SelectHero(game.selectedHeroIndex);
        game.inHeroSelection = false;
    }
}

void DrawHeroSelection(void) {
    if (!game.inHeroSelection) return;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("SELECT YOUR HERO", SCREEN_WIDTH/2 - 150, 50, 32, WHITE);
    DrawText("Use ARROW KEYS or A/D to select, ENTER/SPACE to confirm", SCREEN_WIDTH/2 - 280, 100, 16, GRAY);

    int heroWidth = 200;
    int heroHeight = 300;
    int startX = (SCREEN_WIDTH - MAX_HERO_TYPES * heroWidth) / 2;
    int startY = 150;

    for (int i = 0; i < MAX_HERO_TYPES; i++) {
        int x = startX + i * heroWidth;
        int y = startY;
        
        Color panelColor = (i == game.selectedHeroIndex) ? DARKBLUE : DARKGRAY;
        Color borderColor = (i == game.selectedHeroIndex) ? BLUE : GRAY;
        
        DrawRectangle(x, y, heroWidth - 10, heroHeight, panelColor);
        DrawRectangleLines(x, y, heroWidth - 10, heroHeight, borderColor);

        if (game.heroSprite.id > 0) {
            Rectangle src = {heroTypes[i].spriteX, 0, 32, 32};
            Rectangle dest = {x + (heroWidth - 10 - 64) / 2, y + 20, 64, 64};
            DrawTexturePro(game.heroSprite, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangle(x + (heroWidth - 10 - 64) / 2, y + 20, 64, 64, BLUE);
        }

        DrawText(heroTypes[i].name, x + 10, y + 100, 20, WHITE);
        
        char stats[256];
        snprintf(stats, sizeof(stats), 
                "HP: %d\nMana: %d\nStr: %d\nDef: %d\nMag: %d", 
                heroTypes[i].baseHp, heroTypes[i].baseMana,
                heroTypes[i].baseStrength, heroTypes[i].baseDefense, 
                heroTypes[i].baseMagic);
        DrawText(stats, x + 10, y + 130, 14, LIGHTGRAY);

        int descY = y + 230;
        int lineHeight = 16;
        char* desc = heroTypes[i].description;
        int maxWidth = heroWidth - 30;
        
        char line[64];
        int lineStart = 0;
        int lineEnd = 0;
        
        while (desc[lineEnd] != '\0') {
            int lineLen = 0;
            lineStart = lineEnd;
            
            while (desc[lineEnd] != '\0' && lineLen < maxWidth/8 && desc[lineEnd] != '\n') {
                if (desc[lineEnd] == ' ') {
                    int nextWordLen = 0;
                    int tempEnd = lineEnd + 1;
                    while (desc[tempEnd] != '\0' && desc[tempEnd] != ' ' && desc[tempEnd] != '\n') {
                        nextWordLen++;
                        tempEnd++;
                    }
                    if (lineLen + nextWordLen > maxWidth/8) break;
                }
                lineEnd++;
                lineLen++;
            }
            
            if (desc[lineEnd] == ' ') lineEnd++;
            
            int len = lineEnd - lineStart;
            if (len > 0 && len < (int)sizeof(line)) {
                strncpy(line, desc + lineStart, len);
                line[len] = '\0';
                DrawText(line, x + 10, descY, 12, WHITE);
                descY += lineHeight;
            }
            
            if (desc[lineEnd] == '\0') break;
        }
    }

    EndDrawing();
}

void SelectHero(int heroType) {
    if (heroType < 0 || heroType >= MAX_HERO_TYPES) return;
    
    game.player.heroType = heroType;
    
    HeroType* hero = &heroTypes[heroType];
    game.player.hp = hero->baseHp;
    game.player.maxHp = hero->baseHp;
    game.player.mana = hero->baseMana;
    game.player.maxMana = hero->baseMana;
    game.player.strength = hero->baseStrength;
    game.player.defense = hero->baseDefense;
    game.player.magic = hero->baseMagic;
}

HeroType* GetHeroType(int type) {
    if (type < 0 || type >= MAX_HERO_TYPES) return NULL;
    return &heroTypes[type];
}
