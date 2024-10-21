#include "shared.h"

struct Cell **allocateGrid(int width, int height)
{
    struct Cell **grid;
    grid = (struct Cell **)malloc(sizeof(struct Cell *) * width); // Première coordonnée: x / largeur
    if (grid == NULL)
    {
        printCritical("Failed to allocate grid");
        exit(EXIT_FAILURE);
    }
    for (int x = 0; x < width; x++)
    {

        grid[x] = (struct Cell *)malloc(sizeof(struct Cell) * height); // deuxième coordonnée: y / hauteur
        if (grid[x] == NULL)
        {
            printCritical("Failed to allocate grid col");
            exit(EXIT_FAILURE);
        }

        for (int y = 0; y < height; y++)
        {
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].type = TERRAIN;
            grid[x][y].visited = false;
        }
    }

    return grid;
}

void freeGrid(struct Cell **grid, int width)
{
    printf("\033[38;5;243m $\033[0m Freeing \033[38;5;11;1m%d\033[0m grid cols:\t", width);
    for (int x = 0; x < width; x++)
    {
        free(grid[x]);
    }
    free(grid);

    printf("\033[38;5;42m Done \033[0m\n");
}

void genBasicPath(struct Cell **grid, int width, int height)
{
    int variable_arret1 = 0;
    int chemin_x = 1;
    int chemin_y = (rand() % (height));
    if (chemin_y == height - 1) // Rectifications du random pour pas la 1ere ni la derniere ligne
        chemin_y = chemin_y - 1;

    if (chemin_y == 0)
        chemin_y = chemin_y + 1;

    grid[0][chemin_y].type = CHEMIN; // 2 premieres cases sans les autres options, fixées à une hauteur aléatoire
    grid[chemin_x][chemin_y].type = CHEMIN;

    int **historique;
    historique = (int **)malloc(HISTORY_SIZE * (sizeof(int *)));
    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        historique[i] = (int *)malloc(2 * sizeof(int));
    }

    int history_index = 0;

    while (chemin_x != width - 1)
    {
        bool posible_bas = chemin_y < height - 2                              //
                           && grid[chemin_x - 1][chemin_y + 1].type != CHEMIN // bas gauche
                           && grid[chemin_x + 1][chemin_y + 1].type != CHEMIN // bas droite
                           && grid[chemin_x][chemin_y + 1].type != CHEMIN     // bas
                           && grid[chemin_x - 1][chemin_y + 2].type != CHEMIN // bas bas gauche
                           && grid[chemin_x][chemin_y + 2].type != CHEMIN     // bas bas
                           && !grid[chemin_x][chemin_y + 1].visited;

        bool possible_haut = chemin_y > 1                                       //
                             && grid[chemin_x - 1][chemin_y - 1].type != CHEMIN // haut gauche
                             && grid[chemin_x + 1][chemin_y - 1].type != CHEMIN // haut droit
                             && grid[chemin_x][chemin_y - 1].type != CHEMIN     // haut
                             && grid[chemin_x][chemin_y - 2].type != CHEMIN     // haut haut
                             && grid[chemin_x - 1][chemin_y - 2].type != CHEMIN // haut haut gauche
                             && !grid[chemin_x][chemin_y - 1].visited;

        bool possible_droite = grid[chemin_x + 1][chemin_y + 1].type != CHEMIN                           // droite bas
                               && grid[chemin_x + 1][chemin_y - 1].type != CHEMIN                        // droite haut
                               && grid[chemin_x + 1][chemin_y].type != CHEMIN                            // droite
                               && (chemin_x == width - 2 || grid[chemin_x + 2][chemin_y].type != CHEMIN) // droite droite
                               && !grid[chemin_x + 1][chemin_y].visited;

        bool possible_gauche = chemin_x > 1                                       //
                               && grid[chemin_x - 1][chemin_y + 1].type != CHEMIN // gauche bas
                               && grid[chemin_x - 1][chemin_y - 1].type != CHEMIN // gauche haut
                               && grid[chemin_x - 1][chemin_y].type != CHEMIN     // gauche
                               && grid[chemin_x - 2][chemin_y].type != CHEMIN     // gauche gauche
                               && grid[chemin_x - 2][chemin_y + 1].type != CHEMIN // gauche gauche haut
                               && grid[chemin_x - 2][chemin_y - 1].type != CHEMIN // gauche gauche bas
                               && !grid[chemin_x - 1][chemin_y].visited;

        if (!posible_bas && !possible_haut && !possible_droite && !possible_gauche)
        {
            grid[chemin_x][chemin_y].type = TERRAIN;
            history_index -= 1;
            chemin_x = historique[history_index][0];
            chemin_y = historique[history_index][1];
            grid[chemin_x][chemin_y].index = history_index;
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

        grid[chemin_x][chemin_y].type = CHEMIN;
        grid[chemin_x][chemin_y].visited = true;
        grid[chemin_x][chemin_y].index = history_index;
    }

    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        free(historique[i]);
    }
    free(historique);
}