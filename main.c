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
#define PI 3.14159265f

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
    Vector2f targetPos;
    Vector2f velocity;
    float range;
    float damage;
    float fireRate;
    float lastShot;
    float speed;
    int team;
    bool selected;
    bool active;
    bool moving;
} Unit;

typedef struct {
    Vector2f position;
    Vector2f velocity;
    float damage;
    int targetId;
    bool active;
} Bullet;

typedef struct {
    Vector2f position;
    int team;
    float hp;
    float maxHp;
    bool active;
} Building;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    Unit units[MAX_TOWERS];
    Building buildings[10];
    Bullet bullets[MAX_BULLETS];
    Vector2f path[10];
    int pathLength;
    int playerHp;
    int gold;
    int wood;
    float enemySpawnTimer;
    int wave;
    Vector2f selectionStart;
    Vector2f selectionEnd;
    bool selecting;
} GameState;

GameState game = {0};

Vector2f pathPoints[] = {
    {0, 7}, {3, 7}, {3, 3}, {8, 3}, {8, 10}, {12, 10}, 
    {12, 5}, {16, 5}, {16, 12}, {19, 12}
};

void InitGame(void) {
    game.playerHp = 10;
    game.gold = 500;
    game.wood = 300;
    game.wave = 1;
    game.enemySpawnTimer = 0;
    game.pathLength = 10;
    game.selecting = false;
    
    for (int i = 0; i < game.pathLength; i++) {
        game.path[i] = pathPoints[i];
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].active = false;
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        game.units[i].active = false;
        game.units[i].selected = false;
    }
    
    for (int i = 0; i < 10; i++) {
        game.buildings[i].active = false;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        game.bullets[i].active = false;
    }
    
    game.buildings[0].position.x = 2 * CELL_SIZE + CELL_SIZE/2;
    game.buildings[0].position.y = 2 * CELL_SIZE + CELL_SIZE/2;
    game.buildings[0].team = 0;
    game.buildings[0].hp = 1000;
    game.buildings[0].maxHp = 1000;
    game.buildings[0].active = true;
    
    game.buildings[1].position.x = 17 * CELL_SIZE + CELL_SIZE/2;
    game.buildings[1].position.y = 12 * CELL_SIZE + CELL_SIZE/2;
    game.buildings[1].team = 1;
    game.buildings[1].hp = 1000;
    game.buildings[1].maxHp = 1000;
    game.buildings[1].active = true;
    
    for (int i = 0; i < 3; i++) {
        game.units[i].position.x = (3 + i) * CELL_SIZE + CELL_SIZE/2;
        game.units[i].position.y = 4 * CELL_SIZE + CELL_SIZE/2;
        game.units[i].targetPos = game.units[i].position;
        game.units[i].range = 80;
        game.units[i].damage = 25;
        game.units[i].fireRate = 1.0f;
        game.units[i].lastShot = 0;
        game.units[i].speed = 60.0f;
        game.units[i].team = 0;
        game.units[i].selected = false;
        game.units[i].active = true;
        game.units[i].moving = false;
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

void SelectUnits(Vector2f start, Vector2f end) {
    float minX = fminf(start.x, end.x);
    float maxX = fmaxf(start.x, end.x);
    float minY = fminf(start.y, end.y);
    float maxY = fmaxf(start.y, end.y);
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.units[i].active || game.units[i].team != 0) continue;
        
        if (game.units[i].position.x >= minX && game.units[i].position.x <= maxX &&
            game.units[i].position.y >= minY && game.units[i].position.y <= maxY) {
            game.units[i].selected = true;
        }
    }
}

void MoveSelectedUnits(Vector2f target) {
    int selectedCount = 0;
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (game.units[i].active && game.units[i].selected && game.units[i].team == 0) {
            selectedCount++;
        }
    }
    
    if (selectedCount == 0) return;
    
    float angle = 0;
    float radius = 20;
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (game.units[i].active && game.units[i].selected && game.units[i].team == 0) {
            float offsetX = cosf(angle) * radius;
            float offsetY = sinf(angle) * radius;
            
            game.units[i].targetPos.x = target.x + offsetX;
            game.units[i].targetPos.y = target.y + offsetY;
            game.units[i].moving = true;
            
            angle += (2.0f * PI) / selectedCount;
        }
    }
}

void FireBullet(int unitId, int enemyId) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game.bullets[i].active) {
            game.bullets[i].position = game.units[unitId].position;
            
            Vector2f direction = {
                game.enemies[enemyId].position.x - game.units[unitId].position.x,
                game.enemies[enemyId].position.y - game.units[unitId].position.y
            };
            
            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
            game.bullets[i].velocity.x = (direction.x / length) * 200;
            game.bullets[i].velocity.y = (direction.y / length) * 200;
            
            game.bullets[i].damage = game.units[unitId].damage;
            game.bullets[i].targetId = enemyId;
            game.bullets[i].active = true;
            break;
        }
    }
}

void UpdateGame(void) {
    float deltaTime = GetFrameTime();
    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!IsKeyDown(KEY_LEFT_SHIFT)) {
            for (int i = 0; i < MAX_TOWERS; i++) {
                game.units[i].selected = false;
            }
        }
        game.selectionStart.x = mousePos.x;
        game.selectionStart.y = mousePos.y;
        game.selecting = true;
    }
    
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && game.selecting) {
        game.selectionEnd.x = mousePos.x;
        game.selectionEnd.y = mousePos.y;
    }
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && game.selecting) {
        game.selectionEnd.x = mousePos.x;
        game.selectionEnd.y = mousePos.y;
        SelectUnits(game.selectionStart, game.selectionEnd);
        game.selecting = false;
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2f target = {mousePos.x, mousePos.y};
        MoveSelectedUnits(target);
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.units[i].active) continue;
        
        if (game.units[i].moving) {
            Vector2f direction = {
                game.units[i].targetPos.x - game.units[i].position.x,
                game.units[i].targetPos.y - game.units[i].position.y
            };
            
            float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
            
            if (distance < 5.0f) {
                game.units[i].moving = false;
                game.units[i].position = game.units[i].targetPos;
            } else {
                game.units[i].position.x += (direction.x / distance) * game.units[i].speed * deltaTime;
                game.units[i].position.y += (direction.y / distance) * game.units[i].speed * deltaTime;
            }
        }
        
        game.units[i].lastShot += deltaTime;
        
        if (game.units[i].lastShot >= game.units[i].fireRate) {
            int closestEnemy = -1;
            float closestDistance = game.units[i].range;
            
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (!game.enemies[j].active) continue;
                
                float dx = game.enemies[j].position.x - game.units[i].position.x;
                float dy = game.enemies[j].position.y - game.units[i].position.y;
                float distance = sqrtf(dx * dx + dy * dy);
                
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestEnemy = j;
                }
            }
            
            for (int j = 0; j < MAX_TOWERS; j++) {
                if (!game.units[j].active || game.units[j].team == game.units[i].team) continue;
                
                float dx = game.units[j].position.x - game.units[i].position.x;
                float dy = game.units[j].position.y - game.units[i].position.y;
                float distance = sqrtf(dx * dx + dy * dy);
                
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestEnemy = j + MAX_ENEMIES;
                }
            }
            
            if (closestEnemy != -1) {
                if (closestEnemy < MAX_ENEMIES) {
                    FireBullet(i, closestEnemy);
                }
                game.units[i].lastShot = 0;
            }
        }
    }
    
    game.enemySpawnTimer += deltaTime;
    if (game.enemySpawnTimer > 3.0f) {
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
                    game.gold += 20;
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
    
    for (int i = 0; i < 10; i++) {
        if (!game.buildings[i].active) continue;
        
        Color buildingColor = game.buildings[i].team == 0 ? BLUE : MAROON;
        DrawRectangle(
            game.buildings[i].position.x - CELL_SIZE,
            game.buildings[i].position.y - CELL_SIZE,
            CELL_SIZE * 2, CELL_SIZE * 2, buildingColor
        );
        
        float hpPercent = game.buildings[i].hp / game.buildings[i].maxHp;
        DrawRectangle(
            game.buildings[i].position.x - CELL_SIZE,
            game.buildings[i].position.y - CELL_SIZE - 8,
            CELL_SIZE * 2, 6, DARKGRAY
        );
        DrawRectangle(
            game.buildings[i].position.x - CELL_SIZE,
            game.buildings[i].position.y - CELL_SIZE - 8,
            (CELL_SIZE * 2) * hpPercent, 6, GREEN
        );
    }
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!game.units[i].active) continue;
        
        Color unitColor = game.units[i].team == 0 ? GREEN : RED;
        
        if (game.units[i].selected) {
            DrawCircleLines(game.units[i].position.x, game.units[i].position.y, 20, YELLOW);
        }
        
        DrawCircle(game.units[i].position.x, game.units[i].position.y, 8, unitColor);
        
        if (game.units[i].moving && game.units[i].selected) {
            DrawCircleLines(game.units[i].targetPos.x, game.units[i].targetPos.y, 5, WHITE);
            DrawLine(game.units[i].position.x, game.units[i].position.y,
                    game.units[i].targetPos.x, game.units[i].targetPos.y, WHITE);
        }
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game.enemies[i].active) continue;
        
        DrawCircle(game.enemies[i].position.x, game.enemies[i].position.y, 12, PURPLE);
        
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
    
    if (game.selecting) {
        float x = fminf(game.selectionStart.x, game.selectionEnd.x);
        float y = fminf(game.selectionStart.y, game.selectionEnd.y);
        float width = fabsf(game.selectionEnd.x - game.selectionStart.x);
        float height = fabsf(game.selectionEnd.y - game.selectionStart.y);
        
        DrawRectangleLines(x, y, width, height, WHITE);
        DrawRectangle(x, y, width, height, Fade(WHITE, 0.1f));
    }
    
    DrawText(TextFormat("Gold: %d", game.gold), GRID_WIDTH * CELL_SIZE + 10, 20, 20, GOLD);
    DrawText(TextFormat("Wood: %d", game.wood), GRID_WIDTH * CELL_SIZE + 10, 50, 20, BROWN);
    DrawText(TextFormat("HP: %d", game.playerHp), GRID_WIDTH * CELL_SIZE + 10, 80, 20, WHITE);
    DrawText(TextFormat("Wave: %d", game.wave), GRID_WIDTH * CELL_SIZE + 10, 110, 20, WHITE);
    
    DrawText("Controls:", GRID_WIDTH * CELL_SIZE + 10, 150, 16, WHITE);
    DrawText("LMB - Select units", GRID_WIDTH * CELL_SIZE + 10, 170, 14, LIGHTGRAY);
    DrawText("RMB - Move selected", GRID_WIDTH * CELL_SIZE + 10, 190, 14, LIGHTGRAY);
    DrawText("Shift+LMB - Add to selection", GRID_WIDTH * CELL_SIZE + 10, 210, 14, LIGHTGRAY);
    
    if (game.playerHp <= 0) {
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 36, RED);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 40, 20, WHITE);
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple RTS Strategy");
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
