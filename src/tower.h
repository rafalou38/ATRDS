#pragma once

#include "shared.h"
#include "enemies.h"
// true: les balles tirées par les tourelles seront affichées
#define BULLETS_ON false

#if BULLETS_ON

struct Bullet
{
    struct Enemy *target;
    float grid_x;
    float grid_y;
    float damage;
    float hit;
};
typedef struct BulletPool
{
    struct Bullet bullets[0xff];
    int count;
} BulletPool;

#endif

void showTowerSelection(int ligne, bool hasTurret, struct Turret selected_tower);
void showSaveScreen(int ligne);
struct Turret getTurretStruct(enum TurretType type);
int getTurretPrice(enum TurretType type, int level);

#if BULLETS_ON
void updateTowers(Grid grid, EnemyPool ep, BulletPool *bp, float dt, GameStats *gs);
void drawBullets(BulletPool bp);
void updateBullets(BulletPool *bp, float dt);
#else
void updateTowers(Grid grid, EnemyPool ep, float dt, GameStats *gs);
#endif