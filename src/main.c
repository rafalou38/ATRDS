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
};

/*
#############
CONFIGURATION
#############
*/
#define MIN_TERMINAL_WIDTH 140
#define MIN_TERMINAL_HEIGHT 30

#define CELL_WIDTH 6
#define CELL_HEIGHT 3

#define GAP 2

/*
#################
ÉTAT DU PROGRAMME
#################
*/
int gridheight = 7;
int gridwidth = 7;
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

            printf("\033[42m");
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
        bool chemin_vers_haut = false;
        bool chemin_vers_droite = false;
        bool chemin_vers_bas = false;
        bool chemin_vers_gauche = false;
        if (cell.x + 1 < gridwidth && grid[cell.x + 1][cell.y].type == CHEMIN)
            chemin_vers_droite = true;
        if (cell.x - 1 >= 0 && grid[cell.x - 1][cell.y].type == CHEMIN)
            chemin_vers_gauche = true;
        if (cell.y - 1 >= 0 && grid[cell.x][cell.y - 1].type == CHEMIN)
            chemin_vers_haut = true;
        if (cell.y + 1 < gridheight && grid[cell.x][cell.y + 1].type == CHEMIN)
            chemin_vers_bas = true;

        for (int y = 0; y < CELL_HEIGHT; y++)
        {
            int terminal_x = (cell.x * (CELL_WIDTH + GAP) + 3); // -1 car le chemin est décalé
            int terminal_y = (cell.y * (CELL_HEIGHT + GAP / 2) + 2); // -1 car le chemin est décalé

            move_to(terminal_x, terminal_y + y);
            printf("\033[104m");

            for (int x = 0; x < CELL_WIDTH; x++)
            {
                if (x == 0 && y == 0 && !chemin_vers_gauche && !chemin_vers_haut || (x == CELL_WIDTH - 1 && y == CELL_HEIGHT - 1 && chemin_vers_bas && chemin_vers_droite))
                    printf("┌");
                else if ((x == CELL_WIDTH - 1 && y == 0 && !chemin_vers_droite && !chemin_vers_haut) || (x == 0 && y == CELL_HEIGHT - 1 && chemin_vers_gauche && chemin_vers_bas))
                    printf("┐");
                else if (x == CELL_WIDTH - 1 && y == CELL_HEIGHT - 1 && !chemin_vers_droite && !chemin_vers_bas || (x==0 && y==0 && chemin_vers_haut && chemin_vers_gauche))
                    printf("┘");
                else if (x == 0 && y == CELL_HEIGHT - 1 && !chemin_vers_gauche && !chemin_vers_bas || (y == 0 && x == CELL_WIDTH - 1 && chemin_vers_haut && chemin_vers_droite))
                    printf("└");
                else if ((x == 0 && !chemin_vers_gauche) || (x == CELL_WIDTH - 1 && !chemin_vers_droite))
                    printf("│");
                else if ((y == 0 && !chemin_vers_haut) || (y == CELL_HEIGHT - 1 && !chemin_vers_bas))
                    printf("─");
                else
                    printf(" ");
            }


            move_to(terminal_x + CELL_WIDTH/2, terminal_y + CELL_HEIGHT/2);
            printf("🐧");
            
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
        printf("Merci d'agrandir le terminal ou de dézoomer avec la commande \"ctrl -\"");
        move_to(width / 2 - 7, height / 2);
        printf("Colones: %d/%d", width, MIN_TERMINAL_WIDTH);
        move_to(width / 2 - 7, height / 2 + 1);
        printf("Rangées: %d/%d", height, MIN_TERMINAL_HEIGHT);

        fflush(stdout);

        usleep(100 * 1000);
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
        }
    }

    grid[3][1].type = CHEMIN;
    grid[3][2].type = CHEMIN;
    grid[3][3].type = CHEMIN;
    grid[3][4].type = CHEMIN;
    grid[4][4].type = CHEMIN;
    grid[5][4].type = CHEMIN;
    grid[2][4].type = CHEMIN;
    grid[1][4].type = CHEMIN;
    grid[1][3].type = CHEMIN;
    grid[1][2].type = CHEMIN;
    grid[2][2].type = CHEMIN;
    grid[5][5].type = CHEMIN;
    grid[4][3].type = CHEMIN;

    grid[3][0].type = CHEMIN;
    grid[4][0].type = CHEMIN;
    grid[5][0].type = CHEMIN;
    grid[5][1].type = CHEMIN;
    grid[6][1].type = CHEMIN;

    for (int x = 0; x < gridheight; x++)
    {
        for (int y = 0; y < gridwidth; y++)
        {
            drawCell(grid[x][y]);
        }
    }
    fflush(stdout);

    return 0;
}
