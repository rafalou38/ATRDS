#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "IO.h"

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
    bool visited;
};

/*
#############
CONFIGURATION
#############
*/
#define MIN_TERMINAL_WIDTH 120
#define MIN_TERMINAL_HEIGHT 20

#define CELL_WIDTH 6
#define CELL_HEIGHT 3

#define GAP 2

/*
#################
ÉTAT DU PROGRAMME
#################
*/
int gridheight = 10;
int gridwidth = 10;
struct Cell **grid;

void drawCell(struct Cell cell)
{
    if (cell.type == TERRAIN)
    {
        for (int y = 0; y < CELL_HEIGHT; y++)
        {
            // printf("%d")
            int terminal_x = cell.x * (CELL_WIDTH + GAP) + 3;
            int terminal_y = cell.y * (CELL_HEIGHT + GAP / 2) + y + 2;

            move_to(terminal_x, terminal_y);

            if (cell.visited)
            {

                printf("\033[43m");
            }
            else
            {

                printf("\033[42m");
            }
            for (int x = 0; x < CELL_WIDTH; x++)
            {
                printf(" ");
            }
            printf("\033[0m");
        }
    }
    if (cell.type == CHEMIN)
    {
        // Détermination du type de chemin
        bool chemin_vers_haut = cell.y - 1 >= 0 && grid[cell.x][cell.y - 1].type == CHEMIN;
        bool chemin_vers_droite = cell.x + 1 < gridwidth && grid[cell.x + 1][cell.y].type == CHEMIN;
        bool chemin_vers_bas = cell.y + 1 < gridheight && grid[cell.x][cell.y + 1].type == CHEMIN;
        bool chemin_vers_gauche = cell.x - 1 >= 0 && grid[cell.x - 1][cell.y].type == CHEMIN;

        bool chemin_vers_haut_gauche = cell.x - 1 >= 0 && cell.y - 1 >= 0 && grid[cell.x - 1][cell.y - 1].type == CHEMIN;
        bool chemin_vers_haut_droit = cell.x + 1 < gridwidth && cell.y - 1 >= 0 && grid[cell.x + 1][cell.y - 1].type == CHEMIN;

        for (int y = 0; y < CELL_HEIGHT + 2; y++)
        {
            int terminal_x = (cell.x * (CELL_WIDTH + GAP) + 3);
            int terminal_y = (cell.y * (CELL_HEIGHT + GAP / 2) + 2);

            move_to(terminal_x - 1, terminal_y + y - 1);

            for (int x = 0; x < CELL_WIDTH + 2; x++)
            {
                if (
                    x == 0 && y == 0 && !chemin_vers_gauche && !chemin_vers_haut                                             //
                    || (x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && chemin_vers_bas && chemin_vers_droite)                //
                    || (x == CELL_WIDTH + 1 && y == 0 && chemin_vers_haut_droit && chemin_vers_haut && !chemin_vers_droite)) //
                    //
                    printf("╭");
                else if ((x == CELL_WIDTH + 1 && y == 0 && !chemin_vers_droite && !chemin_vers_haut)  //
                         || (x == 0 && y == CELL_HEIGHT + 1 && chemin_vers_gauche && chemin_vers_bas) //
                         || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_haut_gauche && !chemin_vers_gauche))
                    //
                    printf("╮");
                else if (x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && !chemin_vers_droite && !chemin_vers_bas //
                         || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_gauche && !chemin_vers_haut_gauche))
                    //
                    printf("╯");
                else if (x == 0 && y == CELL_HEIGHT + 1 && !chemin_vers_gauche && !chemin_vers_bas //
                         || (y == 0 && x == CELL_WIDTH + 1 && chemin_vers_haut && chemin_vers_droite && !chemin_vers_haut_droit))
                    //
                    printf("╰");
                else if (((x == 0 && !chemin_vers_gauche) //
                          || (x == CELL_WIDTH + 1 && !chemin_vers_droite)))
                    //
                    printf("│");
                else if (((y == 0 && !chemin_vers_haut) //
                          || (y == CELL_HEIGHT + 1 && !chemin_vers_bas)))
                    //
                    printf("─");
                else if (x >= 1 && x <= CELL_WIDTH && y >= 1 && y <= CELL_HEIGHT)
                {
                    printf("\033[104m");
                    printf(" ");
                    printf("\033[0m");
                }
                else
                {
                    printf(" ");
                }
            }

            move_to(terminal_x + CELL_WIDTH / 2 - 1, terminal_y + CELL_HEIGHT / 2);
            printf("🐧");
        }
    }
}

int main()
{
    srand(8);
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
        printf("Merci d'agrandir le terminal ou de dézoomer avec la commande \"ctrl -\"");
        move_to(width / 2 - 7, height / 2);
        printf("Colones: %d/%d", width, MIN_TERMINAL_WIDTH);
        move_to(width / 2 - 7, height / 2 + 1);
        printf("Rangées: %d/%d", height, MIN_TERMINAL_HEIGHT);

        fflush(stdout);

        // usleep(1000);
    }

    /*
        ###########################
        INITIALISATION DE LA GRILLE
        ###########################
    */
    grid = malloc(sizeof(struct Cell *) * gridwidth); // Première coordonnée: x / largeur
    for (int x = 0; x < gridheight; x++)
    {

        grid[x] = malloc(sizeof(struct Cell) * gridheight); // deuxième coordonnée: y / hauteur

        for (int y = 0; y < gridwidth; y++)
        {
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].type = TERRAIN;
            grid[x][y].visited = false;
        }
    }

    // Prototype de création automatique du chemin
    int variable_arret1 = 0;
    int chemin_x = 1;
    int chemin_y = (rand() % (gridheight));
    if (chemin_y == gridheight - 1) // Rectifications du random pour pas la 1ere ni la derniere ligne
    {
        chemin_y = chemin_y - 1;
    }
    if (chemin_y == 0)
    {
        chemin_y = chemin_y + 1;
    }

    grid[0][chemin_y].type = CHEMIN; // 2 premieres cases sans les autres options, fixées à une hauteur aléatoire
    grid[chemin_x][chemin_y].type = CHEMIN;

    int historique[100][2];
    int history_index = 0;

    while (variable_arret1 == 0)
    {
        // conditions pour le chemin + randomisation de la verticalité (?)
        bool posible_bas = chemin_y + 1 < gridheight - 1                      //
                           && grid[chemin_x - 1][chemin_y + 1].type != CHEMIN // bas gauche
                           && grid[chemin_x + 1][chemin_y + 1].type != CHEMIN // bas droite
                           && grid[chemin_x][chemin_y + 1].type != CHEMIN     // bas
                           && !grid[chemin_x][chemin_y + 1].visited;

        bool possible_haut = chemin_y - 1 > 1                                   //
                             && grid[chemin_x - 1][chemin_y - 1].type != CHEMIN // haut gauche
                             && grid[chemin_x + 1][chemin_y - 1].type != CHEMIN // haut droit
                             && grid[chemin_x][chemin_y - 1].type != CHEMIN     // haut
                             && !grid[chemin_x][chemin_y - 1].visited;

        bool possible_droite = grid[chemin_x + 1][chemin_y + 1].type != CHEMIN    // droite bas
                               && grid[chemin_x + 1][chemin_y - 1].type != CHEMIN // droite haut
                               && grid[chemin_x + 1][chemin_y].type != CHEMIN     // droite
                               && !grid[chemin_x + 1][chemin_y + 1].visited;

        bool possible_gauche = chemin_x - 1 > 1                                   //
                               && grid[chemin_x - 1][chemin_y + 1].type != CHEMIN // gauche bas
                               && grid[chemin_x - 1][chemin_y - 1].type != CHEMIN // gauche haut
                               && grid[chemin_x - 1][chemin_y].type != CHEMIN     // gauche
                               && !grid[chemin_x - 1][chemin_y].visited;

        if (!posible_bas && !possible_haut && !possible_droite && !possible_gauche)
        {
            grid[chemin_x][chemin_y].type = TERRAIN;
            history_index--;
            chemin_x = historique[history_index][0];
            chemin_y = historique[history_index][1];
            for (int x = 0; x < gridheight; x++)
            {
                for (int y = 0; y < gridwidth; y++)
                {
                    drawCell(grid[x][y]);
                    fflush(stdout);
                    // usleep(200* 1000);
                }
            }
            usleep(100000);
            fflush(stdout);
            continue;
        }

        while (true)
        {
            int random_chemin = rand() % 4;
            if (random_chemin == 0 && posible_bas)
            {
                chemin_y++;
                break;
            }
            if (random_chemin == 1 && possible_haut)
            {
                chemin_y--;
                break;
            }
            if (random_chemin == 2 && possible_droite)
            {
                chemin_x++;
                break;
            }
            if (random_chemin == 3 && possible_gauche)
            {
                chemin_x--;
                break;
            }
        }

        historique[history_index][0] = chemin_x;
        historique[history_index][1] = chemin_y;
        history_index++;

        grid[chemin_x][chemin_y].type = CHEMIN;
        grid[chemin_x][chemin_y].visited = true;

        if (chemin_x == gridwidth - 1)
        {
            variable_arret1 = 69;
        }
        drawCell(grid[chemin_x][chemin_y]);
        for (int x = 0; x < gridheight; x++)
        {
            for (int y = 0; y < gridwidth; y++)
            {
                drawCell(grid[x][y]);
                fflush(stdout);
                // usleep(200* 1000);
            }
        }
        usleep(100000);
        fflush(stdout);
    } // FIN  Génération de CHEMIN

    for (int x = 0; x < gridheight; x++)
    {
        for (int y = 0; y < gridwidth; y++)
        {
            drawCell(grid[x][y]);
            fflush(stdout);
            // usleep(200* 1000);
        }
    }

    printf("\n\n\n\n");

    return 0;
}
