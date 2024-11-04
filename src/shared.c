#include "shared.h"

int msleep(long ms) // Fonction de repos (temps pendant lequel le terminal ne lis pas la suite du programme), ici en millisecondes
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    return nanosleep(&ts, &ts);
}

void updateLabels(Labels *labels, float dt) // Mets a jour les labels (+1$)
{
    for (int i = 0; i < labels->count; i++)
    {
        labels->labels[i].counter += dt;
    }
    int right = 0;
    int left = -1;

    while (right < labels->count)
    {
        if (labels->labels[right].counter < labels->labels[right].duration)
        {
            if (left != -1)
            {
                assert(left >= 0);
                // assert(left < labels->length);

                labels->labels[left] = labels->labels[right];
                left++;
            }
        }
        else if (left == -1)
        {
            left = right;
        }

        right++;
    }
    if (left != -1)
    {
        labels->count -= right - left;
    }
    if (labels->count < 0)
        labels->count = 0;
};

void drawLabels(Labels labels) // affiche tous les labels
{
    for (int i = 0; i < labels.count; i++)
    {
        move_to(labels.labels[i].x, labels.labels[i].y);
        printf(labels.labels[i].text);
        // printf("%.2f", labels.labels[i].counter);
    }
};

void freeLabels(Labels labels) // nettoie le tableau des labels
{
    printf(COLOR_GRAY " $ " RESET "Freeing %s%d%s labels:\t", COLOR_YELLOW, labels.count, RESET);
    free(labels.labels);

    printf("%s Done %s\n", COLOR_GREEN, RESET);
};

/*
#############
##    IO   ##
#############
*/

// Lecture des inputs de l'utilisateur
char get_key_press() 
{
    int k = 0;
    // Récupère le nombre de characters en attente d’être lus
    ioctl(STDIN_FILENO, FIONREAD, &k);

    if (k > 0)
        return getchar();
    else
        return 0;
}

// Les fonctions suivantes sont claires de par leurs noms (les "\033[" indiquent des actions sur l'affichage (voir README et les codes ANSI))
void move_to(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

void clear_screen()
{
    printf("\033[2J");
}

void clear_line()
{
    printf("\033[2K");
}

void hide_cursor()
{
    printf("\033[?25l");
}

void show_cursor()
{
    printf("\033[?25h");
}

void printCritical(char *errorMessage)
{
    printf("%s%sCRITICAL%s: %s %s %s\n", COLOR_RED, UNDERLINE, UNDERLINE_RST, BOLD, errorMessage, RESET);
}

// Récupération de la taille du terminale
void get_terminal_size(int *width, int *height) 
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *width = w.ws_col;
    *height = w.ws_row;
}

// Oblige l'utilisateur à se mettre dans une bonne configuration de terminal pour jouer
void checkTerminalSize(int *width, int *height) 
{
    get_terminal_size(width, height);

    while (*width < MIN_TERMINAL_WIDTH || *height < MIN_TERMINAL_HEIGHT)
    {
        clear_screen();

        move_to(*width / 2 - 20, *height / 2 - 1);
        printf("Merci d'agrandir le terminal ou de dézoomer avec la commande \"ctrl -\"");
        move_to(*width / 2 - 7, *height / 2);
        printf("Colones: %d/%d", *width, MIN_TERMINAL_WIDTH);
        move_to(*width / 2 - 7, *height / 2 + 1);
        printf("Rangées: %d/%d", *height, MIN_TERMINAL_HEIGHT);

        get_terminal_size(width, height);

        fflush(stdout);

        msleep(200);
    }
}

// Dessine les cercles de portée des tourelles
void drawRange(int term_width, int term_height, float range, float grid_x, float grid_y, bool fill)
{
    printf(COLOR_STANDARD_BG);

    for (int y = 0; y < term_height; y++)
    {
        for (int x = 0; x < term_width; x++)
        {
            float current_grid_x = (x - 3.0f) / (CELL_WIDTH + GAP);
            float current_grid_y = (y - 2.0f) / (CELL_HEIGHT + GAP / 2);

            float dx = grid_x - current_grid_x;
            float dy = grid_y - current_grid_y;
            float d = sqrt(dx * dx + dy * dy);

            if (d < range + 0.05 && (d > range - 0.06 || fill))
            {
                int term_x = x;
                int term_y = y;
                if (term_x > 0 && term_y > 0 && term_x < term_width && term_y < term_height)
                {
                    move_to(term_x, term_y);
                    printf("•");
                }
            }
        }
    }
}