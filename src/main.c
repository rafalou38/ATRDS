#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "IO.h"

#define MIN_TERMINAL_WIDTH 140
#define MIN_TERMINAL_HEIGHT 30

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

enum CellType
{
    CHEMIN,
    TERRAIN
};

struct Cell
{
    int x;
    int y;
    enum CellType type;
};

#define CELL_WIDTH 6
#define CELL_HEIGHT 3
#define GAP 2

void drawCell(struct Cell cell)
{
    if (cell.type == TERRAIN)
    {
        for (int y = 0; y < CELL_HEIGHT; y++)
        {
            // printf("%d")
            int cx = cell.x * (CELL_WIDTH + GAP);
            if (cell.x != 0)
                cx++;
            move_to(cx, cell.y * (CELL_HEIGHT + GAP/2) + y + 1);

            printf("\033[42m");
            for (int x = 0; x < CELL_WIDTH; x++)
            {
                printf(" ");
            }
            printf("\033[0m");
        }
    }
}

int main()
{
    // Cette fonction si on l'active n'affichera pas le résultat des printf en direct mais tout d'un coup apres avoir appelé fflush(stdout); (meilleures performances)
    // https://en.cppreference.com/w/c/io/setvbuf
    setvbuf(stdout, NULL, _IOFBF, (MIN_TERMINAL_WIDTH + 5) * (MIN_TERMINAL_HEIGHT + 5));

    // Enregistre la fonction cleanup pour qu'elle soit exécutée a la terminaison du programme.
    atexit(cleanup);

    hide_cursor();
    clear_screen();

    // => Pré-check de la taille du terminal

    // TODO: re-check un peu tout le temps au cas ou c'est re-dimensionné.

    int width = 0;
    int height = 0;
    get_terminal_size(&width, &height);

    while (width < MIN_TERMINAL_WIDTH || height < MIN_TERMINAL_HEIGHT)
    {
        clear_screen();
        get_terminal_size(&width, &height);

        move_to(width / 2 - 20, height / 2 - 1);
        printf("Merci d'agrandir le terminal ou de dézoomer");
        move_to(width / 2 - 7, height / 2);
        printf("Colones: %d/%d", width, MIN_TERMINAL_WIDTH);
        move_to(width / 2 - 7, height / 2 + 1);
        printf("Rangées: %d/%d", height, MIN_TERMINAL_HEIGHT);

        fflush(stdout);

        usleep(100 * 1000);
    }
    int gridheight = 7;
    int gridwidth = 7;
    struct Cell grid[gridheight][gridwidth];
    for (int x = 0; x < gridheight; x++)
    {
        for (int y = 0; y < gridwidth; y++)
        {
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].type = TERRAIN;
        }
    }

    for (int x = 0; x < gridheight; x++)
    {
        for (int y = 0; y < gridwidth; y++)
        {
            drawCell(grid[x][y]);
        }
    }
    fflush(stdout);

    // while (true)
    // {
    //     move_to(0, 0);
    //     // Si possible, il faudra éviter d'appeler clear_screen, ça fait clignoter l'écran, mieux vaut écrire par dessus. -> quand les mises a jour sont rapides (~1/10s pour réécrire la totalité de l'écran)
    //     clear_screen();
    //     get_terminal_size(&width, &height);

    //     for (int y = 0; y < height - 5; y++)
    //     {
    //         for (int x = 0; x < (grid_width + 2) * (width / (grid_width + 2)); x++)
    //         {
    //             // +2 pour l’espace;
    //             if (x % (grid_width + 2) <= 1 || y % grid_height == 0)
    //             {
    //                 printf(" ");
    //             }
    //             else
    //             {
    //                 // if (x % ((grid_width+2) * 2) < grid_width+2)
    //                 //     printf("🮘");
    //                 // else
    //                 printf("X");
    //             }
    //         }
    //         printf("\n");
    //     }

    //     printf("Ctrl+C pour quitter\ntaille de la grille: %dx%d  carrées: 7x4 9x5 11x6 13x7...\n", grid_width, grid_height);
    //     printf("Nouvelle taille ? (nxn): ");
    //     fflush(stdout);

    //     scanf("%dx%d", &grid_width, &grid_height);
    //     grid_width = MAX(1, MIN(grid_width, 20));
    //     grid_height = MAX(1, MIN(grid_height, 20));

    //     // usleep((1.0f / 60) * 1000 * 1000); // limite a  fps < 60Hz (ne prends pas en compte le delay réel)
    // }

    return 0;
}
