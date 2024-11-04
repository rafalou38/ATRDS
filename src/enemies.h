#pragma once

#include "shared.h"
#include "grid.h"

//Enumérations et structures des éléments liées aux ennemis

enum EnemyType 
{
    ENEMY_TUX,
    ENEMY_SPEED,
    ENEMY_BOSS
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
    int damage;
    int money;

    enum EnemyState state;

    bool on_last_cell;
    struct Cell next_cell;
    struct Cell previous_cell;

    bool has_effect;
    enum EffectType effet;
    float puissance_effet;
    float temps_recharge;
    float last_hit;
    float temps_rest;

    // Indice intentionnellement flottant pour permette un positionnement dynamique dans chaque cellule
    float grid_x;
    float grid_y;

    float term_x;
    float term_y;
};


EnemyPool AllocEnemyPool();
void freeEnemyPool(EnemyPool ep);
struct Enemy defEnemy(Grid grid, enum EnemyType type, int start_x, int start_y);
void addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y);
void defragEnemyPool(EnemyPool *ep);
void drawEnemies(EnemyPool ep, Grid grid);
void updateEnemies(EnemyPool *ep, Grid grid, GameStats *gs, Labels *labels, float dt_sec);