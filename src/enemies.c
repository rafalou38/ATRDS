#include "enemies.h"

EnemyPool AllocEnemyPool()
{
    EnemyPool ep;
    ep.length = 0xff; // 256
    ep.count = 0x0;
    ep.enemies = (struct Enemy *)malloc(sizeof(struct Enemy) * ep.length);
    if (ep.enemies == NULL)
    {
        printCritical("Failed to allocate enemy pool");
        exit(EXIT_FAILURE);
    }

    return ep;
}

void freeEnemyPool(EnemyPool ep)
{
    printf(COLOR_GRAY);
    printf(" $ ");
    printf(RESET);
    printf("Freeing %s%d%s enemies:\t", COLOR_YELLOW, ep.count, RESET);

    free(ep.enemies);
    printf("%s Done %s\n", COLOR_GREEN, RESET);
}

void addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y)
{
    if (ep->count < ep->length)
    {

        struct Enemy newEnemy;

        newEnemy.hp = 10;
        newEnemy.maxHP = 10;

        newEnemy.speed = 0.8f;

        newEnemy.state = ENEMY_STATE_ALIVE;

        newEnemy.type = type;
        newEnemy.grid_x = (float)start_x;
        newEnemy.grid_y = (float)start_y;
        newEnemy.previous_cell = grid.cells[start_x][start_y];
        newEnemy.next_cell = grid.cells[start_x][start_y];
        newEnemy.on_last_cell = false;

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

    while (right < ep->count)
    {
        if (ep->enemies[right].state == ENEMY_STATE_ALIVE)
        {
            if (left != -1)
            {
                assert(left >= 0);
                assert(left < ep->length);

                ep->enemies[left] = ep->enemies[right];
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
        struct Enemy *enemy = &(ep.enemies[i]);
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
        int px = terminal_x + (CELL_WIDTH + GAP) * (enemy->grid_x - (int)enemy->grid_x);
        int py = terminal_y + (CELL_HEIGHT + GAP / 2) * (enemy->grid_y - (int)enemy->grid_y);

        printf(COLOR_STANDARD_BG);
        if (enemy->type == ENEMY_TUX)
        {
            move_to(px, py);
            printf("🦍");
        }

        // move_to((enemy->next_cell.x * (CELL_WIDTH + GAP) + 3), (enemy->next_cell.y * (CELL_HEIGHT + GAP / 2) + 2));
        // printf("N%d", i);
        // move_to((enemy->previous_cell.x * (CELL_WIDTH + GAP) + 3), (enemy->previous_cell.y * (CELL_HEIGHT + GAP / 2) + 2));
        // printf("P%d", i);

        move_to(px - 1, py - 1);

        printf(COLOR_HEALTH_BG);
        float ratio_tot = enemy->hp / (float)enemy->maxHP;
        if (ratio_tot >= 0.75)
            printf(COLOR_HEALTH_75);
        else if (ratio_tot >= 0.5)
            printf(COLOR_HEALTH_50);
        else if (ratio_tot >= 0.25)
            printf(COLOR_HEALTH_25);
        else
            printf(COLOR_HEALTH_0);
        for (int i = 0; i < 4; i++)
        {
            float ratio = enemy->hp / (float)enemy->maxHP;

            float rest = ratio * 4 - i;

            if (rest >= 1)
                printf("█");
            else if (rest >= 6.0f / 8.0f)
                printf("▉");
            else if (rest >= 3.0f / 4.0f)
                printf("▊");
            else if (rest >= 5.0f / 8.0f)
                printf("▋");
            else if (rest >= 1.0f / 2.0f)
                printf("▌");
            else if (rest >= 3.0f / 8.0f)
                printf("▍");
            else if (rest >= 1.0f / 4.0f)
                printf("▎");
            else if (rest >= 1.0f / 8.0f)
                printf("▏");
            else
                printf(" ");
        }
        printf(RESET);
    }
}

void updateEnemies(EnemyPool *ep, Grid grid, GameStats *gs, Labels *labels, float dt_sec)
{
    bool defragNeeded = false;
    // Walk
    for (int i = 0; i < ep->count; i++)
    {
        struct Enemy *enemy = &(ep->enemies[i]);
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

        float rand_factor = 0.5;
        // float rand_factor = 0.4 + ((float)rand() / RAND_MAX) * 0.2;

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

        dx *= enemy->speed * dt_sec;
        dy *= enemy->speed * dt_sec;

        enemy->grid_x += dx;
        enemy->grid_y += dy;

        if (enemy->grid_x >= grid.width)
        {
            enemy->state = ENEMY_STATE_ARRIVED;
            defragNeeded = true;
            gs->health -= 1;
        }

        if (enemy->hp <= 0)
        {
            enemy->state = ENEMY_STATE_DEAD;
            drawCell(enemy->previous_cell, grid);
            defragNeeded = true;
            gs->cash += 1;
            labels->labels[labels->count].counter = 0;
            labels->labels[labels->count].duration = 2;
            labels->labels[labels->count].text = COLOR_STANDARD_BG COLOR_YELLOW "+1€" RESET;
            labels->labels[labels->count].x = (enemy->grid_x * (CELL_WIDTH + GAP) + 3);
            labels->labels[labels->count].y = (enemy->grid_y * (CELL_HEIGHT + GAP / 2) + 2);
            labels->count++;
        }
    }

    if (defragNeeded)
    {
        defragEnemyPool(ep);
    }
}