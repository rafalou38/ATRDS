#include "shared.h"

#include "grid.h"
#include "enemies.h"

/*
#################
ÉTAT DU PROGRAMME
#################
*/

Grid grid;
EnemyPool enemyPool;

void cleanup()
{
    show_cursor();
    printf("Bye.\n");
    fflush(stdout);
}

int main()
{
    // SEEDS FUN: énorme jaune: 1729285683    /    problème ? 1729285706
    unsigned int seed = 1729518567 ;//time(NULL);
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

    grid.width = (width - 2) / (CELL_WIDTH + 2);
    grid.height = (height - 2) / (CELL_HEIGHT + 1);

    allocateGridCells(&grid);

    enemyPool = AllocEnemyPool();
    genBasicPath(&grid);

    clear_screen();
    drawFullGrid(grid);

    addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);

    while (true)
    {
        /* code */
        if (rand() % 200 == 0)
            addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);

        updateEnemies(&enemyPool, grid);
        clearPath(grid);
        drawEnemies(enemyPool, grid);

        move_to(0,0);
        printf("Enemy count %d/%d", enemyPool.count, enemyPool.length);

        fflush(stdout);
        msleep(10);
    }


    printf("\n");
    printf("\n");

    freeGrid(grid);
    freeEnemyPool(enemyPool);

    printf("\n");

    return 0;
}

// 1729518567