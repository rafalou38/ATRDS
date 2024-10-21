#pragma once

#include "shared.h"
#include "grid.h"

enum EnemyType
{
    ENEMY_TUX
};

enum EnemyState
{
    ENEMY_STATE_ALIVE,
    ENEMY_STATE_DEAD,
    ENEMY_STATE_ARRIVED
};

struct Enemy
{
    enum EnemyType type;
    float hp;
    float maxHP;
    float speed;

    enum EnemyState state;

    bool on_last_cell;
    struct Cell next_cell;
    struct Cell previous_cell;

    // Indice intentionnellement flottant pour permette un positionnement dynamique dans chaque cellule
    float grid_x;
    float grid_y;

    float term_x;
    float term_y;
};


EnemyPool AllocEnemyPool();
void freeEnemyPool(EnemyPool ep);
void addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y);
void defragEnemyPool(EnemyPool *ep);
void drawEnemies(EnemyPool ep, Grid grid);
void updateEnemies(EnemyPool *ep, Grid grid, float dt_sec);