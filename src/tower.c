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

void updateTowers(Grid grid, EnemyPool ep, float dt)
{
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            if (grid.cells[x][y].hasTurret)
            {
                int terminal_x = x * (CELL_WIDTH + GAP) + 3;
                int terminal_y = y * (CELL_HEIGHT + GAP / 2) + 2;
                move_to(terminal_x, terminal_y);

                printf("%.2f", grid.cells[x][y].turret.compteur);
                grid.cells[x][y].turret.compteur += dt;
                if (grid.cells[x][y].turret.compteur >= grid.cells[x][y].turret.reload_delay)
                {
                    for (int i = 0; i < ep.count; i++)
                    {
                        int d = sqrt(pow(ep.enemies[i].grid_x - x, 2) + pow(ep.enemies[i].grid_y - y, 2));
                        if (d <= grid.cells[x][y].turret.range)
                        {
                            grid.cells[x][y].turret.compteur = 0;
                            ep.enemies[i].hp -= grid.cells[x][y].turret.damage;
                        }
                    }
                }
            }
        }
    }
}