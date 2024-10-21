#include "shared.h"
#include "IO.h"

void allocateGridCells(Grid *grid)
{
    struct Cell **cells;
    cells = (struct Cell **)malloc(sizeof(struct Cell *) * grid->width); // Première coordonnée: x / width(largeur)
    grid->cells = cells;

    if (cells == NULL)
    {
        printCritical("Failed to allocate grid");
        exit(EXIT_FAILURE);
    }
    for (int x = 0; x < grid->width; x++)
    {

        cells[x] = (struct Cell *)malloc(sizeof(struct Cell) * grid->height); // deuxième coordonnée: y / height(hauteur)
        if (cells[x] == NULL)
        {
            printCritical("Failed to allocate grid col");
            exit(EXIT_FAILURE);
        }

        for (int y = 0; y < grid->height; y++)
        {
            cells[x][y].x = x;
            cells[x][y].y = y;
            cells[x][y].type = TERRAIN;
            cells[x][y].visited = false;
        }
    }
}

void freeGrid(Grid grid)
{
    printf("\033[38;5;243m $\033[0m Freeing \033[38;5;11;1m%dx%d\033[0m grid:\t", grid.width, grid.height);
    for (int x = 0; x < grid.width; x++)
    {
        free(grid.cells[x]);
    }
    free(grid.cells);

    printf("\033[38;5;42m Done \033[0m\n");
}

void genBasicPath(Grid grid)

{
    int chemin_x = 1;
    int chemin_y = (rand() % (grid.height));
    if (chemin_y == grid.height - 1) // Rectifications du random pour pas la 1ere ni la derniere ligne
        chemin_y = chemin_y - 1;

    if (chemin_y == 0)
        chemin_y = chemin_y + 1;

    struct Cell **cells = grid.cells;

    cells[0][chemin_y].type = CHEMIN; // 2 premieres cases sans les autres options, fixées à une hauteur aléatoire
    cells[chemin_x][chemin_y].type = CHEMIN;

    int **historique;
    historique = (int **)malloc(HISTORY_SIZE * (sizeof(int *)));
    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        historique[i] = (int *)malloc(2 * sizeof(int));
    }

    int history_index = 0;

    while (chemin_x != grid.width - 1)
    {
        bool posible_bas = chemin_y < grid.height - 2                              //
                           && cells[chemin_x - 1][chemin_y + 1].type != CHEMIN // bas gauche
                           && cells[chemin_x + 1][chemin_y + 1].type != CHEMIN // bas droite
                           && cells[chemin_x][chemin_y + 1].type != CHEMIN     // bas
                           && cells[chemin_x - 1][chemin_y + 2].type != CHEMIN // bas bas gauche
                           && cells[chemin_x][chemin_y + 2].type != CHEMIN     // bas bas
                           && !cells[chemin_x][chemin_y + 1].visited;

        bool possible_haut = chemin_y > 1                                       //
                             && cells[chemin_x - 1][chemin_y - 1].type != CHEMIN // haut gauche
                             && cells[chemin_x + 1][chemin_y - 1].type != CHEMIN // haut droit
                             && cells[chemin_x][chemin_y - 1].type != CHEMIN     // haut
                             && cells[chemin_x][chemin_y - 2].type != CHEMIN     // haut haut
                             && cells[chemin_x - 1][chemin_y - 2].type != CHEMIN // haut haut gauche
                             && !cells[chemin_x][chemin_y - 1].visited;

        bool possible_droite = cells[chemin_x + 1][chemin_y + 1].type != CHEMIN                           // droite bas
                               && cells[chemin_x + 1][chemin_y - 1].type != CHEMIN                        // droite haut
                               && cells[chemin_x + 1][chemin_y].type != CHEMIN                            // droite
                               && (chemin_x == grid.width - 2 || cells[chemin_x + 2][chemin_y].type != CHEMIN) // droite droite
                               && !cells[chemin_x + 1][chemin_y].visited;

        bool possible_gauche = chemin_x > 1                                       //
                               && cells[chemin_x - 1][chemin_y + 1].type != CHEMIN // gauche bas
                               && cells[chemin_x - 1][chemin_y - 1].type != CHEMIN // gauche haut
                               && cells[chemin_x - 1][chemin_y].type != CHEMIN     // gauche
                               && cells[chemin_x - 2][chemin_y].type != CHEMIN     // gauche gauche
                               && cells[chemin_x - 2][chemin_y + 1].type != CHEMIN // gauche gauche haut
                               && cells[chemin_x - 2][chemin_y - 1].type != CHEMIN // gauche gauche bas
                               && !cells[chemin_x - 1][chemin_y].visited;

        if (!posible_bas && !possible_haut && !possible_droite && !possible_gauche)
        {
            cells[chemin_x][chemin_y].type = TERRAIN;
            history_index -= 1;
            chemin_x = historique[history_index][0];
            chemin_y = historique[history_index][1];
            cells[chemin_x][chemin_y].index = history_index;
            continue;
        }

        while (true)
        {
            int random_chemin = rand() % 7;
            if (random_chemin <= 1 && posible_bas)
            {
                chemin_y++;
                break;
            }
            else if (random_chemin <= 3 && possible_haut)
            {
                chemin_y--;
                break;
            }
            else if (random_chemin <= 5 && possible_gauche)
            {
                chemin_x--;
                break;
            }
            else if (random_chemin == 6 && possible_droite)
            {
                chemin_x++;
                break;
            }
        }

        history_index++;
        assert(history_index < HISTORY_SIZE);

        historique[history_index][0] = chemin_x;
        historique[history_index][1] = chemin_y;

        cells[chemin_x][chemin_y].type = CHEMIN;
        cells[chemin_x][chemin_y].visited = true;
        cells[chemin_x][chemin_y].index = history_index;
    }

    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        free(historique[i]);
    }
    free(historique);
}


void drawFullGrid(Grid grid){
    
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            drawCell(grid.cells[x][y], grid);
        }
    }
    fflush(stdout);
}