#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TILE_SIZE 32
#define MAP_WIDTH 32
#define MAP_HEIGHT 24
#define MAX_ENEMIES 50
#define MAX_ITEMS 100
#define MAX_MESSAGES 10
#define MAX_HERO_TYPES 4

#define TILE_FLOOR 0
#define TILE_WALL 1
#define TILE_DOOR 2

#define HERO_WARRIOR 0
#define HERO_MAGE 1
#define HERO_ROGUE 2
#define HERO_PALADIN 3

typedef struct {
    float x, y;
} Vector2f;

typedef struct {
    char name[32];
    char description[128];
    int baseHp;
    int baseMana;
    int baseStrength;
    int baseDefense;
    int baseMagic;
    int spriteX;
} HeroType;

typedef struct {
    Vector2f position;
    int level;
    int hp;
    int maxHp;
    int mana;
    int maxMana;
    int exp;
    int expToNext;
    int strength;
    int defense;
    int magic;
    int gold;
    bool alive;
    int heroType;
} Player;

typedef struct {
    Vector2f position;
    int type;
    int hp;
    int maxHp;
    int damage;
    int expValue;
    bool active;
} Enemy;

typedef struct {
    Vector2f position;
    int type;
    int value;
    bool active;
} Item;

typedef struct {
    char text[100];
    Color color;
    float displayTime;
    bool active;
} Message;

typedef struct {
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Item items[MAX_ITEMS];
    Message messages[MAX_MESSAGES];
    int map[MAP_HEIGHT][MAP_WIDTH];
    int currentLevel;
    bool gameOver;
    bool playerTurn;
    float cameraX;
    float cameraY;
    Texture2D heroSprite;
    Texture2D enemySprite;
    bool inHeroSelection;
    int selectedHeroIndex;
} GameState;

extern GameState game;
extern HeroType heroTypes[MAX_HERO_TYPES];

#endif
