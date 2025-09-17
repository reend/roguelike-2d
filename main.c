#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TILE_SIZE 32
#define MAP_WIDTH 30
#define MAP_HEIGHT 20
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

GameState game = {0};

void AttackEnemy(int enemyId);

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

void GenerateDungeon(void) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                game.map[y][x] = TILE_WALL;
            } else if ((x % 4 == 0 && y % 4 == 0) || GetRandomValue(0, 100) < 20) {
                game.map[y][x] = TILE_WALL;
            } else {
                game.map[y][x] = TILE_FLOOR;
            }
        }
    }
}

void SpawnEnemies(void) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game.enemies[j].active) {
                int x, y;
                do {
                    x = GetRandomValue(1, MAP_WIDTH - 2);
                    y = GetRandomValue(1, MAP_HEIGHT - 2);
                } while (game.map[y][x] != TILE_FLOOR || 
                        (abs(x - (int)game.player.position.x) < 3 && 
                         abs(y - (int)game.player.position.y) < 3));
                
                game.enemies[j].position.x = x;
                game.enemies[j].position.y = y;
                game.enemies[j].type = GetRandomValue(0, 2);
                game.enemies[j].hp = 30 + game.enemies[j].type * 20;
                game.enemies[j].maxHp = game.enemies[j].hp;
                game.enemies[j].damage = 5 + game.enemies[j].type * 5;
                game.enemies[j].expValue = 10 + game.enemies[j].type * 5;
                game.enemies[j].speed = 2.0f;
                game.enemies[j].lastMove = 0;
                game.enemies[j].active = true;
                break;
            }
        }
    }
}

void InitGame(void) {
    game.player.position.x = MAP_WIDTH / 2;
    game.player.position.y = MAP_HEIGHT / 2;
    game.player.level = 1;
    game.player.hp = 100;
    game.player.maxHp = 100;
    game.player.mana = 50;
    game.player.maxMana = 50;
    game.player.exp = 0;
    game.player.expToNext = 100;
    game.player.strength = 10;
    game.player.defense = 5;
    game.player.magic = 5;
    game.player.gold = 0;
    game.player.alive = true;
    
    game.currentLevel = 1;
    game.gameOver = false;
    game.cameraX = 0;
    game.cameraY = 0;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }
    
    for (int i = 0; i < MAX_ITEMS; i++) {
        game.items[i].active = false;
    }
    
    GenerateDungeon();
    
    int startX, startY;
    do {
        startX = GetRandomValue(1, MAP_WIDTH - 2);
        startY = GetRandomValue(1, MAP_HEIGHT - 2);
    } while (game.map[startY][startX] != TILE_FLOOR);
    
    game.player.position.x = startX;
    game.player.position.y = startY;
    
    SpawnEnemies();
    LoadSprites();
}

bool CanMoveTo(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    return game.map[y][x] == TILE_FLOOR;
}

void MovePlayer(int dx, int dy) {
    int newX = (int)game.player.position.x + dx;
    int newY = (int)game.player.position.y + dy;
    
    if (CanMoveTo(newX, newY)) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game.enemies[i].active && 
                (int)game.enemies[i].position.x == newX && 
                (int)game.enemies[i].position.y == newY) {
                AttackEnemy(i);
                return;
            }
        }
        
        game.player.position.x = newX;
        game.player.position.y = newY;
    }
}

void AttackEnemy(int enemyId) {
    if (!game.enemies[enemyId].active) return;
    
    int damage = game.player.strength + GetRandomValue(-2, 2);
    game.enemies[enemyId].hp -= damage;
    
    if (game.enemies[enemyId].hp <= 0) {
        game.player.exp += game.enemies[enemyId].expValue;
        game.player.gold += GetRandomValue(1, 5);
        game.enemies[enemyId].active = false;
        
        if (game.player.exp >= game.player.expToNext) {
            game.player.level++;
            game.player.exp -= game.player.expToNext;
            game.player.expToNext = game.player.level * 100;
            game.player.maxHp += 20;
            game.player.hp = game.player.maxHp;
            game.player.strength += 2;
            game.player.defense += 1;
        }
    } else {
        int enemyDamage = game.enemies[enemyId].damage - game.player.defense;
        if (enemyDamage < 1) enemyDamage = 1;
        game.player.hp -= enemyDamage;
        
        if (game.player.hp <= 0) {
            game.player.alive = false;
            game.gameOver = true;
        }
    }
}

void UpdateGame(void) {
    if (game.gameOver) return;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        MovePlayer(0, -1);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        MovePlayer(0, 1);
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        MovePlayer(-1, 0);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        MovePlayer(1, 0);
    }
    
    float deltaTime = GetFrameTime();
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        game.enemies[i].lastMove += deltaTime;
        if (game.enemies[i].lastMove < game.enemies[i].speed) continue;
        
        game.enemies[i].lastMove = 0;
        
        int dx = 0, dy = 0;
        float playerX = game.player.position.x;
        float playerY = game.player.position.y;
        
        if (game.enemies[i].position.x < playerX) dx = 1;
        else if (game.enemies[i].position.x > playerX) dx = -1;
        
        if (game.enemies[i].position.y < playerY) dy = 1;
        else if (game.enemies[i].position.y > playerY) dy = -1;
        
        int newX = (int)game.enemies[i].position.x + dx;
        int newY = (int)game.enemies[i].position.y + dy;
        
        if (newX == (int)playerX && newY == (int)playerY) {
            int damage = game.enemies[i].damage - game.player.defense;
            if (damage < 1) damage = 1;
            game.player.hp -= damage;
            
            if (game.player.hp <= 0) {
                game.player.alive = false;
                game.gameOver = true;
            }
        } else if (CanMoveTo(newX, newY)) {
            bool blocked = false;
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (j != i && game.enemies[j].active && 
                    (int)game.enemies[j].position.x == newX && 
                    (int)game.enemies[j].position.y == newY) {
                    blocked = true;
                    break;
                }
            }
            
            if (!blocked) {
                game.enemies[i].position.x = newX;
                game.enemies[i].position.y = newY;
            }
        }
    }
    
    game.cameraX = game.player.position.x * TILE_SIZE - SCREEN_WIDTH / 2;
    game.cameraY = game.player.position.y * TILE_SIZE - SCREEN_HEIGHT / 2;
    
    if (game.cameraX < 0) game.cameraX = 0;
    if (game.cameraY < 0) game.cameraY = 0;
    if (game.cameraX > MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH) 
        game.cameraX = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;
    if (game.cameraY > MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT) 
        game.cameraY = MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT;
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode2D((Camera2D){{0, 0}, {-game.cameraX, -game.cameraY}, 0, 1.0f});
    
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
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
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
    }
    
    Rectangle heroSrc = {0, 0, 32, 32};
    Rectangle heroDest = {
        game.player.position.x * TILE_SIZE,
        game.player.position.y * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };
    
    if (game.heroSprite.id > 0) {
        DrawTexturePro(game.heroSprite, heroSrc, heroDest, (Vector2){0, 0}, 0.0f, WHITE);
        static bool debugPrinted = false;
        if (!debugPrinted) {
            printf("Drawing hero sprite at (%d, %d)\n", 
                   (int)(game.player.position.x * TILE_SIZE), 
                   (int)(game.player.position.y * TILE_SIZE));
            debugPrinted = true;
        }
    } else {
        DrawRectangle(
            game.player.position.x * TILE_SIZE,
            game.player.position.y * TILE_SIZE,
            TILE_SIZE, TILE_SIZE, BLUE
        );
    }
    
    EndMode2D();
    
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
    
    DrawText("WASD/Arrows - Move", 10, SCREEN_HEIGHT - 80, 14, LIGHTGRAY);
    DrawText("Move into enemy to attack", 10, SCREEN_HEIGHT - 60, 14, LIGHTGRAY);
    DrawText("R - Restart", 10, SCREEN_HEIGHT - 40, 14, LIGHTGRAY);
    
    if (game.heroSprite.id > 0) {
        DrawText(TextFormat("Hero sprite loaded: %dx%d", 
                          game.heroSprite.width, game.heroSprite.height), 
                10, SCREEN_HEIGHT - 20, 12, GREEN);
    } else {
        DrawText("Hero sprite: FAILED to load assets/heroes.png", 10, SCREEN_HEIGHT - 20, 12, RED);
    }
    
    if (game.gameOver) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, 48, RED);
        DrawText(TextFormat("You reached level %d", game.player.level), 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 20, WHITE);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 30, 20, WHITE);
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Roguelike RPG");
    SetTargetFPS(60);
    
    InitGame();
    
    while (!WindowShouldClose()) {
        if (!game.gameOver) {
            UpdateGame();
        } else if (IsKeyPressed(KEY_R)) {
            UnloadSprites();
            InitGame();
        }
        
        DrawGame();
    }
    
    UnloadSprites();
    CloseWindow();
    return 0;
}
