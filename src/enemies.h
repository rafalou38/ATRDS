#pragma once
#include "shared.h"

enum EnemyType
{
    ENEMY_TUX
};

struct Enemy
{
    enum EnemyType type;
    float hp;
    float maxHP;
    float speed;

    // Indice intentionnellement flottant pour permette un positionnement dynamique dans chaque cellule
    float grid_x;
    float grid_y;
};

typedef struct EnemyPool
{
    int length; // Taille du tableau
    int count;  // Nombre d’ennemis dans le tableau
    struct Enemy **enemies;
} EnemyPool;

EnemyPool AllocEnemyPool()
{
    EnemyPool ep;
    ep.length = 0x100; // 256
    ep.count = 0x0;
    ep.enemies = (struct Enemy **)malloc(sizeof(struct Enemy *) * ep.length);
    if (ep.enemies == NULL)
    {
        printCritical("Failed to allocate enemy pool");
        exit(EXIT_FAILURE);
    }

    return ep;
}

void freeEnemyPool(EnemyPool ep)
{
    printf("\033[38;5;243m $\033[0m Freeing \033[38;5;11;1m%d\033[0m enemies:\t", ep.count);
    for (int i = 0; i < ep.count; i++)
    {
        free(ep.enemies[i]);
    }

    free(ep.enemies);
    printf("\033[38;5;42m Done \033[0m\n");
}


void addEnemy(EnemyPool ep, enum EnemyType type, int start_x, int start_y)
{
    if (ep.count < ep.length)
    {

        struct Enemy *newEnemy = malloc(sizeof(struct Enemy));

        newEnemy->hp=10;
        newEnemy->maxHP=10;
        newEnemy->type=type;
        newEnemy->grid_x = (float)start_x;
        newEnemy->grid_y = (float)start_y;

        ep.enemies[ep.count] = newEnemy;
        ep.count++;
    }
    else
    {
        printCritical("Overflow not yet implemented\n");
    }
}

// void drawEnemies(EnemyPool ep){

// }