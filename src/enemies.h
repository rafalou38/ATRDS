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

void addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y)
{
    if (ep->count < ep->length)
    {

        struct Enemy *newEnemy = malloc(sizeof(struct Enemy));

        newEnemy->hp = 10;
        newEnemy->maxHP = 10;

        newEnemy->speed = 0.015;

        newEnemy->state = ENEMY_STATE_ALIVE;

        newEnemy->type = type;
        newEnemy->grid_x = (float)start_x;
        newEnemy->grid_y = (float)start_y;
        newEnemy->previous_cell = grid.cells[start_x][start_y];
        newEnemy->next_cell = grid.cells[start_x][start_y];
        newEnemy->on_last_cell = false;

        ep->enemies[ep->count] = newEnemy;
        ep->count++;
    }
    else
    {
        printCritical("Overflow not yet implemented\n");
    }
}

// Cette fonction supprime les ennemis qui ont atteint l'objectif ou sont morts et réordonne le tableau
// Elle utilise un double pointeur pour parcourir le tableau et décaler tous les éléments pour remplir les trous.
void defragEnemyPool(EnemyPool *ep)
{
    int right = 0;
    int left = -1;

    int removed = 0;

    while (right < ep->count)
    {
        if (ep->enemies[right]->state == ENEMY_STATE_ALIVE)
        {
            if (right != -1)
            {
                // The enemy being replaced is a dead one
                if (ep->enemies[left]->state != ENEMY_STATE_ALIVE)
                    free(ep->enemies[left]);
                // Else, the enemy is already moved

                ep->enemies[left] = ep->enemies[right];
                removed++;
                left++;
            }
        }
        else if (left == -1)
        {
            left = right;
        }

        right++;
    }
    ep->count -= right - left;
}

void drawEnemies(EnemyPool ep, Grid grid)
{
    // Path must be cleared beforehand

    for (int i = 0; i < ep.count; i++)
    {
        struct Enemy *enemy = ep.enemies[i];
        if (enemy->state != ENEMY_STATE_ALIVE)
        {
            continue;
        }

        int cx = (size_t)enemy->grid_x;
        int cy = (size_t)enemy->grid_y;

        assert(cx >= 0);
        assert(cx < grid.width);
        assert(cy >= 0);
        assert(cy < grid.height);

        struct Cell cell = grid.cells[cx][cy];

        int terminal_x = (cell.x * (CELL_WIDTH + GAP) + 3);
        int terminal_y = (cell.y * (CELL_HEIGHT + GAP / 2) + 2);

        if (enemy->type == ENEMY_TUX)
        {
            move_to(terminal_x + CELL_WIDTH * (enemy->grid_x - (int)enemy->grid_x), terminal_y + CELL_HEIGHT * (enemy->grid_y - (int)enemy->grid_y));
            printf("🐧");
        }
    }
}

void updateEnemies(EnemyPool *ep, Grid grid)
{
    bool defragNeeded = false;
    // Walk
    for (int i = 0; i < ep->count; i++)
    {
        struct Enemy *enemy = ep->enemies[i];
        if (enemy->state != ENEMY_STATE_ALIVE)
        {
            continue;
        }

        struct Cell cell = grid.cells[(size_t)enemy->grid_x][(size_t)enemy->grid_y];

        if (cell.x == enemy->next_cell.x && cell.y == enemy->next_cell.y)
        {
            // Déterminer la prochaine cellule, vers laquelle l’ennemi doit marcher

            if (cell.x + 1 < grid.width                           //
                && enemy->previous_cell.x != cell.x + 1           //
                && grid.cells[cell.x + 1][cell.y].type == CHEMIN) // droite
                enemy->next_cell = grid.cells[cell.x + 1][cell.y];

            else if (cell.x > 0                                        //
                     && enemy->previous_cell.x != cell.x - 1           //
                     && grid.cells[cell.x - 1][cell.y].type == CHEMIN) // gauche
                enemy->next_cell = grid.cells[cell.x - 1][cell.y];

            else if (cell.y > 0                                        //
                     && enemy->previous_cell.y != cell.y - 1           //
                     && grid.cells[cell.x][cell.y - 1].type == CHEMIN) // haut
                enemy->next_cell = grid.cells[cell.x][cell.y - 1];

            else if (cell.y + 1 < grid.height                          //
                     && enemy->previous_cell.y != cell.y + 1           //
                     && grid.cells[cell.x][cell.y + 1].type == CHEMIN) // bas
                enemy->next_cell = grid.cells[cell.x][cell.y + 1];
            else if (cell.x == grid.width - 1)
                enemy->on_last_cell = true;
            else
                return;

            enemy->previous_cell = cell;
        }

        // float rand_factor = 0.5;
        float rand_factor = 0.4 + ((float)rand() / RAND_MAX) * 0.3;

        // Le déplacement se fait dans le repère orthonormé de la grille, indépendant de la dimension du terminal.
        // L'écart est calculé sous forme de vecteur normalisé puis multiplié par la vitesse pour obtenir un déplacement convenable.

        float dx = (enemy->next_cell.x + rand_factor) - enemy->grid_x;
        float dy = (enemy->next_cell.y + rand_factor) - enemy->grid_y;

        if (enemy->on_last_cell)
        {
            dx = 1;
        }

        float norme = sqrtf(dx * dx + dy * dy);
        dx /= norme;
        dy /= norme;

        dx *= enemy->speed;
        dy *= enemy->speed;

        enemy->grid_x += dx;
        enemy->grid_y += dy;

        if (enemy->grid_x >= grid.width)
        {
            enemy->state = ENEMY_STATE_ARRIVED;
            defragNeeded = true;
        }
    }

    if (defragNeeded)
    {
        defragEnemyPool(ep);
    }
}