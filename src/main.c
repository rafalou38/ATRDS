#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIN_TERMINAL_WIDTH 150
#define MIN_TERMINAL_HEIGHT 50

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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

void get_terminal_size(int *width, int *height)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *width = w.ws_col;
    *height = w.ws_row;
}

void cleanup()
{
    // move_to(0, 0);
    // clear_screen();
    show_cursor();
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
        printf("Merci d'agrandir le terminal ou de dézoomer");
        move_to(width / 2 - 7, height / 2);
        printf("Colones: %d/%d", width, MIN_TERMINAL_WIDTH);
        move_to(width / 2 - 7, height / 2 + 1);
        printf("Rangées: %d/%d", height, MIN_TERMINAL_HEIGHT);

        fflush(stdout);

        usleep(100 * 1000);
    }

    int grid_width = 9;
    int grid_height = 5;

    while (true)
    {
        move_to(0, 0);
        // Si possible, il faudra éviter d'appeler clear_screen, ça fait clignoter l'écran, mieux vaut écrire par dessus. -> quand les mises a jour sont rapides (~1/10s pour réécrire la totalité de l'écran)
        clear_screen();

        for (int y = 0; y < height - 5; y++)
        {
            for (int x = 0; x < (grid_width+1)*(width/(grid_width+1)); x++)
            {
                // +1 pour l’espace; <= 1 pour un espace de 2
                if (x % (grid_width + 1) <= 1 || y % grid_height == 0)
                {
                    printf(" ");
                }
                else
                {
                    if (x % ((grid_width + 1) * 2) < grid_width + 1)
                        printf("🮘");
                    else
                        printf("█");
                }
            }
            printf("\n");
        }

        printf("Ctrl+C pour quitter\ntaille de la grille: %dx%d  carrées: 7x4 9x5 11x6 13x7...\n", grid_width, grid_height);
        printf("Nouvelle taille ? (nxn): ");
        fflush(stdout);

        scanf("%dx%d", &grid_width, &grid_height);
        grid_width = MAX(1, MIN(grid_width, 20));
        grid_height = MAX(1, MIN(grid_height, 20));

        // usleep((1.0f / 60) * 1000 * 1000); // limite a  fps < 60Hz (ne prends pas en compte le delay réel)
    }

    usleep(1000 * 1000);
    return 0;
}
