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
};

typedef struct EnemyPool
{
    int length; // Taille du tableau
    int size;   // Nombre d’ennemis dans le tableau
    struct Enemy **enemies;
} EnemyPool;

EnemyPool AllocEnemyPool()
{
    EnemyPool ep;
    ep.length = 0x100; // 256
    ep.size = 0x0;
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
    printf("\033[38;5;243m $\033[0m Freeing \033[38;5;11;1m%d\033[0m enemies:\t\t", ep.size);
    for (size_t i = 0; i < ep.size; i++)
    {
        free(ep.enemies[i]);
    }

    free(ep.enemies);
    printf("\033[38;5;42m Done \033[0m\n");
}