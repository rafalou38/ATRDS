#include "shared.h"

#include "grid.h"
#include "enemies.h"
#include "tower.h"

/*
#################
ÉTAT DU PROGRAMME
#################
*/

Grid grid;
EnemyPool enemyPool;
GameStats gameStats;
Labels labels;

#if BULLETS_ON
BulletPool bulletPool;
#endif

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
    genBasicPath(&grid);

    gameStats.cash = 100;
    gameStats.health = 10;
    gameStats.wave = 0;

    labels.size = 255;
    labels.labels = malloc(sizeof(struct Label) * labels.size);

    enemyPool = AllocEnemyPool();

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

    bool selection_active = false;
    bool was_range_visible = false;
    int selected_cell_x = 0;
    int selected_cell_y = 0;
    int ligne = 1;
    grid.cells[0][0].selected = true;
    int nb_ennemy = 0;
    enum EnemyType possible_ennemy[2];
    bool changing_wave = true;

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

        /*
        #############################
        GESTION DE L'AFFICHAGE DU JEU
        #############################
        */
        { // MENU DE SELECTION
            if (selection_active)
            {
                showTowerSelection(ligne, grid.cells[selected_cell_x][selected_cell_y].hasTurret, grid.cells[selected_cell_x][selected_cell_y].turret);
            }
            else
            {
                // JEU NORMAL

                // Spawn des ennemis
                spawnTimer += delta_t;
                if (changing_wave)
                {
                    gameStats.wave += 1;
                    if (gameStats.wave / 5 == 0)
                    {
                        possible_ennemy[0] = ENEMY_TUX;
                        float alea = (float)rand() / RAND_MAX;
                        nb_ennemy = 15 + (alea * 20 - 10);
                    }
                    else
                    {
                        possible_ennemy[0] = ENEMY_TUX;
                        possible_ennemy[1] = ENEMY_SPEED;
                        float alea = (float)rand() / RAND_MAX;
                        nb_ennemy = 15 + (alea * 20 - 10);
                    }
                    changing_wave = false;
                }
                if (spawnTimer > 1.0)
                {
                    if (nb_ennemy > 0)
                    {
                        nb_ennemy -= 1;
                        if (gameStats.wave / 5 == 0)
                        {
                            addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);
                            spawnTimer = 0.0;
                        }
                        else
                        {
                            int type_alea = rand();
                            addEnemy(grid, &enemyPool, possible_ennemy[(RAND_MAX - type_alea) / (RAND_MAX / 2)],
                                     grid.start_x, grid.start_y);
                            spawnTimer = 0.0;
                        }
                    }
                }
                if (enemyPool.count == 0 && nb_ennemy == 0)
                {
                    changing_wave = true;
                }

#if BULLETS_ON
                if (i % 50 == 0)
                {
                    fillBG(1, 1, width + 1, height + 1);
                    drawFullGrid(grid);
                }
                else
                {
                    clearUsedPath(grid, enemyPool);
                }

                updateTowers(grid, enemyPool, &bulletPool, delta_t);
                updateBullets(&bulletPool, delta_t);

                drawBullets(bulletPool);
#else
                clearUsedPath(grid, enemyPool);
                updateTowers(grid, enemyPool, delta_t);
#endif
                updateLabels(&labels, delta_t);
                drawLabels(labels);

                // Mise à jour des ennemis existants
                updateEnemies(&enemyPool, grid, &gameStats, &labels, delta_t);
                // Affichage des ennemis
                drawEnemies(enemyPool, grid);

                // Affichage cercle range
                if (grid.cells[selected_cell_x][selected_cell_y].hasTurret)
                {
                    float range_min = grid.cells[selected_cell_x][selected_cell_y].turret.range_min[grid.cells[selected_cell_x][selected_cell_y].turret.lvl];
                    float range_max = grid.cells[selected_cell_x][selected_cell_y].turret.range_max[grid.cells[selected_cell_x][selected_cell_y].turret.lvl];
                    if (range_min > 0)
                        drawRange(width, height, range_min, selected_cell_x, selected_cell_y);
                    if (range_max > 0)
                        drawRange(width, height, range_max, selected_cell_x, selected_cell_y);

                    was_range_visible = true;
                }else if(was_range_visible){
                    fillBG(1, 1, width + 1, height + 1);
                    drawFullGrid(grid);
                    was_range_visible = false;
                }

                move_to(width - (7 + 6), 1);
                printf(COLOR_RED "%02d ❤" RESET COLOR_STANDARD_BG " | " COLOR_YELLOW " % 4d €" RESET, gameStats.health, gameStats.cash);
                if (gameStats.health <= 0)
                {
                    break;
                }
            }
            fflush(stdout);
        }

        /*
        ##############################
        AFFICHAGE DE LA RANGÉE DU HAUT
        ##############################
        */
        if (1.0f / delta_t < 2 * TARGET_FPS)
        {
            move_to(0, 0);
            printf(COLOR_STANDARD_BG);
            info = mallinfo2();
            printf("Enemies: %d/%d | Labels: %d/%d | (%.1f fps) | runtime: %lds | Heap: %zuKo | wave: %d | ennemy left: %d",
                   enemyPool.count,                      //
                   enemyPool.length,                     //
                   labels.count,                         //
                   labels.size,                          //
                   round(1.0f / delta_t * 10.0f) / 10.0, //
                   current_time.tv_sec - time_start,     //
                   info.uordblks / 1000,
                   gameStats.wave,
                   nb_ennemy);
        }

        /*
        ###########################
        GESTION DES ENTREES CLAVIER
        ###########################
        */
        int c = get_key_press();
        if (c != 0)
        {
            if (c == 'q' || c == 'Q')
            {
                break;
            }
            else if (c == ' ') // Touche espace pressée
            {
                // Nettoyage
                if (selection_active)
                {
                    fillBG(1, 1, width + 1, height + 1);
                    drawFullGrid(grid);
                    ligne = 1;
                }
                selection_active = !selection_active;
            }
            else if (c == 27) // Touches spéciales (\033)
            {
                getchar(); // [
                c = getchar();
                int cell_x = selected_cell_x;
                int cell_y = selected_cell_y;
                bool up = c == 'A';
                bool down = c == 'B';
                bool right = c == 'C';
                bool left = c == 'D';
                if (!selection_active)
                {
                    if (up)
                    {
                        while (cell_y > 1 && grid.cells[cell_x][cell_y - 1].type == CHEMIN)
                            cell_y -= 1;

                        if (cell_y > 0)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_y = cell_y - 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                        else
                        {
                            cell_y = grid.height;
                            while (cell_y > 1 && grid.cells[cell_x][cell_y - 1].type == CHEMIN)
                            {
                                cell_y -= 1;
                            }
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_y = cell_y - 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                    }
                    else if (down)
                    {
                        while (cell_y < grid.height - 1 && grid.cells[cell_x][cell_y + 1].type == CHEMIN)
                            cell_y += 1;

                        if (cell_y < grid.height - 1)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_y = cell_y + 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                        else
                        {
                            cell_y = -1;
                            while (cell_y < grid.height - 1 && grid.cells[cell_x][cell_y + 1].type == CHEMIN)
                            {
                                cell_y += 1;
                            }
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_y = cell_y + 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                    }
                    else if (right)
                    {
                        while (cell_x < grid.width - 1 && grid.cells[cell_x + 1][cell_y].type == CHEMIN)
                            cell_x += 1;

                        if (cell_x < grid.width - 1)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_x = cell_x + 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                        else
                        {
                            cell_x = -1;
                            while (cell_x < grid.width - 1 && grid.cells[cell_x + 1][cell_y].type == CHEMIN)
                            {
                                cell_x += 1;
                            }
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_x = cell_x + 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                    }
                    else if (left)
                    {
                        while (cell_x >= 1 && grid.cells[cell_x - 1][cell_y].type == CHEMIN)
                            cell_x -= 1;

                        if (cell_x > 0)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_x = cell_x - 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                        else
                        {
                            cell_x = grid.width;
                            while (cell_x > 1 && grid.cells[cell_x - 1][cell_y].type == CHEMIN)
                            {
                                cell_x -= 1;
                            }
                            grid.cells[selected_cell_x][selected_cell_y].selected = false;
                            selected_cell_x = cell_x - 1;
                            grid.cells[selected_cell_x][selected_cell_y].selected = true;
                        }
                    }
                    drawFullGrid(grid);
                    fflush(stdout);
                }
                else
                {
                    if (down)
                    {
                        ligne += 1;
                    }
                    else if (up)
                    {
                        ligne -= 1;
                    }
                }
            }
            else if (c == 10) // ENTER
            {
                if (selection_active)
                {
                    if (grid.cells[selected_cell_x][selected_cell_y].hasTurret)
                    {
                        int upgrade_price = getTurretPrice(Sniper, grid.cells[selected_cell_x][selected_cell_y].turret.lvl + 1);
                        if (ligne == 1 && upgrade_price > 0 && gameStats.cash >= upgrade_price)
                        {
                            // Upgrade
                            gameStats.cash -= upgrade_price;
                            grid.cells[selected_cell_x][selected_cell_y].turret.lvl = 1;
                        }
                        else if (ligne == 2)
                        {
                            gameStats.cash += getTurretPrice(Sniper, grid.cells[selected_cell_x][selected_cell_y].turret.lvl);
                            grid.cells[selected_cell_x][selected_cell_y].hasTurret = false;
                        }
                    }
                    else
                    {
                        if (ligne == 1 && gameStats.cash >= getTurretPrice(Sniper, 0))
                        {
                            gameStats.cash -= getTurretPrice(Sniper, 0);
                            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Sniper);
                            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
                        }
                        else if (ligne == 2 && gameStats.cash >= getTurretPrice(Inferno, 0))
                        {
                            gameStats.cash -= getTurretPrice(Inferno, 0);
                            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Inferno);
                            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
                        }
                        else if (ligne == 3 && gameStats.cash >= getTurretPrice(Mortier, 0))
                        {
                            gameStats.cash -= getTurretPrice(Mortier, 0);
                            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Mortier);
                            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
                        }
                    }
                    selection_active = false;
                    fillBG(1, 1, width + 1, height + 1);
                    drawFullGrid(grid);
                }
            }
        }
    }

    /*
    ################
    FIN DE LA PARTIE
    ################
    */

    printf("\n");
    clear_screen();
    move_to(0, 0);
    freeGrid(grid);
    freeEnemyPool(enemyPool);
    freeLabels(labels);

    return 0;
}

// 1729518567