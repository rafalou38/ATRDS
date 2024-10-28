#include "tower.h"

#if BULLETS_ON
void updateTowers(Grid grid, EnemyPool ep, BulletPool *bp, float dt)
{
#else
void updateTowers(Grid grid, EnemyPool ep, float dt)
{
#endif
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            if (grid.cells[x][y].hasTurret)
            {
                int lvl = grid.cells[x][y].turret.lvl;
                grid.cells[x][y].turret.compteur += dt;
                if (grid.cells[x][y].turret.compteur >= grid.cells[x][y].turret.reload_delay[lvl])
                {
                    int enemies_hit = 0;
                    for (int i = 0; i < ep.count; i++)
                    {
                        int d = sqrt(pow(ep.enemies[i].grid_x - x, 2) + pow(ep.enemies[i].grid_y - y, 2));
                        if (d <= grid.cells[x][y].turret.range_max[lvl]
                        && d >= grid.cells[x][y].turret.range_min[lvl])
                        {
                            grid.cells[x][y].turret.compteur = 0;

                            float dx = ep.enemies[i].grid_x - x;
                            float dy = ep.enemies[i].grid_y - y;
                            float d = sqrt(dx * dx + dy * dy);

                            grid.cells[x][y].turret.last_shot_dx = dx / d;
                            grid.cells[x][y].turret.last_shot_dy = dy / d;

#if BULLETS_ON
                            bp->bullets[bp->count].hit = false;
                            bp->bullets[bp->count].grid_x = x + 0.5 + (dx / d) / 4;
                            bp->bullets[bp->count].grid_y = y + 0.5 + (dy / d) / 4;
                            bp->bullets[bp->count].target = &(ep.enemies[i]);
                            bp->bullets[bp->count].damage = grid.cells[x][y].turret.damage[lvl];
                            bp->count++;
#else
                            ep.enemies[i].hp -= grid.cells[x][y].turret.damage[lvl];
                            if (grid.cells[x][y].turret.splash[lvl] != 0.0)
                            {
                                float d_min = grid.cells[x][y].turret.splash[lvl];
                                for (int j = 0 ; j < ep.count ; j++)
                                {
                                    if (j != i)
                                    {
                                        int d_enemy = sqrt(pow(ep.enemies[i].grid_x - ep.enemies[j].grid_x, 2) + 
                                        pow(ep.enemies[i].grid_y - ep.enemies[j].grid_y, 2));
                                        if (d_enemy < d_min)
                                        {
                                            ep.enemies[j].hp -= grid.cells[x][y].turret.damage[lvl];
                                        }
                                    }
                                }

                            }
#endif
                            enemies_hit++;
                            if (enemies_hit >= grid.cells[x][y].turret.nb_ennemi[lvl])
                                break;
                        }
                    }
                }
            }
        }
    }
}

#if BULLETS_ON
void defragBullets(BulletPool *bp)
{
    int right = 0;
    int left = -1;

    while (right < bp->count)
    {
        if (!bp->bullets[right].hit)
        {
            if (left != -1)
            {
                assert(left >= 0);
                // assert(left < bp->length);

                bp->bullets[left] = bp->bullets[right];
                left++;
            }
        }
        else if (left == -1)
        {
            left = right;
        }

        right++;
    }
    bp->count -= right - left;
    if (bp->count < 0)
        bp->count = 0;
}

void drawBullets(BulletPool bp)
{
    for (int i = 0; i < bp.count; i++)
    {
        int terminal_x = bp.bullets[i].grid_x * (CELL_WIDTH + GAP) + 3;
        int terminal_y = bp.bullets[i].grid_y * (CELL_HEIGHT + GAP / 2) + 2;
        move_to(terminal_x, terminal_y);

        printf("⌾");
    }
}
void updateBullets(BulletPool *bp, float dt)
{
    float defrag_needed = false;
    for (int i = 0; i < bp->count; i++)
    {
        if (bp->bullets[i].hit)
            continue;

        float dx = bp->bullets[i].target->grid_x - bp->bullets[i].grid_x;
        float dy = bp->bullets[i].target->grid_y - bp->bullets[i].grid_y;
        float d = sqrt(dx * dx + dy * dy);
        bp->bullets[i].grid_x += (dx / d) * 10 * dt;
        bp->bullets[i].grid_y += (dy / d) * 10 * dt;

        if (d <= 0.1)
        {
            bp->bullets[i].target->hp -= bp->bullets[i].damage;
            bp->bullets[i].hit = true;
            defrag_needed = true;
        }
    }
    if (defrag_needed)
        defragBullets(bp);
};

#endif

int getTurretPrice(enum TurretType type, int level)
{
    if (type == Sniper)
    {
        if (level == 0)
        {
            return 10;
        }
        if (level == 1)
        {
            return 15;
        }
    }
    if (type == Inferno)
    {
        if (level == 0)
        {
            return 10;
        }
        if (level == 1)
        {
            return 15;
        }
    }
    if (type == Mortier)
    {
        if (level == 0)
        {
            return 10;
        }
        if (level == 1)
        {
            return 15;
        }
    }

    return -1;
}

struct Turret getTurretStruct(enum TurretType type)
{
    struct Turret tur;
    if (type == Sniper)
    {
        tur.type = Sniper;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 100;
        tur.range_max[1] = 100;
        tur.damage[0] = 1;
        tur.damage[1] = 1.5;
        tur.reload_delay[0] = 0.5;
        tur.reload_delay[1] = 0.6;
        tur.splash[0] = 0.0;
        tur.splash[1] = 0.0;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 2;
    }
    else if (type == Inferno)
    {
        tur.type = Inferno;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 1;
        tur.range_max[1] = 2;
        tur.damage[0] = 0.2;
        tur.damage[1] = 0.3;
        tur.reload_delay[0] = 1.5;
        tur.reload_delay[1] = 2;
        tur.splash[0] = 0.0;
        tur.splash[1] = 0.0;
        tur.nb_ennemi[0] = 4;
        tur.nb_ennemi[1] = 8;
    }
    else if (type == Mortier)
    {
        tur.type = Mortier;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 2;
        tur.range_min[1] = 1;
        tur.range_max[0] = 5;
        tur.range_max[1] = 6;
        tur.damage[0] = 1.3;
        tur.damage[1] = 1.5;
        tur.reload_delay[0] = 3;
        tur.reload_delay[1] = 2.8;
        tur.splash[0] = 1.0;
        tur.splash[1] = 1.5;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 1;
    }
    return tur;
}

void showTowerSelection(int ligne, bool hasTurret, struct Turret selectedTurret)
{
    int terminal_width, terminal_height;
    get_terminal_size(&terminal_width, &terminal_height);

    int width = 80;  // terminal_width/2;
    int height = 30; // terminal_height/2;
    assert(width < terminal_width);
    assert(height < terminal_height);

    int x0 = terminal_width / 2.0f - width / 2.0f;
    int y0 = terminal_height / 2.0f - height / 2.0f + 1;

    printf(RESET);
    printf(COLOR_STANDARD_BG);

    for (int y = 0; y < height; y++)
    {
        move_to(x0, y0 + y);

        for (int x = 0; x < width; x++)
        {
            printf(RESET);
            if (y == ligne)
                printf("\033[38;5;221m");
            if (x == 0 || x == width - 1)
                printf("█");
            else if (y == 0)

                printf("▀");
            else if (y == height - 1)
                printf("▄");
            else
                printf(" ");
        }
    }

    if (hasTurret)
    {
        int next_price = getTurretPrice(selectedTurret.type, selectedTurret.lvl + 1);

        if (next_price != -1)
        {
            move_to(x0 + 1, y0 + 1);
            printf(" 🛠️ Upgrade");

            move_to(x0 + 1 + width - 8, y0 + 1);
            printf(COLOR_YELLOW "-% 3d €" RESET, next_price);
        }

        move_to(x0 + 1, y0 + 2);
        printf(" 💰 Sell");

        move_to(x0 + 1 + width - 8, y0 + 2);
        printf(COLOR_GREEN "+% 3d €" RESET, getTurretPrice(selectedTurret.type, selectedTurret.lvl));
    }
    else
    {
        move_to(x0 + 1, y0 + 1);
        printf(" 🔫 Sniper");
        move_to(x0 + 1 + width - 8, y0 + 1);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Sniper, 0));

        move_to(x0 + 1, y0 + 2);
        printf(" 🔥 Inferno");
        move_to(x0 + 1 + width - 8, y0 + 2);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Inferno, 0));
        
        move_to(x0 + 1, y0 + 3);
        printf(" Mortier ");
        move_to(x0 + 1 + width - 8, y0 + 3);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Mortier, 0));
    }
    printf(RESET);
}