#include "shared.h"

#include "grid.h"
#include "enemies.h"
#include "tower.h"

#include <string.h>

/*
#################
ÉTAT DU PROGRAMME
#################
*/

Grid grid;
EnemyPool enemyPool;
GameStats gameStats;
Labels labels;
WaveSystem waveSystem;

#if BULLETS_ON
BulletPool bulletPool;
#endif

// est ce q'une cellule a été activé (shop ouvert)
bool selection_active = false;
// vraie quand la range d'une tourelle est affichée (sur la quelle est placée le curseur), pour pouvoir effacer la range une fois qu'elle ne l'est plus
bool was_range_visible = false;
// Position de la selection sur la grille
int selected_cell_x = 0;
int selected_cell_y = 0;

// Position de la selection dans les menus (shop/upgrade)
int ligne = 1;

// Taille actuelle du terminal
int width = 0;
int height = 0;

// Ces fonctions sont définies a la fin du fichier
void handle_arrow_keys();
void handle_enter_key();

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

    // Place le terminal en mode non canonique (entrées envoyées sans enter)
    t_settings.c_lflag &= ~(ICANON);
    // sans echo, les touches pressés ne sont pas affichées.
    t_settings.c_lflag &= ~(ECHO);
    t_settings.c_lflag &= ~(ECHOE);
    t_settings.c_lflag &= ~(ECHOK);
    t_settings.c_lflag &= ~(ECHONL);

    tcsetattr(STDIN_FILENO, TCSANOW, &t_settings);
}

/**
 * Fonction principale du jeu.
 *
 * Initialise le jeu, gère les boucles de jeu, les evenements, les mises à jour, les affichages.
 *
 * Le jeu se termine lorsque la vie de l'utilisateur atteint 0.
 */
int main(int argc, char *argv[])
{
    // Enregistre la fonction cleanup pour qu'elle soit exécutée  la terminaison du programme.
    atexit(cleanup);
    struct sigaction act;
    act.sa_handler = &cleanup;
    sigaction(SIGINT, &act, NULL);

    unsigned int seed = time(NULL);
    printf("seed: %d\n", seed);
    srand(seed);

    /**
     * ##########################
     * Initialisation du terminal
     * ##########################
     */
    configTerminal();
    clear_screen();

    // Vérifie et bloque tant que la taille du terminal ne convient pas
    checkTerminalSize(&width, &height);
    clear_screen();
    fillBG(1, 1, width + 1, height + 1);

    /**
     * ##################################
     * Initialisation des éléments du jeu
     * ##################################
     */
    // Gestion de la grille et du chemin
    grid.width = (width - 2) / (CELL_WIDTH + 2);
    grid.height = (height - 2) / (CELL_HEIGHT + 1);
    allocateGridCells(&grid);
    // Génération du chemin
    genBasicPath(&grid);

    // Gestion des statistiques
    gameStats.cash = 500;
    gameStats.health = 100;

    // Gestion labels
    labels.size = 255;
    labels.labels = malloc(sizeof(struct Label) * labels.size);

    // Gestion ennemis
    enemyPool = AllocEnemyPool();
    switchToWave(&waveSystem, 0);

    /**
     * ###############################
     * Gestion des paramètres spéciaux
     * ###############################
     */

    // La première cellule est initialement sélectionnée
    grid.cells[0][0].selected = true;

    /**
     * ###############################
     * Gestion des paramètres spéciaux
     * ###############################
     */
    if (argc > 1)
    {
        if (strcmp(argv[1], "--sim-wave") == 0)
        {
            int n = 0;
            if (argc == 3)
                n = atoi(argv[2]);
            testWaveSystem(grid, &enemyPool, n);
        }
        return 0;
    }

    // Affichage intégral de la grille
    drawFullGrid(grid);

    // Timers du jeu
    struct timespec prev_time;
    clock_gettime(CLOCK_MONOTONIC, &prev_time);
    time_t time_start = prev_time.tv_sec;
    struct mallinfo2 info;

    // size_t est un int de capacité plus élevée.
    size_t frame_index;

    // Le jeu tourne tant que la vie n'atteint pas 0
    while (gameStats.health > 0)
    {
        // Gestion du temps
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        // Ici on calcule dt (écart de temps entre cette frame et la précédente)
        float delta_t = (current_time.tv_nsec - prev_time.tv_nsec) / 1000000000.0 + (current_time.tv_sec - prev_time.tv_sec);
        prev_time = current_time;
        // On attends le temps restant pour avoir un FPS fixe
        if (delta_t < 1.0 / TARGET_FPS)
            msleep((1.0 / TARGET_FPS - delta_t) * 1000);
        if (delta_t > 3.0 / TARGET_FPS) // ici on évite d'avoir un dt trop énorme ce qui peut poser des problèmes
            delta_t = 3.0 / TARGET_FPS;

        frame_index++;

        /*
        ###############################
        AFFICHAGE ET MISE A JOUR DU JEU
        ###############################
        */

        if (selection_active) // MENU DE SELECTION
        {
            showTowerSelection(ligne, grid.cells[selected_cell_x][selected_cell_y].hasTurret, grid.cells[selected_cell_x][selected_cell_y].turret);
        }
        else // JEU NORMAL
        {
#if BULLETS_ON
            // Cas ou les misilles sont activées

            if (frame_index % 50 == 0) // Toutes les 50 frames, on efface l'écran pour supprimer les artefacts restants
            {
                fillBG(1, 1, width + 1, height + 1);
                drawFullGrid(grid);
            }
            else // Le reste du temps, on efface uniquement les cases du chemin qui ont été mises a jour
            {
                clearUsedPath(grid, enemyPool);
            }

            // Update towers gère l'affichage et les animations des tourelles, il s'occupe aussi de tirer sur les ennemis a portée le moment venu pour chacune des tourelles
            updateTowers(grid, enemyPool, &bulletPool, delta_t, &gameStats);

            // Si les bullets sont activées, on les mets a jour (gestion de leur mouvement)
            updateBullets(&bulletPool, delta_t);
            // Puis on les affiche
            drawBullets(bulletPool);
#else
            // Si les bullets ne sont pas activées, on gère uniquement les tourelles
            updateTowers(grid, enemyPool, delta_t, &gameStats);
            clearUsedPath(grid, enemyPool);
#endif

            // Mis a jour des labels (suppression des vieux)
            updateLabels(&labels, delta_t);
            drawLabels(labels);

            // Mise à jour des ennemis existants
            updateEnemies(&enemyPool, grid, &gameStats, &labels, delta_t);
            // Affichage des ennemis
            drawEnemies(enemyPool, grid);

            // Affichage de la portée des tourelles
            if (grid.cells[selected_cell_x][selected_cell_y].hasTurret)
            {
                float range_min = grid.cells[selected_cell_x][selected_cell_y].turret.range_min[grid.cells[selected_cell_x][selected_cell_y].turret.lvl];
                float range_max = grid.cells[selected_cell_x][selected_cell_y].turret.range_max[grid.cells[selected_cell_x][selected_cell_y].turret.lvl];
                if (range_min > 0)
                    drawRange(width, height, range_min, selected_cell_x + 0.5, selected_cell_y + 0.5, false);
                if (range_max > 0)
                    drawRange(width, height, range_max, selected_cell_x + 0.5, selected_cell_y + 0.5, false);

                was_range_visible = true;
            }
            else if (was_range_visible)
            {
                fillBG(1, 1, width + 1, height + 1);
                drawFullGrid(grid);
                was_range_visible = false;
            }
        }

        // Mise a jour de l'affichage
        fflush(stdout);

        /*
        ##############################
        AFFICHAGE DE LA RANGÉE DU HAUT
        ##############################
        */
        if (1.0f / delta_t < 2 * TARGET_FPS) // evite d'afficher les valeurs aberrantes de FPS
        {
            move_to(0, 0);
            printf(COLOR_STANDARD_BG);
            info = mallinfo2();
            printf("Enemies: %d/%d | Labels: %d/%d | (%.1f fps) | runtime: %lds | Heap: %zuKo | wave: %d",
                   enemyPool.count,                      //
                   enemyPool.length,                     //
                   labels.count,                         //
                   labels.size,                          //
                   round(1.0f / delta_t * 10.0f) / 10.0, //
                   current_time.tv_sec - time_start,     //
                   info.uordblks / 1000,
                   0);
        }
        // Affichage des stats de jeu (vie et argent)
        move_to(width - (7 + 10), 1);
        printf(COLOR_RED "%02d ❤" RESET COLOR_STANDARD_BG " | " COLOR_YELLOW " % 4d €" RESET, gameStats.health, gameStats.cash);

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
                ligne = 1;
                selection_active = !selection_active;
            }
            else if (c == 27) // Touches directionnelles commencent par le char (\033)

                handle_arrow_keys();

            else if (c == 10 && selection_active) // ENTER
                handle_enter_key();
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

void handle_enter_key()
{
    if (grid.cells[selected_cell_x][selected_cell_y].hasTurret)
    {
        int upgrade_price = getTurretPrice(grid.cells[selected_cell_x][selected_cell_y].turret.type, grid.cells[selected_cell_x][selected_cell_y].turret.lvl + 1);
        if (ligne == 1 && upgrade_price > 0 && gameStats.cash >= upgrade_price)
        {
            // Upgrade
            gameStats.cash -= upgrade_price;
            grid.cells[selected_cell_x][selected_cell_y].turret.lvl = 1;
        }
        else if (ligne == 2)
        {
            gameStats.cash += getTurretPrice(grid.cells[selected_cell_x][selected_cell_y].turret.type, grid.cells[selected_cell_x][selected_cell_y].turret.lvl);
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
        else if (ligne == 4 && gameStats.cash >= getTurretPrice(Gatling, 0))
        {
            gameStats.cash -= getTurretPrice(Gatling, 0);
            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Gatling);
            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
        }
        else if (ligne == 5 && gameStats.cash >= getTurretPrice(Petrificateur, 0))
        {
            gameStats.cash -= getTurretPrice(Petrificateur, 0);
            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Petrificateur);
            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
        }
        else if (ligne == 6 && gameStats.cash >= getTurretPrice(Freezer, 0))
        {
            gameStats.cash -= getTurretPrice(Freezer, 0);
            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Freezer);
            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
        }
        else if (ligne == 7 && gameStats.cash >= getTurretPrice(Banque, 0))
        {
            gameStats.cash -= getTurretPrice(Banque, 0);
            grid.cells[selected_cell_x][selected_cell_y].turret = getTurretStruct(Banque);
            grid.cells[selected_cell_x][selected_cell_y].hasTurret = true;
        }
    }
    selection_active = false;
    fillBG(1, 1, width + 1, height + 1);
    drawFullGrid(grid);
}

void handle_arrow_keys()
{
    getchar(); // [
    char c = getchar();
    int cell_x = selected_cell_x;
    int cell_y = selected_cell_y;
    bool up = c == 'A';
    bool down = c == 'B';
    bool right = c == 'C';
    bool left = c == 'D';
    if (!selection_active)
    {
        // Mouvement de la sélection de cellule, en prenant en compte le chemin et les bords de la grid
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
        // Mouvement du curseur dans le shop
        if (down)
        {
            ligne += 1;
        }
        else if (up)
        {
            ligne -= 1;
        }

        if (selection_active)
        { // le shop est ouvert
            // 7 tourelles => 7 lignes sélectionnables
            ligne = CLAMP(ligne, 1, 7);
        }
        else
        {
            // c'est le menu d'upgrade
            // Deux options: améliorer ou vendre
            ligne = CLAMP(ligne, 1, 2);
        }
    }
}