#include "shared.h"
#include "IO.h"

#include "grid.h"

/*
#################
ÉTAT DU PROGRAMME
#################
*/
int gridheight = 8;
int gridwidth = 20;
struct Cell **grid;


void cleanup()
{
    show_cursor();
    printf("Bye.\n");
    fflush(stdout);
}

int main()
{
    // SEEDS FUN: énorme jaune: 1729285683    /    problème ? 1729285706
    unsigned int seed = time(NULL);
    printf("seed: %d\n", seed);
    srand(seed);
    // Cette fonction si on l'active n'affichera pas le résultat des printf en direct mais tout d'un coup apres avoir appelé fflush(stdout); (meilleures performances)
    // https://en.cppreference.com/w/c/io/setvbuf
    setvbuf(stdout, NULL, _IOFBF, (MIN_TERMINAL_WIDTH + 5) * (MIN_TERMINAL_HEIGHT + 5));

    // Enregistre la fonction cleanup pour qu'elle soit exécutée a la terminaison du programme.
    atexit(cleanup);

    hide_cursor();
    clear_screen();

    int width = 0;
    int height = 0;

    checkTerminalSize(&width, &height);

    gridwidth = (width - 2) / (CELL_WIDTH + 2);
    gridheight = (height - 2) / (CELL_HEIGHT + 1);


    grid = allocateGrid(gridwidth, gridheight);
    genBasicPath(grid, gridwidth, gridheight);

    clear_screen();
    for (int x = 0; x < gridwidth; x++)
    {
        for (int y = 0; y < gridheight; y++)
        {
            drawCell(grid[x][y], grid, gridwidth, gridheight);
        }
    }
    fflush(stdout);


    freeGrid(grid, gridwidth);

    printf("\n");

    return 0;
}
