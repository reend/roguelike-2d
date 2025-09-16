#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define GRID_WIDTH 20
#define GRID_HEIGHT 15
#define CELL_SIZE 32
#define MAX_ENEMIES 20
#define MAX_TOWERS 50
#define MAX_BULLETS 100

typedef struct {
    float x, y;
} Vector2f;

typedef struct {
    Vector2f position;
    float hp;
    float maxHp;
    int pathIndex;
    bool active;
} Enemy;

typedef struct {
    Vector2f position;
    float range;
    float damage;
    float fireRate;
    float lastShot;
    bool active;
} Tower;

typedef struct {
    Vector2f position;
    Vector2f velocity;
    float damage;
    int targetId;
    bool active;
} Bullet;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    Tower towers[MAX_TOWERS];
    Bullet bullets[MAX_BULLETS];
    Vector2f path[10];
    int pathLength;
    int playerHp;
    int score;
    float enemySpawnTimer;
    int wave;
} GameState;

GameState game = {0};

Vector2f pathPoints[] = {
    {0, 7}, {3, 7}, {3, 3}, {8, 3}, {8, 10}, {12, 10}, 
    {12, 5}, {16, 5}, {16, 12}, {19, 12}
};

void InitGame(void) {
    game.playerHp = 10;
    game.score = 0;
    game.wave = 1;
    game.enemySpawnTimer = 0;
    game.pathLength = 10;
    
    for (int i = 0; i < game.pathLength; i++) {
        game.path[i] = pathPoints[i];
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        game.towers[i].active = false;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        game.bullets[i].active = false;
    }
}

void SpawnEnemy(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) {
            game.enemies[i].position.x = game.path[0].x * CELL_SIZE + CELL_SIZE/2;
            game.enemies[i].position.y = game.path[0].y * CELL_SIZE + CELL_SIZE/2;
            game.enemies[i].hp = 50 + (game.wave * 10);
            game.enemies[i].maxHp = game.enemies[i].hp;
            game.enemies[i].pathIndex = 0;
            game.enemies[i].active = true;
            break;
        }
    }
}

void PlaceTower(int gridX, int gridY) {
    if (gridX < 0 || gridX >= GRID_WIDTH || gridY < 0 || gridY >= GRID_HEIGHT) return;
    
    bool pathBlocked = false;
    for (int i = 0; i < game.pathLength; i++) {
        if ((int)game.path[i].x == gridX && (int)game.path[i].y == gridY) {
            pathBlocked = true;
            break;
        }
    }
    if (pathBlocked) return;
    
    bool towerExists = false;
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (game.towers[i].active && 
            (int)(game.towers[i].position.x / CELL_SIZE) == gridX && 
            (int)(game.towers[i].position.y / CELL_SIZE) == gridY) {
            towerExists = true;
            break;
        }
    }
    if (towerExists) return;
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.towers[i].active) {
            game.towers[i].position.x = gridX * CELL_SIZE + CELL_SIZE/2;
            game.towers[i].position.y = gridY * CELL_SIZE + CELL_SIZE/2;
            game.towers[i].range = 80;
            game.towers[i].damage = 25;
            game.towers[i].fireRate = 1.0f;
            game.towers[i].lastShot = 0;
            game.towers[i].active = true;
            break;
        }
    }
}

void FireBullet(int towerId, int enemyId) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game.bullets[i].active) {
            game.bullets[i].position = game.towers[towerId].position;
            
            Vector2f direction = {
                game.enemies[enemyId].position.x - game.towers[towerId].position.x,
                game.enemies[enemyId].position.y - game.towers[towerId].position.y
            };
            
            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
            game.bullets[i].velocity.x = (direction.x / length) * 200;
            game.bullets[i].velocity.y = (direction.y / length) * 200;
            
            game.bullets[i].damage = game.towers[towerId].damage;
            game.bullets[i].targetId = enemyId;
            game.bullets[i].active = true;
            break;
        }
    }
}

void UpdateGame(void) {
    float deltaTime = GetFrameTime();
    
    game.enemySpawnTimer += deltaTime;
    if (game.enemySpawnTimer > 2.0f) {
        SpawnEnemy();
        game.enemySpawnTimer = 0;
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        Vector2f target = {
            game.path[game.enemies[i].pathIndex].x * CELL_SIZE + CELL_SIZE/2,
            game.path[game.enemies[i].pathIndex].y * CELL_SIZE + CELL_SIZE/2
        };
        
        Vector2f direction = {
            target.x - game.enemies[i].position.x,
            target.y - game.enemies[i].position.y
        };
        
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        
        if (distance < 5.0f) {
            game.enemies[i].pathIndex++;
            if (game.enemies[i].pathIndex >= game.pathLength) {
                game.playerHp--;
                game.enemies[i].active = false;
                continue;
            }
        } else {
            float speed = 50.0f;
            game.enemies[i].position.x += (direction.x / distance) * speed * deltaTime;
            game.enemies[i].position.y += (direction.y / distance) * speed * deltaTime;
        }
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.towers[i].active) continue;
        
        game.towers[i].lastShot += deltaTime;
        
        if (game.towers[i].lastShot >= game.towers[i].fireRate) {
            int closestEnemy = -1;
            float closestDistance = game.towers[i].range;
            
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (!game.enemies[j].active) continue;
                
                float dx = game.enemies[j].position.x - game.towers[i].position.x;
                float dy = game.enemies[j].position.y - game.towers[i].position.y;
                float distance = sqrtf(dx * dx + dy * dy);
                
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestEnemy = j;
                }
            }
            
            if (closestEnemy != -1) {
                FireBullet(i, closestEnemy);
                game.towers[i].lastShot = 0;
            }
        }
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game.bullets[i].active) continue;
        
        game.bullets[i].position.x += game.bullets[i].velocity.x * deltaTime;
        game.bullets[i].position.y += game.bullets[i].velocity.y * deltaTime;
        
        if (game.bullets[i].targetId >= 0 && game.enemies[game.bullets[i].targetId].active) {
            float dx = game.bullets[i].position.x - game.enemies[game.bullets[i].targetId].position.x;
            float dy = game.bullets[i].position.y - game.enemies[game.bullets[i].targetId].position.y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance < 8.0f) {
                game.enemies[game.bullets[i].targetId].hp -= game.bullets[i].damage;
                
                if (game.enemies[game.bullets[i].targetId].hp <= 0) {
                    game.score += 10;
                    game.enemies[game.bullets[i].targetId].active = false;
                }
                
                game.bullets[i].active = false;
            }
        }
        
        if (game.bullets[i].position.x < 0 || game.bullets[i].position.x > SCREEN_WIDTH ||
            game.bullets[i].position.y < 0 || game.bullets[i].position.y > SCREEN_HEIGHT) {
            game.bullets[i].active = false;
        }
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int gridX = (int)(mousePos.x / CELL_SIZE);
        int gridY = (int)(mousePos.y / CELL_SIZE);
        PlaceTower(gridX, gridY);
    }
}

void DrawGame(void) {
    BeginDrawing();
    ClearBackground(DARKGREEN);
    
    for (int x = 0; x <= GRID_WIDTH; x++) {
        DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, DARKGRAY);
    }
    for (int y = 0; y <= GRID_HEIGHT; y++) {
        DrawLine(0, y * CELL_SIZE, GRID_WIDTH * CELL_SIZE, y * CELL_SIZE, DARKGRAY);
    }
    
    for (int i = 0; i < game.pathLength; i++) {
        int x = game.path[i].x * CELL_SIZE;
        int y = game.path[i].y * CELL_SIZE;
        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, BROWN);
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.towers[i].active) continue;
        
        DrawRectangle(
            game.towers[i].position.x - CELL_SIZE/2,
            game.towers[i].position.y - CELL_SIZE/2,
            CELL_SIZE, CELL_SIZE, GREEN
        );
        
        DrawCircleLines(
            game.towers[i].position.x,
            game.towers[i].position.y,
            game.towers[i].range, LIGHTGRAY
        );
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        DrawCircle(game.enemies[i].position.x, game.enemies[i].position.y, 12, RED);
        
        float hpPercent = game.enemies[i].hp / game.enemies[i].maxHp;
        DrawRectangle(
            game.enemies[i].position.x - 15,
            game.enemies[i].position.y - 20,
            30, 4, DARKGRAY
        );
        DrawRectangle(
            game.enemies[i].position.x - 15,
            game.enemies[i].position.y - 20,
            30 * hpPercent, 4, RED
        );
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game.bullets[i].active) continue;
        DrawCircle(game.bullets[i].position.x, game.bullets[i].position.y, 3, YELLOW);
    }
    
    DrawText(TextFormat("HP: %d", game.playerHp), GRID_WIDTH * CELL_SIZE + 10, 20, 20, WHITE);
    DrawText(TextFormat("Score: %d", game.score), GRID_WIDTH * CELL_SIZE + 10, 50, 20, WHITE);
    DrawText(TextFormat("Wave: %d", game.wave), GRID_WIDTH * CELL_SIZE + 10, 80, 20, WHITE);
    DrawText("Click to place tower", GRID_WIDTH * CELL_SIZE + 10, 120, 16, WHITE);
    
    if (game.playerHp <= 0) {
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 36, RED);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 40, 20, WHITE);
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Tower Defense");
    SetTargetFPS(60);
    
    InitGame();
    
    while (!WindowShouldClose()) {
        if (game.playerHp > 0) {
            UpdateGame();
        } else if (IsKeyPressed(KEY_R)) {
            InitGame();
        }
        
        DrawGame();
    }
    
    CloseWindow();
    return 0;
}
