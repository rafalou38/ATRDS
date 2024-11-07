#include "enemies.h"

EnemyPool AllocEnemyPool() // pré-Alloue le tableau d’ennemis avec une taille initiale prédéterminée
{
    EnemyPool ep;
    // Correspond au nombre maximal d’ennemis pouvant exister simultanément
    ep.length = 255;
    // Correspond au nombre d’ennemis en vie et actifs.
    ep.count = 0;
    ep.enemies = (struct Enemy *)malloc(sizeof(struct Enemy) * ep.length);
    if (ep.enemies == NULL)
    {
        printCritical("Failed to allocate enemy pool");
        exit(EXIT_FAILURE);
    }

    return ep;
}

void freeEnemyPool(EnemyPool ep) // On libère le tableau des enemies.
{
    printf(COLOR_GRAY);
    printf(" $ ");
    printf(RESET);
    printf("Freeing %s%d%s enemies:\t", COLOR_YELLOW, ep.count, RESET);

    free(ep.enemies);
    printf("%s Done %s\n", COLOR_GREEN, RESET);
}

// Definitions des différents types d'ennemis et de leurs caractéristiques
struct Enemy defEnemy(Grid grid, enum EnemyType type, int start_x, int start_y)
{
    struct Enemy enemy;
    enemy.has_effect = false;
    enemy.is_boss = false;
    if (type == ENEMY_TUX) // Ennemi de base
    {
        enemy.type = ENEMY_TUX;
        enemy.hp = 10;
        enemy.maxHP = 10;
        enemy.speed = 0.8f;
        enemy.damage = 1;
        enemy.money = 1;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_SPEED) // Ennemi rapide
    {
        enemy.type = ENEMY_SPEED;
        enemy.hp = 5;
        enemy.maxHP = 5;
        enemy.speed = 1.5f;
        enemy.damage = 1;
        enemy.money = 2;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_SLIME_BOSS) // Boss ennemi
    {
        enemy.type = ENEMY_SLIME_BOSS;
        enemy.is_boss = true;
        enemy.hp = 50;
        enemy.maxHP = 50;
        enemy.speed = 0.5f;
        enemy.damage = 5;
        enemy.money = 5;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_SPIDER)
    {
        enemy.type = ENEMY_SPIDER;
        enemy.hp = 20;
        enemy.maxHP = 20;
        enemy.speed = 1.0f;
        enemy.damage = 2;
        enemy.money = 1;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_HYPERSPEED)
    {
        enemy.type = ENEMY_HYPERSPEED;
        enemy.hp = 10;
        enemy.maxHP = 10;
        enemy.speed = 2.0f;
        enemy.damage = 2;
        enemy.money = 2;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_HIGHTUX)
    {
        enemy.type = ENEMY_HIGHTUX;
        enemy.hp = 15;
        enemy.maxHP = 15;
        enemy.speed = 1.0f;
        enemy.damage = 2;
        enemy.money = 2;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_TANK)
    {
        enemy.type = ENEMY_TANK;
        enemy.is_boss = true;
        enemy.hp = 1000;
        enemy.maxHP = 1000;
        enemy.speed = 0.2f;
        enemy.damage = 20;
        enemy.money = 30;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    else if (type == ENEMY_BOSS_STUN)
    {
        enemy.type = ENEMY_BOSS_STUN;
        enemy.is_boss = true;
        enemy.hp = 100;
        enemy.maxHP = 100;
        enemy.speed = 0.5f;
        enemy.damage = 10;
        enemy.money = 20;
        enemy.state = ENEMY_STATE_ALIVE;
        enemy.has_effect = true;
        enemy.effet = BOSS_STUN;
        enemy.puissance_effet = 2.5;
        enemy.grid_x = (float)start_x;
        enemy.grid_y = (float)start_y;
        enemy.previous_cell = grid.cells[start_x][start_y];
        enemy.next_cell = grid.cells[start_x][start_y];
        enemy.on_last_cell = false;
    }
    return enemy;
}

struct Enemy *addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y) // Ajoute un nouvel ennemi au tableau des enemies
{
    if (ep->count < ep->length)
    {
        ep->enemies[ep->count] = defEnemy(grid, type, start_x, start_y);

        return &(ep->enemies[ep->count++]);
    }
    else
    {
        // Dans le cas ou le nombre d’ennemis dépasse la capacité du tableau
        printCritical("Overflow not yet implemented\n");
        exit(EXIT_FAILURE);

        return NULL;
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

void drawEnemies(EnemyPool ep, Grid grid) // Dessine les ennemis sur un chemin VIDE
{
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

        printf(COLOR_STANDARD_BG); // Sprites des ennemis
        if (enemy->type == ENEMY_TUX)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[4][3] =
                {{
                     COLOR_TUX_BASE "▞▀▀▚",
                     "▌" COLOR_TUX_EYES "▚ ▞" COLOR_TUX_BASE,
                     "▚▄▄▞",
                 },
                 {
                     COLOR_TUX_BASE "▞▀▀▚",
                     "▌" COLOR_TUX_EYES "▀ ▀" COLOR_TUX_BASE,
                     "▚▄▄▞",
                 },
                 {
                     COLOR_TUX_BASE "▞▀▀▚",
                     COLOR_TUX_EYES "▚ ▞" COLOR_TUX_BASE "▐",
                     "▚▄▄▞",
                 },
                 {
                     COLOR_TUX_BASE "▞▀▀▚",
                     "▌" COLOR_TUX_EYES "▄ ▄" COLOR_TUX_BASE,
                     "▚▄▄▞",
                 }};
            if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 > px)
            {
                sprite_anim = 0;
            }
            else if (enemy->next_cell.y * (CELL_HEIGHT + GAP / 2) + 2 < py)
            {
                sprite_anim = 1;
            }
            else if (enemy->next_cell.x * (CELL_HEIGHT + GAP / 2) + 2 > py)
            {
                sprite_anim = 3;
            }
            else if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 < px)
            {
                sprite_anim = 2;
            }
            for (int i = 0; i < 3; i++)
            {
                move_to(px - 1, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_SPEED)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[2][3] =
                {{
                     COLOR_SPEED_BASE "▞▄▄▚",
                     "▐" COLOR_SPEED_EYES "▐▌" COLOR_SPEED_BASE "▌",
                     "▚▀▀▞",
                 },
                 {
                     COLOR_SPEED_BASE "▞▀▀▚",
                     "▐" COLOR_SPEED_EYES "▐▌" COLOR_SPEED_BASE "▌",
                     "▚▄▄▞",
                 }};
            if (((int)enemy->grid_x % 2 == 1 && (int)enemy->grid_y % 2 == 1) //
                || ((int)enemy->grid_x % 2 == 0 && (int)enemy->grid_y % 2 == 0))
            {
                sprite_anim = 0;
            }
            else
            {
                sprite_anim = 1;
            }
            for (int i = 0; i < 3; i++)
            {
                move_to(px - 1, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_SPIDER)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[3][3] =
                {{
                     COLOR_SPIDER_BASE "▌" COLOR_SPIDER_EYES "▄▄" COLOR_SPIDER_BASE "▐",
                     COLOR_SPIDER_EYES "▐▄▄▌",
                     COLOR_SPIDER_LEGS "▞▞▚▚",
                 },
                 {
                     COLOR_SPIDER_BASE "▌" COLOR_SPIDER_EYES "▄▄" COLOR_SPIDER_BASE "▐",
                     COLOR_SPIDER_EYES "▐▄▄▌",
                     COLOR_SPIDER_LEGS "▞▞▞▚",
                 },
                 {
                     COLOR_SPIDER_BASE "▌" COLOR_SPIDER_EYES "▄▄" COLOR_SPIDER_BASE "▐",
                     COLOR_SPIDER_EYES "▐▄▄▌",
                     COLOR_SPIDER_LEGS "▞▚▚▚",
                 }};
            if ((int)enemy->grid_x % 2 == 1 && (int)enemy->grid_y % 2 == 1)
            {
                sprite_anim = 2;
            }
            else if ((int)enemy->grid_x % 2 == 0 && (int)enemy->grid_y % 2 == 0)
            {
                sprite_anim = 1;
            }
            else
            {
                sprite_anim = 0;
            }
            for (int i = 0; i < 3; i++)
            {
                move_to(px - 1, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_HYPERSPEED)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[2][3] =
                {{
                     COLOR_HYPERSPEED_BASE " ▞▄▀▀▄▚ ",
                     " ▐" COLOR_HYPERSPEED_BOX "▐" COLOR_HYPERSPEED_EYES "▞▞" COLOR_HYPERSPEED_BOX "▌" COLOR_HYPERSPEED_BASE "▌ ",
                     " ▚▀▄▄▀▞ ",
                 },
                 {
                     COLOR_HYPERSPEED_BASE " ▞▄▀▀▄▚ ",
                     " ▐" COLOR_HYPERSPEED_BOX "▐" COLOR_HYPERSPEED_EYES "▚▚" COLOR_HYPERSPEED_BOX "▌" COLOR_HYPERSPEED_BASE "▌ ",
                     " ▚▀▄▄▀▞ ",
                 }};
            if (((int)enemy->grid_x % 2 == 1 && (int)enemy->grid_y % 2 == 1) //
                || ((int)enemy->grid_x % 2 == 0 && (int)enemy->grid_y % 2 == 0))
            {
                sprite_anim = 0;
            }
            else
            {
                sprite_anim = 1;
            }
            for (int i = 0; i < 3; i++)
            {
                move_to(px - 3, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_HIGHTUX)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[4][3] =
                {{
                     COLOR_HIGHTUX_CROWN " ▞▚▞▚▞▚ ",
                     COLOR_HIGHTUX_BASE " ▌ " COLOR_HIGHTUX_EYES "▚ ▞" COLOR_HIGHTUX_BASE "▐ ",
                     COLOR_HIGHTUX_BASE " ▚▄▄▄▄▞ ",
                 },
                 {
                     COLOR_HIGHTUX_CROWN " ▞▚▞▚▞▚ ",
                     COLOR_HIGHTUX_BASE " ▌ " COLOR_HIGHTUX_EYES "▀ ▀" COLOR_HIGHTUX_BASE "▐ ",
                     COLOR_HIGHTUX_BASE " ▚▄▄▄▄▞ ",
                 },
                 {
                     COLOR_HIGHTUX_CROWN " ▞▚▞▚▞▚ ",
                     COLOR_HIGHTUX_BASE " ▌" COLOR_HIGHTUX_EYES "▚ ▞" COLOR_HIGHTUX_BASE " ▐ ",
                     COLOR_HIGHTUX_BASE " ▚▄▄▄▄▞ ",
                 },
                 {
                     COLOR_HIGHTUX_CROWN " ▞▚▞▚▞▚ ",
                     COLOR_HIGHTUX_BASE " ▌ " COLOR_HIGHTUX_EYES "▄ ▄" COLOR_HIGHTUX_BASE "▐ ",
                     COLOR_HIGHTUX_BASE " ▚▄▄▄▄▞ ",
                 }};
            if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 > px)
            {
                sprite_anim = 0;
            }
            else if (enemy->next_cell.y * (CELL_HEIGHT + GAP / 2) + 2 < py)
            {
                sprite_anim = 1;
            }
            else if (enemy->next_cell.x * (CELL_HEIGHT + GAP / 2) + 2 > py)
            {
                sprite_anim = 3;
            }
            else if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 < px)
            {
                sprite_anim = 2;
            }
            for (int i = 0; i < 3; i++)
            {
                move_to(px - 3, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_SLIME_BOSS)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[4][5] =
                {{
                     COLOR_BOSS_SLIME_CROWN "  ▞▞▞▚▚▚  ",
                     COLOR_BOSS_SLIME_BASE " ▞▀▀▀▀▀▀▚ ",
                     " ▌  " COLOR_BOSS_SLIME_EYES "▚  ▞" COLOR_BOSS_SLIME_BASE "▐ ",
                     " ▌   " COLOR_BOSS_SLIME_MOUTH "▞▚" COLOR_BOSS_SLIME_BASE " ▐ ",
                     " ▚▄▄▄▄▄▄▞ ",
                 },
                 {
                     COLOR_BOSS_SLIME_CROWN "  ▞▞▞▚▚▚  ",
                     COLOR_BOSS_SLIME_BASE " ▞▀▀" COLOR_BOSS_SLIME_EYES "▚" COLOR_BOSS_SLIME_BASE "▀" COLOR_BOSS_SLIME_EYES "▞" COLOR_BOSS_SLIME_BASE "▀▚ ",
                     " ▌ " COLOR_BOSS_SLIME_EYES "▀   ▀" COLOR_BOSS_SLIME_BASE "▐ ",
                     " ▌   " COLOR_BOSS_SLIME_MOUTH "▞▚" COLOR_BOSS_SLIME_BASE " ▐ ",
                     " ▚▄▄▄▄▄▄▞ ",
                 },
                 {
                     COLOR_BOSS_SLIME_CROWN "  ▞▞▞▚▚▚  ",
                     COLOR_BOSS_SLIME_BASE " ▞▀▀▀▀▀▀▚ ",
                     " ▌" COLOR_BOSS_SLIME_EYES "▚  ▞" COLOR_BOSS_SLIME_BASE "  ▐ ",
                     " ▌ " COLOR_BOSS_SLIME_MOUTH "▞▚" COLOR_BOSS_SLIME_BASE "   ▐ ",
                     " ▚▄▄▄▄▄▄▞ ",
                 },
                 {
                     COLOR_BOSS_SLIME_CROWN "  ▞▞▞▚▚▚  ",
                     COLOR_BOSS_SLIME_BASE " ▞▀▀▀▀▀▀▚ ",
                     " ▌ " COLOR_BOSS_SLIME_EYES "▄▝ ▘▄" COLOR_BOSS_SLIME_BASE "▐ ",
                     " ▌   " COLOR_BOSS_SLIME_MOUTH "▞▚" COLOR_BOSS_SLIME_BASE " ▐ ",
                     " ▚▄▄▄▄▄▄▞ ",
                 }};
            if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 > px)
            {
                sprite_anim = 0;
            }
            else if (enemy->next_cell.y * (CELL_HEIGHT + GAP / 2) + 2 < py)
            {
                sprite_anim = 1;
            }
            else if (enemy->next_cell.x * (CELL_HEIGHT + GAP / 2) + 2 > py)
            {
                sprite_anim = 3;
            }
            else if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 < px)
            {
                sprite_anim = 2;
            }
            for (int i = 0; i < 5; i++)
            {
                move_to(px - 3, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_TANK)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[2][4] =
                {{
                     COLOR_TANK_BASE "  ▞▀▚▄▄   ",
                     COLOR_TANK_BASE " ▞" COLOR_TANK_URSS "URSS" UNDERLINE_RST BOLD_RST COLOR_TANK_BASE "▚" COLOR_TANK_CANON "▄▄▄",
                     COLOR_TANK_BASE "▗▙▄▄▄▄▟▄▖ ",
                     COLOR_TANK_BASE "▚▄▄▄▄▄▄▄▞ ",
                 },
                 {
                     COLOR_TANK_BASE "   ▄▄▞▀▚  ",
                     COLOR_TANK_CANON "▄▄▄" COLOR_TANK_BASE "▞" COLOR_TANK_URSS "URSS" UNDERLINE_RST BOLD_RST COLOR_TANK_BASE "▚ ",
                     COLOR_TANK_BASE " ▗▄▙▄▄▄▄▟▖",
                     COLOR_TANK_BASE " ▚▄▄▄▄▄▄▄▞",
                 }};
            if (enemy->next_cell.x * (CELL_WIDTH + GAP) + 3 > px)
            {
                sprite_anim = 0;
            }
            else
            {
                sprite_anim = 1;
            }
            for (int i = 0; i < 4; i++)
            {
                move_to(px - 3, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }
        else if (enemy->type == ENEMY_BOSS_STUN)
        {
            int sprite_anim = 0;
            char *sprite_ennemy[2][5] =
                {{
                     COLOR_BOSS_STUN_BASE" ▞▀▀▚▞▀▀▚ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_SPARKS"▘"COLOR_BOSS_STUN_LIGHTNING"▞"COLOR_BOSS_STUN_SPARKS"▗"COLOR_BOSS_STUN_LIGHTNING" ▖"COLOR_BOSS_STUN_SPARKS"▝"COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_LIGHTNING"▞▝▖▞▝▖"COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_SPARKS" ▖"COLOR_BOSS_STUN_LIGHTNING"▝▝▖ "COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▚▄▄▞▚▄▄▞ ",
                 },
                 {
                     COLOR_BOSS_STUN_BASE" ▞▀▀▚▞▀▀▚ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_LIGHTNING"▖ ▝▖ "COLOR_BOSS_STUN_SPARKS"▝"COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_LIGHTNING"▝▖▞▝▖▞"COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▌"COLOR_BOSS_STUN_SPARKS   "▘"COLOR_BOSS_STUN_LIGHTNING"▞"COLOR_BOSS_STUN_SPARKS"▗ ▝ "COLOR_BOSS_STUN_BASE"▐ ",
                     COLOR_BOSS_STUN_BASE" ▚▄▄▞▚▄▄▞ ",
                 }};
            if (((int)enemy->grid_x % 2 == 1 && (int)enemy->grid_y % 2 == 1) //
                || ((int)enemy->grid_x % 2 == 0 && (int)enemy->grid_y % 2 == 0))
            {
                sprite_anim = 0;
            }
            else
            {
                sprite_anim = 1;
            }
            for (int i = 0; i < 5; i++)
            {
                move_to(px - 3, i + py);
                printf(sprite_ennemy[sprite_anim][i]);
            }
        }

        // move_to((enemy->next_cell.x * (CELL_WIDTH + GAP) + 3), (enemy->next_cell.y * (CELL_HEIGHT + GAP / 2) + 2));
        // printf("N%d", i);
        // move_to((enemy->previous_cell.x * (CELL_WIDTH + GAP) + 3), (enemy->previous_cell.y * (CELL_HEIGHT + GAP / 2) + 2));
        // printf("P%d", i);
        if (enemy->type == ENEMY_SLIME_BOSS || enemy->type == ENEMY_BOSS_STUN)
        {
            move_to(px, py - 1);
        }
        else
        {
            move_to(px - 1, py - 1);
        }
        printf(COLOR_HEALTH_BG); // Sprites de la barre de vie des ennemis
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

// Mise à jour des ennemis (Points de Vie, Position...)
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

        float rand_factor = 0.4;
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

        // Influence des effet des tourelles comme le stun (pétrification) et le slow (ralentissement) sur les mouvements des ennemis
        if (enemy->has_effect)
        {
            if (enemy->effet == Stun)
            {
                dx = 0;
                dy = 0;
                enemy->temps_rest -= dt_sec;
                if (enemy->temps_rest <= 0)
                {
                    enemy->has_effect = false;
                }
            }
            else if (enemy->effet == Slow)
            {
                dx *= enemy->puissance_effet;
                dy *= enemy->puissance_effet;
                enemy->temps_rest -= dt_sec;
                if (enemy->temps_rest <= 0)
                {
                    enemy->has_effect = false;
                }
            }
            else if (enemy->effet == Fire)
            {
                enemy->temps_rest -= dt_sec;
                if (enemy->temps_recharge < enemy->last_hit - enemy->temps_rest)
                {
                    enemy->last_hit = enemy->temps_rest;
                    enemy->hp -= 0.5;
                }
                if (enemy->temps_rest <= 0)
                {
                    enemy->has_effect = false;
                }
            }
        }

        enemy->grid_x += dx;
        enemy->grid_y += dy;

        if (enemy->grid_x >= grid.width) // Arrivée d'un ennmi à l'objectif
        {
            enemy->state = ENEMY_STATE_ARRIVED;
            defragNeeded = true;
            gs->health -= enemy->damage;
            for (size_t i = 0; i < CELL_HEIGHT; i++)
            {
                move_to((int)enemy->grid_x * (CELL_WIDTH + GAP) + 2, (int)enemy->grid_y * (CELL_HEIGHT + GAP / 2) + 2 + i);
                printf(COLOR_STANDARD_BG "       ");
            }
        }

        if (enemy->hp <= 0) // Effets liés à la mort d'un ennemi
        {
            enemy->state = ENEMY_STATE_DEAD;
            drawCell(enemy->previous_cell, grid);
            defragNeeded = true;
            gs->cash += enemy->money;
            labels->labels[labels->count].counter = 0;
            labels->labels[labels->count].duration = 2;
            labels->labels[labels->count].text = enemy->money;
            labels->labels[labels->count].x = (enemy->grid_x * (CELL_WIDTH + GAP) + 3);
            labels->labels[labels->count].y = (enemy->grid_y * (CELL_HEIGHT + GAP / 2) + 2);
            labels->count++;
        }
    }

    if (defragNeeded) // Suppression des ennemis à faire disparaître (morts où arrivés au bout)
    {
        defragEnemyPool(ep);
    }
}

/**
 * Renvoie le pattern de wave correspondant a l'index, la structure des vagues est soi calculée soit prédéfinie dans cette fonction
 * Initialise target_HP et target_HPPS selon des fonctions ajustées au tableur
 *
 * */
WavePattern getWaveByIndex(int waveIndex)
{
    WavePattern wp = {
        .target_POWER = (10 + 10 * waveIndex),
        .target_POWERPS = 5 + 2 * waveIndex,
        .random_coeffs = {0},
        .min_spawns = {0}};

    wp.random_coeffs[ENEMY_TUX] = 1;
    wp.random_coeffs[ENEMY_SPEED] = 1;
    wp.random_coeffs[ENEMY_SPIDER] = MIN(1, 0.5 + 0.5f * waveIndex / 16);   // 1 a vague 16
    wp.random_coeffs[ENEMY_HIGHTUX] = MIN(1, 0 + 1.0f * waveIndex / 20);    // 1 a vague 20
    wp.random_coeffs[ENEMY_HYPERSPEED] = MIN(1, 0 + 1.0f * waveIndex / 25); // 1 a vague 25

    wp.random_coeffs[ENEMY_SLIME_BOSS] = CLAMP(-0.1f + 0.2f * waveIndex / 20, 0.0f, 0.2f);
    wp.random_coeffs[ENEMY_BOSS_STUN] = CLAMP(-0.4f + 0.2f * waveIndex / 30, 0.0f, 0.2f);
    wp.random_coeffs[ENEMY_TANK] = CLAMP(-0.4f + 0.2f * waveIndex / 40, 0.0f, 0.2f);


    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        wp.coeff_sum += wp.random_coeffs[i];
    }

    return wp;
}

void switchToWave(WaveSystem *ws, int waveIndex)
{
    ws->current_wave_index = waveIndex;
    ws->current_wave_pattern = getWaveByIndex(waveIndex);
    ws->wave_HP_left = ws->current_wave_pattern.target_POWER;
    ws->next_spawn_timer = -1;
    ws->wave_timer = -1;
}

/**
 * Mets a jour le système de vagues
 * tant que wave_HP_left > 0
 * alors
 * - choisit un ennemi E aléatoirement selon:
 *      - les coeff
 *      - de sorte a ce que E.hp < target_HP (un ennemi ne doit pas avoir plus de PV qu'une vague entière)
 *      - ( de sorte a ce que E.hp < target_HPPS * HPPS_factor pour empêcher de placer de gros ennemis involontairement)
 * - ajoute l'ennemi a l'EnemyPool
 * - wave_HP_left -= E.hp
 * - Attendre T = E.hp / target_HPPS (période avant le prochain spawn)
 *      ici on utilisera next_spawn_timer et prev_spawn=E, ainsi, le prochain spawn arrivera après un certain nombre de frames.
 * fin tant que
 *
 * Renvoie:
 *  -2 = fin de la vague
 *  -1 = pas de spawn
 *  n  = id du mob spawné
 */
int updateWaveSystem(WaveSystem *ws, Grid grid, EnemyPool *ep, float dt)
{
    ws->next_spawn_timer -= dt;
    if (ws->next_spawn_timer > 0)
        return -1;

    // Détermination de la liste des ennemis choisissables
    bool enemy_choice_pool[ENEMY_COUNT];
    struct Enemy ennemi_courant;
    WavePattern *pattern = &ws->current_wave_pattern;
    bool valid = false;
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        // Cet ennemi ne va pas ètre ajouté, on le récupère juste pour connaître ses HP
        ennemi_courant = defEnemy(grid, i, 0, 0);

        if ((ennemi_courant.maxHP * ennemi_courant.speed <= ws->wave_HP_left && pattern->random_coeffs[i] != 0) || pattern->min_spawns[i] > 0)
        {
            enemy_choice_pool[i] = true;
            valid = true;
        }
        else
        {
            enemy_choice_pool[i] = false;
        }
    }

    if (!valid) // Si tous les ennemis de la vague on été spawnés.
    {
        if (ep->count == 0) // Dans le cas ou tous les ennemis sont morts, on peut démarrer le comptage pour la prochaine vague
        {
            if (ws->wave_timer == -1)
            {
                ws->wave_timer = WAVE_DELAY;
                return -2;
            }
            else if (ws->wave_timer == 0)
            {
                switchToWave(ws, ws->current_wave_index + 1);
            }
            else
            {
                ws->wave_timer = MAX(0, ws->wave_timer - dt);
            }
            return -1;
        }
        else // s'il reste des ennemis en vie, on ne fait rien
        {
            return -1;
        }
    }

    // Choix aléatoire de l’ennemi a spawn en fonction des coefficients
    enum EnemyType ennemi_choisi_id;
    struct Enemy *ennemi_choisi;
    size_t i = rand() % ENEMY_COUNT;
    while (true)
    {
        if (enemy_choice_pool[i % ENEMY_COUNT])
        {
            if (rand() / (float)RAND_MAX <= pattern->random_coeffs[i % ENEMY_COUNT] / pattern->coeff_sum || pattern->min_spawns[i % ENEMY_COUNT] > 0)
            {
                ennemi_choisi_id = i % ENEMY_COUNT;
                if (pattern->min_spawns[ennemi_choisi_id] > 0)
                {
                    pattern->min_spawns[ennemi_choisi_id]--;
                }
                break;
            }
        }
        i++;
    }

    ennemi_choisi = addEnemy(grid, ep, ennemi_choisi_id, grid.start_x, grid.start_y + 0.5);

    if (!ennemi_choisi->is_boss)
    {
        ws->wave_HP_left -= ennemi_choisi->maxHP * ennemi_choisi->speed;
        ws->next_spawn_timer = ennemi_choisi->maxHP / pattern->target_POWERPS;
    }
    else
    {
        ws->next_spawn_timer = 1.5;
    }

    return ennemi_choisi_id;
}

void testWaveSystem(Grid grid, EnemyPool *ep, int n)
{

    printf("SIMULATEUR\n");
    printf("Appuie sur une touche quelquonque pour générer une vague, q pour quitter\n");
    WaveSystem ws;
    int i = 0;
    int argent_cumul = 0;
    FILE *fptr;
    fptr = fopen("testing/waves.csv", "w");
    fprintf(fptr, "wave,hp,hpps,duration,argentCumul,ennemiesSpawned,");
    fprintf(fptr, "TUX,");
    fprintf(fptr, "SPEED,");
    fprintf(fptr, "SLIME BOSS,");
    fprintf(fptr, "HYPERSPEED,");
    fprintf(fptr, "SPIDER,");
    fprintf(fptr, "HIGHTUX,");
    fprintf(fptr, "SLOWBOSS,");
    fprintf(fptr, "BOSS_STUN");
    fprintf(fptr, "\n");

    printf("\t\t\t\tTUX\t\t");
    printf("SPEED\t\t");
    printf("SLIME BOSS\t");
    printf("HYPERSPEED\t");
    printf("SPIDER\t\t");
    printf("HIGHTUX\t\t");
    printf("SLOWBOSS\t");
    printf("BOSS_STUN");
    printf("\n");
    while (n <= 0 || i < n)
    {
        printf("\n");
        printf("\n");
        printf("Wave " COLOR_GREEN "%d" RESET, i);

        switchToWave(&ws, i);

        printf("\t HP: " COLOR_RED "%.1f" RESET " HPPS: " COLOR_YELLOW "%.1f" RESET " \t", ws.current_wave_pattern.target_POWER, ws.current_wave_pattern.target_POWERPS);
        for (int j = 0; j < ENEMY_COUNT; j++)
        {
            printf("%.1f%% \t\t", ws.current_wave_pattern.random_coeffs[j] / ws.current_wave_pattern.coeff_sum * 100);
        }

        printf("\n");

        int result = 0;
        float t = 0;
        int cnt = 0;
        int spawn_cnt[ENEMY_COUNT] = {0};
        // int tux = 0;
        // int speed = 0;
        // int boss = 0;
        do
        {
            float dt = (1.0f / TARGET_FPS);
            // msleep(dt * 1000);
            t += (dt);

            result = updateWaveSystem(&ws, grid, ep, dt);
            if (result >= 0)
            {
                cnt++;
                ep->count = 0;
                printf("\t %02d. t=%.1fs SPAWN %d (%fHP) -> HP_LEFT=%.1f %d %d\n", cnt, t, result, ep->enemies[0].hp, ws.wave_HP_left, ep->enemies[0].money, argent_cumul);
                argent_cumul += ep->enemies[0].money;
                spawn_cnt[result]++;
            }

            fflush(stdout);
        } while (result != -2);

        printf("\tDurée de la vague:" COLOR_FREEZER_BASE " %.1fs " RESET ", %d ennemis %d", t, cnt, argent_cumul);
        // wave,hp,hpps,duration,ennemiesSpawned
        fprintf(fptr, "%d,%f,%f,%.1f,%d,%d", i, ws.current_wave_pattern.target_POWER, ws.current_wave_pattern.target_POWERPS, t, argent_cumul, cnt);
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            fprintf(fptr, ",%d", spawn_cnt[i]);
        }
        fprintf(fptr, "\n");

        fflush(fptr);

        fflush(stdout);

        i++;

        if (n <= 0)
        {

            while (1)
            {
                int c = get_key_press();
                if (c == 'q')
                    return;
                if (c != 0)
                    break;
            }
        }
    }
    fclose(fptr);
}