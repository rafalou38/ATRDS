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

void configTerminal()
{
    // Cette fonction si on l'active n'affichera pas le résultat des printf en direct mais tout d'un coup apres avoir appelé fflush(stdout); (meilleures performances)
    // https://en.cppreference.com/w/c/io/setvbuf
    setvbuf(stdout, NULL, _IOFBF, (MIN_TERMINAL_WIDTH + 5) * (MIN_TERMINAL_HEIGHT + 5) * 2);
    setbuf(stdin, NULL);
    hide_cursor();

    // https://man7.org/linux/man-pages/man3/termios.3.html
    static struct termios t_settings;
    tcgetattr(STDIN_FILENO, &t_settings);

    // Place le términal en mode non canonique (entrées envoyées sans enter) + sans echo, les touches pressés ne sont pas affichées.
    t_settings.c_lflag &= ~(ICANON);
    t_settings.c_lflag &= ~(ECHO);
    t_settings.c_lflag &= ~(ECHOE);
    t_settings.c_lflag &= ~(ECHOK);
    t_settings.c_lflag &= ~(ECHONL);

    tcsetattr(STDIN_FILENO, TCSANOW, &t_settings);
}

int main()
{
    // SEEDS FUN: énorme jaune: 1729285683    /    problème ? 1729285706
    unsigned int seed = time(NULL); // time(NULL);    seeded : 1729518567
    printf("seed: %d\n", seed);
    srand(seed);

    // Enregistre la fonction cleanup pour qu'elle soit exécutée  la terminaison du programme.
    atexit(cleanup);
    struct sigaction act;
    // Set the signal handler as the default action
    act.sa_handler = &cleanup;
    // Apply the action in the structure to the
    // SIGINT signal (ctrl-c)
    sigaction(SIGINT, &act, NULL);

    configTerminal();
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

    time_t time_start = prev_time.tv_sec;
    float spawnTimer;

    struct mallinfo2 info;

    int selected_cell_x = 3;
    int selected_cell_y = 2;
    

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
            info = mallinfo2();
            printf("Enemy count %d/%d | (%.1f fps) | runtime: %lds | Heap: %zuKo",
                   enemyPool.count,                      //
                   enemyPool.length,                     //
                   round(1.0f / delta_t * 10.0f) / 10.0, //
                   current_time.tv_sec - time_start,     //
                   info.uordblks/1000);
        }

        fflush(stdout);
        // Get keystroke
        
        int c = get_key_press();
        if (c != 0)
        {
            if (c == 'q' || c == 'Q')
            {
                break;
            }
            else if (c == 27) // \033
            {
                getchar(); // [
                c = getchar();

                if (c == 'A' 
                && selected_cell_y-CELL_HEIGHT-GAP/2>1)
                {
                    selected_cell_y=selected_cell_y-CELL_HEIGHT-GAP/2;
                }
                else if (c == 'B' 
                && selected_cell_y+CELL_HEIGHT+GAP/2< 2 + (CELL_HEIGHT+GAP/2) * grid.height )
                {
                    selected_cell_y=selected_cell_y+CELL_HEIGHT+GAP/2;
                }
                else if (c == 'C' 
                && selected_cell_x+CELL_WIDTH+GAP< 3 + (CELL_WIDTH+GAP) * grid.width )
                {
                    selected_cell_x=selected_cell_x+CELL_WIDTH+GAP;
                }
                else if (c == 'D' 
                && selected_cell_x-CELL_WIDTH-GAP>1)
                {
                    selected_cell_x=selected_cell_x-CELL_WIDTH-GAP;
                }
                
                drawFullGrid(grid);

                printf(COLOR_SELECTED_SLOT);
                for(int x_of_cell = selected_cell_x ; x_of_cell<CELL_WIDTH+selected_cell_x; x_of_cell++){
                    move_to(x_of_cell,selected_cell_y);
                    printf("█");
                }
                for(int x_of_cell = selected_cell_x ; x_of_cell<CELL_WIDTH+selected_cell_x; x_of_cell++){
                    move_to(x_of_cell,selected_cell_y+CELL_HEIGHT-1);
                    printf("█");
                }
                for(int y_of_cell = selected_cell_y+1 ; y_of_cell<CELL_HEIGHT+selected_cell_y; y_of_cell++){
                    move_to(selected_cell_x,y_of_cell);
                    printf("██");
                }
                for(int y_of_cell = selected_cell_y+1 ; y_of_cell<CELL_HEIGHT+selected_cell_y; y_of_cell++){
                    move_to(selected_cell_x+CELL_WIDTH-2,y_of_cell);
                    printf("██");
                }
                move_to(selected_cell_x+3,selected_cell_y+2);
                printf("%d,%d",grid.width,selected_cell_x);
                
            }
           
            fflush(stdout);
        }
    }

    printf("\n");
    clear_screen();
    move_to(0, 0);
    freeGrid(grid);
    freeEnemyPool(enemyPool);
    
    return 0;
}

// 1729518567