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
    exit(0);
}

int main()
{
    // SEEDS FUN: énorme jaune: 1729285683    /    problème ? 1729285706
    unsigned int seed = 1729518567; // time(NULL);
    printf("seed: %d\n", seed);
    srand(seed);
    // Cette fonction si on l'active n'affichera pas le résultat des printf en direct mais tout d'un coup apres avoir appelé fflush(stdout); (meilleures performances)
    // https://en.cppreference.com/w/c/io/setvbuf
    setvbuf(stdout, NULL, _IOFBF, (MIN_TERMINAL_WIDTH + 5) * (MIN_TERMINAL_HEIGHT + 5) * 2);

    // Enregistre la fonction cleanup pour qu'elle soit exécutée a la terminaison du programme.
    atexit(cleanup);
    struct sigaction act;
    // Set all of the structure's bits to 0 to avoid errors
    // relating to uninitialized variables...
    // bzero(&act, sizeof(act));
    // Set the signal handler as the default action
    act.sa_handler = &cleanup;
    // Apply the action in the structure to the
    // SIGINT signal (ctrl-c)
    sigaction(SIGINT, &act, NULL);

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
    fillBG(1, 1, width + 1, height + 1);
    // return 0;

    drawFullGrid(grid);

    addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);

    struct timespec prev_time;

    clock_gettime(CLOCK_MONOTONIC, &prev_time);

    float spawnTimer;

    for (int i = 0; i < 10000; i++)
    {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        float delta_t = (current_time.tv_nsec - prev_time.tv_nsec) / 1000000000.0 + (current_time.tv_sec - prev_time.tv_sec);

        prev_time = current_time;

        if (delta_t < 1.0 / TARGET_FPS)
        {
            msleep((1.0 / TARGET_FPS - delta_t) * 1000);
        }

        spawnTimer += delta_t;

        /* code */
        if (spawnTimer > 4.0)
        {
            addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);
            spawnTimer = 0.0;
        }

        updateEnemies(&enemyPool, grid, delta_t);
        // clearPath(grid)
        clearUsedPath(grid, enemyPool);
        drawEnemies(enemyPool, grid);

        if (1.0f / delta_t < 2 * TARGET_FPS)
        {
            move_to(0, 0);
            printf(COLOR_STANDARD_BG);
            printf("Enemy count %d/%d | (%.1f fps)", enemyPool.count, enemyPool.length, round(1.0f / delta_t * 10.0f) / 10.0);
        }

        fflush(stdout);
    }

    printf("\n");
    clear_screen();
    move_to(0, 0);
    freeGrid(grid);
    freeEnemyPool(enemyPool);

    return 0;
}

// 1729518567