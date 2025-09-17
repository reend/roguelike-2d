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

#define TILE_FLOOR 0
#define TILE_WALL 1
#define TILE_DOOR 2

typedef struct {
    float x, y;
} Vector2f;

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
} Player;

typedef struct {
    Vector2f position;
    int type;
    int hp;
    int maxHp;
    int damage;
    int expValue;
    float speed;
    float lastMove;
    bool active;
} Enemy;

typedef struct {
    Vector2f position;
    int type;
    int value;
    bool active;
} Item;

typedef struct {
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Item items[MAX_ITEMS];
    int map[MAP_HEIGHT][MAP_WIDTH];
    int currentLevel;
    bool gameOver;
    float cameraX;
    float cameraY;
    Texture2D heroSprite;
} GameState;

extern GameState game;

#endif
