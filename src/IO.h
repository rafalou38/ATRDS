#pragma once
#include "shared.h"

// Liste des codes: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
void move_to(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}
void clear_screen()
{
    printf("\033[2J");
}
void hide_cursor()
{
    printf("\033[?25l");
}
void show_cursor()
{
    printf("\033[?25h");
}

void printCritical(char *errorMessage){
    printf("\033[91;4mCRITICAL\033[24m: %s \033[0m\n", errorMessage);
}


void get_terminal_size(int *width, int *height)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *width = w.ws_col;
    *height = w.ws_row;
}

void checkTerminalSize(int *width, int *height)
{
    get_terminal_size(width, height);

    while (*width < MIN_TERMINAL_WIDTH || *height < MIN_TERMINAL_HEIGHT)
    {
        clear_screen();

        move_to(*width / 2 - 20, *height / 2 - 1);
        printf("Merci d'agrandir le terminal ou de dézoomer avec la commande \"ctrl -\"");
        move_to(*width / 2 - 7, *height / 2);
        printf("Colones: %d/%d", width, MIN_TERMINAL_WIDTH);
        move_to(*width / 2 - 7, *height / 2 + 1);
        printf("Rangées: %d/%d", *height, MIN_TERMINAL_HEIGHT);

        get_terminal_size(width, height);

        fflush(stdout);

        usleep(1000);
    }
}

void drawCell(struct Cell cell, struct Cell** grid, int gridwidth, int gridheight)
{
    if (cell.type == TERRAIN)
    {
        bool neighbor = false;
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (cell.x + dx >= 0 && cell.y + dy >= 0 && cell.x + dx < gridwidth && cell.y + dy < gridheight && grid[cell.x + dx][cell.y + dy].type == CHEMIN)
                {
                    neighbor = true;
                }
            }
        }

        if (neighbor)
        {
            printf("\033[100m");
        }
        for (int y = 0; y < CELL_HEIGHT; y++)
        {
            int terminal_x = cell.x * (CELL_WIDTH + GAP) + 3;
            int terminal_y = cell.y * (CELL_HEIGHT + GAP / 2) + 2;

            move_to(terminal_x, terminal_y + y);

            for (int x = 0; x < CELL_WIDTH; x++)
            {
                printf(" ");
            }
        }
        printf("\033[0m");
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
                bool side = (cell.x == 0 && x == 0) || (cell.x == gridwidth - 1 && x == CELL_WIDTH + 1);
                if (
                    !side && (x == 0 && y == 0 && !chemin_vers_gauche && !chemin_vers_haut                                              //
                              || (x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && chemin_vers_bas && chemin_vers_droite)                 //
                              || (x == CELL_WIDTH + 1 && y == 0 && chemin_vers_haut_droit && chemin_vers_haut && !chemin_vers_droite))) //
                    //
                    printf("╭");
                else if (!side && ((x == CELL_WIDTH + 1 && y == 0 && !chemin_vers_droite && !chemin_vers_haut)  //
                                   || (x == 0 && y == CELL_HEIGHT + 1 && chemin_vers_gauche && chemin_vers_bas) //
                                   || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_haut_gauche && !chemin_vers_gauche)))
                    //
                    printf("╮");
                else if (!side && (x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && !chemin_vers_droite && !chemin_vers_bas //
                                   || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_gauche && !chemin_vers_haut_gauche)))
                    //
                    printf("╯");
                else if (!side && (x == 0 && y == CELL_HEIGHT + 1 && !chemin_vers_gauche && !chemin_vers_bas //
                                   || (y == 0 && x == CELL_WIDTH + 1 && chemin_vers_haut && chemin_vers_droite && !chemin_vers_haut_droit)))
                    //
                    printf("╰");
                else if (!((cell.x == 0 && x == 0) || (cell.x == gridwidth - 1 && x == CELL_WIDTH + 1)) && ((x == 0 && !chemin_vers_gauche) //
                                                                                                            || (x == CELL_WIDTH + 1 && !chemin_vers_droite)))
                    //
                    printf("│");
                else if (((y == 0 && !chemin_vers_haut) //
                          || (y == CELL_HEIGHT + 1 && !chemin_vers_bas)))
                    //
                    printf("─");
                else if (x >= 1 && x <= CELL_WIDTH && y >= 1 && y <= CELL_HEIGHT)
                {
                    // printf("\033[104m");
                    printf(" ");
                    // printf("\033[0m");
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
