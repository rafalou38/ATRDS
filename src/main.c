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

    grid.pv = 10000;

    bool selection_active = false;
    int selected_cell_x = 0;
    int selected_cell_y = 0;
    int ligne = 1;
    grid.cells[0][0].selected = true;

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
                showTowerSelection(ligne, grid.cells[selected_cell_x][selected_cell_y].hasTurret);
            }
            else
            {
                // JEU NORMAL

                // Spawn des ennemis
                spawnTimer += delta_t;
                if (spawnTimer > 8.0)
                {
                    addEnemy(grid, &enemyPool, ENEMY_TUX, grid.start_x, grid.start_y);
                    spawnTimer = 0.0;
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

                // Mise à jour des ennemis existants
                updateEnemies(&enemyPool, grid, delta_t);
                // Affichage des ennemis
                drawEnemies(enemyPool, grid);
                
                move_to(0,height);
                grid.pv -= 1;
                printf("Pv restants : %d",grid.pv);
                if (grid.pv<=0)
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
            printf("Enemy count %d/%d | (%.1f fps) | runtime: %lds | Heap: %zuKo",
                   enemyPool.count,                      //
                   enemyPool.length,                     //
                   round(1.0f / delta_t * 10.0f) / 10.0, //
                   current_time.tv_sec - time_start,     //
                   info.uordblks / 1000);
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
                        if (ligne == 1)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].turret.lvl = 1;
                        }
                    }
                    else
                    {
                        if (ligne == 1)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].turret.type = Sniper;
                            grid.cells[selected_cell_x][selected_cell_y].turret.lvl = 0;
                            grid.cells[selected_cell_x][selected_cell_y].turret.compteur = 0;
                            grid.cells[selected_cell_x][selected_cell_y].turret.range[0] = 100;
                            grid.cells[selected_cell_x][selected_cell_y].turret.range[1] = 100;
                            grid.cells[selected_cell_x][selected_cell_y].turret.damage[0] = 1;
                            grid.cells[selected_cell_x][selected_cell_y].turret.damage[1] = 1.5;
                            grid.cells[selected_cell_x][selected_cell_y].turret.reload_delay[0] = 0.5;
                            grid.cells[selected_cell_x][selected_cell_y].turret.reload_delay[1] = 0.6;
                            grid.cells[selected_cell_x][selected_cell_y].turret.nb_ennemi[0] = 1;
                            grid.cells[selected_cell_x][selected_cell_y].turret.nb_ennemi[1] = 2;
                            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
                        }
                        else if (ligne == 2)
                        {
                            grid.cells[selected_cell_x][selected_cell_y].turret.type = Inferno;
                            grid.cells[selected_cell_x][selected_cell_y].turret.lvl = 0;
                            grid.cells[selected_cell_x][selected_cell_y].turret.compteur = 0;
                            grid.cells[selected_cell_x][selected_cell_y].turret.range[0] = 1;
                            grid.cells[selected_cell_x][selected_cell_y].turret.range[1] = 2;
                            grid.cells[selected_cell_x][selected_cell_y].turret.damage[0] = 0.2;
                            grid.cells[selected_cell_x][selected_cell_y].turret.damage[1] = 0.3;
                            grid.cells[selected_cell_x][selected_cell_y].turret.reload_delay[0] = 1.5;
                            grid.cells[selected_cell_x][selected_cell_y].turret.reload_delay[1] = 2;
                            grid.cells[selected_cell_x][selected_cell_y].turret.nb_ennemi[0] = 4;
                            grid.cells[selected_cell_x][selected_cell_y].turret.nb_ennemi[1] = 8;
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

    return 0;
}

// 1729518567