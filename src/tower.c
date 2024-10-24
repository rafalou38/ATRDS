#include "tower.h"

void showTowerSelection(int ligne, bool hasTurret)
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

    selectionTower(x0, y0, hasTurret);
    printf(RESET);
}

void selectionTower(int x0, int y0, bool hasTurret)
{
    if (hasTurret)
    {
        move_to(x0 + 1, y0 + 1);
        printf("Upgrade");
    }
    else
    {
        move_to(x0 + 1, y0 + 1);
        printf("Sniper");
    }
}

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
                grid.cells[x][y].turret.compteur += dt;
                if (grid.cells[x][y].turret.compteur >= grid.cells[x][y].turret.reload_delay)
                {
                    for (int i = 0; i < ep.count; i++)
                    {
                        int d = sqrt(pow(ep.enemies[i].grid_x - x, 2) + pow(ep.enemies[i].grid_y - y, 2));
                        if (d <= grid.cells[x][y].turret.range)
                        {
                            grid.cells[x][y].turret.compteur = 0;
#if BULLETS_ON
                            bp->bullets[bp->count].hit = false;
                            bp->bullets[bp->count].grid_x = x;
                            bp->bullets[bp->count].grid_y = y;
                            bp->bullets[bp->count].target = &(ep.enemies[i]);
                            bp->bullets[bp->count].damage = grid.cells[x][y].turret.damage;
                            bp->count++;
#else
                            ep.enemies[i].hp -= grid.cells[x][y].turret.damage;
#endif
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