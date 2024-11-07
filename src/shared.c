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

    // On parcourt le tableau des labels, on supprime les labels qui ont
    // dépassé leur durée de vie, et on décale les labels restants
    // vers la gauche.
    while (right < labels->count)
    {
        if (labels->labels[right].counter < labels->labels[right].duration)
        {
            // Si le label est encore valide et que celui de gauche est un un trou, on déplace celui de droite vers la gauche.
            if (left != -1)
            {
                // On copie le label de droite dans le label de gauche,
                // car on va libérer la mémoire du label de gauche.
                assert(left >= 0);
                labels->labels[left] = labels->labels[right];
                left++;
            }
        }
        else if (left == -1)
        {
            // Si c'est le premier label invalide, on le note comme étant
            // le premier label que l'on va supprimer.
            left = right;
        }

        right++;
    }
    if (left != -1)
    {
        // On met a jour le compteur du nombre de labels,
        // car on a supprimé des labels.
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
        printf(COLOR_STANDARD_BG COLOR_YELLOW "%d" RESET,labels.labels[i].text);
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

// Animation de début de jeu

void anim_debut(int term_width, int term_height)
{
    int bro_this_variable_is_actually_an_easter_egg = 0;
    if (term_width % 2 == 1)
    {
        bro_this_variable_is_actually_an_easter_egg = 1;
        term_width -= 1;
    }
    int x_wanted = 0;
    int y_wanted = 0;
    for (int n = 0; n < term_width / 2; n++)
    {
        for (int y = 0; y < term_height - 1; y++)
        {
            for (int x = 0; x < term_width - 1; x = x + 2)
            {
                move_to(x, y);
                if (x == x_wanted || y == y_wanted || x == term_width - x_wanted || y == term_height - y_wanted)
                {
                    printf("██");
                }
                else
                {
                    printf("  ");
                }
                // move_to(term_width / 2, term_height / 2);
                // printf("%d|%d", x_wanted, y_wanted);
            }
        }
        x_wanted += 2;
        y_wanted += 1;
        fflush(stdout);
        msleep(5);
    }
    x_wanted = term_height * 2;
    y_wanted = term_height / 2;
    for (int n = 0; n < term_width / 4; n++)
    {
        for (int y = 0; y < term_height - 1; y++)
        {
            for (int x = 0; x < term_width - 1; x = x + 2)
            {
                move_to(x, y);
                if ((x == x_wanted && y < y_wanted) || (y == y_wanted && x < x_wanted) || (x == term_width - x_wanted && y > term_height - y_wanted) || (y == term_height - y_wanted && x > term_width - x_wanted))
                {
                    printf("██");
                }
                else
                {
                    printf("  ");
                }
                // move_to(term_width / 2, term_height / 2);
                // printf("%d|%d", x_wanted, y_wanted);
            }
        }
        x_wanted += 2;
        y_wanted += 1;
        fflush(stdout);
        msleep(5);
    }
    x_wanted = term_height * 2;
    y_wanted = term_height / 2;
    for (int n = 0; n < term_width / 4; n++)
    {
        for (int y = 0; y < term_height - 1; y++)
        {
            for (int x = 0; x < term_width - 1; x = x + 2)
            {
                move_to(x, y);
                if ((x == x_wanted && y > term_height - y_wanted) || (y == y_wanted && x > term_width - x_wanted) || (x == term_width - x_wanted && y < y_wanted) || (y == term_height - y_wanted && x < x_wanted))
                {
                    printf("██");
                }
                else
                {
                    printf("  ");
                }
                // move_to(term_width / 2, term_height / 2);
                // printf("%d|%d", x_wanted, y_wanted);
            }
        }
        x_wanted += 2;
        y_wanted += 1;
        fflush(stdout);
        msleep(5);
    }

    x_wanted = term_height * 2;
    y_wanted = term_height / 2;
    int x_wanted2 = x_wanted - 10;
    int y_wanted2 = y_wanted - 5;
    int x_wanted3 = x_wanted - 20;
    int y_wanted3 = y_wanted - 10;

    for (int n = 0; n < term_width / 3; n++)
    {
        for (int y = 0; y < term_height - 1; y++)
        {
            for (int x = 0; x < term_width - 1; x = x + 2)
            {
                move_to(x, y);
                if ((x == x_wanted && y < y_wanted && y > term_height - y_wanted) || (y == y_wanted && x < x_wanted && x > term_width - x_wanted) || (x == term_width - x_wanted && y > term_height - y_wanted && y < y_wanted) || (y == term_height - y_wanted && x > term_width - x_wanted && x < x_wanted) || (x == x_wanted2 && y < y_wanted2 && y > term_height - y_wanted2) || (y == y_wanted2 && x < x_wanted2 && x > term_width - x_wanted2) || (x == term_width - x_wanted2 && y > term_height - y_wanted2 && y < y_wanted2) || (y == term_height - y_wanted2 && x > term_width - x_wanted2 && x < x_wanted2) || (x == x_wanted3 && y < y_wanted3 && y > term_height - y_wanted3) || (y == y_wanted3 && x < x_wanted3 && x > term_width - x_wanted3) || (x == term_width - x_wanted3 && y > term_height - y_wanted3 && y < y_wanted3) || (y == term_height - y_wanted3 && x > term_width - x_wanted3 && x < x_wanted3))
                {
                    printf("██");
                }
                else
                {
                    printf("  ");
                }
                // move_to(term_width / 2, term_height / 2);
                // printf("%d|%d", x_wanted, y_wanted);
                // move_to(term_width / 2, term_height / 2 + 1);
                // printf("%d|%d", x_wanted2, y_wanted2);
                // move_to(term_width / 2, term_height / 2 + 2);
                // printf("%d|%d", x_wanted3, y_wanted3);
            }
        }
        x_wanted += 2;
        y_wanted += 1;
        x_wanted2 = x_wanted - 16;
        y_wanted2 = y_wanted - 8;
        x_wanted3 = x_wanted - 32;
        y_wanted3 = y_wanted - 16;
        fflush(stdout);
        msleep(5);
    }
    int ecart_term_y = 5;
    int ecart_AT = 7;
    char *spriteAT[2][6] = {
        {"     █████╗ ███╗   ██╗████████╗██╗ ██████╗ ██╗   ██╗███████╗      ",
         "    ██╔══██╗████╗  ██║╚══██╔══╝██║██╔═══██╗██║   ██║██╔════╝      ",
         "    ███████║██╔██╗ ██║   ██║   ██║██║   ██║██║   ██║█████╗        ",
         "    ██╔══██║██║╚██╗██║   ██║   ██║██║▄▄ ██║██║   ██║██╔══╝        ",
         "    ██║  ██║██║ ╚████║   ██║   ██║╚██████╔╝╚██████╔╝███████╗      ",
         "    ╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚═╝ ╚══▀▀═╝  ╚═════╝ ╚══════╝      "},
        {"████████╗███████╗██████╗ ███╗   ███╗██╗███╗   ██╗ █████╗ ██╗      ",
         "╚══██╔══╝██╔════╝██╔══██╗████╗ ████║██║████╗  ██║██╔══██╗██║      ",
         "   ██║   █████╗  ██████╔╝██╔████╔██║██║██╔██╗ ██║███████║██║      ",
         "   ██║   ██╔══╝  ██╔══██╗██║╚██╔╝██║██║██║╚██╗██║██╔══██║██║      ",
         "   ██║   ███████╗██║  ██║██║ ╚═╝ ██║██║██║ ╚████║██║  ██║███████╗ ",
         "   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝ "}};
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 6; i++)
        {
            move_to(term_width / 2 - 33 - 4 * (21 - n), ecart_term_y + ecart_AT + i);
            printf("    ");
            move_to(term_width / 2 - 33 + 4 * (20 - n), ecart_term_y + i);
            printf(spriteAT[0][i]);
            move_to(term_width / 2 - 33 - 4 * (20 - n), ecart_term_y + ecart_AT + i);
            printf(spriteAT[1][i]);
            fflush(stdout);
        }
        msleep(10);
    }
    int ecart_Abs = 5;
    char *spriteAbs[4] =
        {
            "     ▗▄▖ ▗▄▄▖  ▗▄▄▖ ▗▄▖ ▗▖   ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖    ",
            "    ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌  █  ▐▌       ",
            "    ▐▛▀▜▌▐▛▀▚▖ ▝▀▚▖▐▌ ▐▌▐▌   ▐▌ ▐▌  █  ▐▛▀▀▘    ",
            "    ▐▌ ▐▌▐▙▄▞▘▗▄▄▞▘▝▚▄▞▘▐▙▄▄▖▝▚▄▞▘  █  ▐▙▄▄▖    ",
        };
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            move_to(term_width / 2 - 26 + 4 * (20 - n), ecart_term_y + 2 * ecart_AT + i);
            printf(spriteAbs[i]);
            fflush(stdout);
        }
        msleep(10);
    }

    char *spriteRou[4] =
        {
            "    ▗▄▄▖  ▗▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖        ",
            "    ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌  █    █  ▐▛▚▖▐▌▐▌           ",
            "    ▐▛▀▚▖▐▌ ▐▌▐▌ ▐▌  █    █  ▐▌ ▝▜▌▐▌▝▜▌        ",
            "    ▐▌ ▐▌▝▚▄▞▘▝▚▄▞▘  █  ▗▄█▄▖▐▌  ▐▌▝▚▄▞▘        ",
        };
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            move_to(term_width / 2 - 26 - 4 * (20 - n), ecart_term_y + 2 * ecart_AT + ecart_Abs + i);
            printf(spriteRou[i]);
            fflush(stdout);
        }
        msleep(10);
    }

    char *spriteDef[4] =
        {
            "    ▗▄▄▄ ▗▄▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▄▄▄▖        ",
            "    ▐▌  █▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌▐▌   ▐▌           ",
            "    ▐▌  █▐▛▀▀▘▐▛▀▀▘▐▛▀▀▘▐▌ ▝▜▌ ▝▀▚▖▐▛▀▀▘        ",
            "    ▐▙▄▄▀▐▙▄▄▖▐▌   ▐▙▄▄▖▐▌  ▐▌▗▄▄▞▘▐▙▄▄▖        ",
        };
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            move_to(term_width / 2 - 26 + 4 * (20 - n), ecart_term_y + 2 * ecart_AT + 2 * ecart_Abs + i);
            printf(spriteDef[i]);
            fflush(stdout);
        }
        msleep(10);
    }

    char *spriteEpi[4] =
        {
            "    ▗▄▄▄▖▗▄▄▖▗▄▄▄▖ ▗▄▄▖                         ",
            "    ▐▌   ▐▌ ▐▌ █  ▐▌                            ",
            "    ▐▛▀▀▘▐▛▀▘  █  ▐▌                            ",
            "    ▐▙▄▄▖▐▌  ▗▄█▄▖▝▚▄▄▖                         ",
        };
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            move_to(term_width / 2 - 26 - 4 * (20 - n), ecart_term_y + 2 * ecart_AT + 3 * ecart_Abs + i);
            printf(spriteEpi[i]);
            fflush(stdout);
        }
        msleep(10);
    }

    char *spriteSim[4] =
        {
            "     ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▖ ▗▖▗▖    ▗▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖     ",
            "    ▐▌     █  ▐▛▚▞▜▌▐▌ ▐▌▐▌   ▐▌ ▐▌ █ ▐▌ ▐▌▐▌ ▐▌    ",
            "     ▝▀▚▖  █  ▐▌  ▐▌▐▌ ▐▌▐▌   ▐▛▀▜▌ █ ▐▌ ▐▌▐▛▀▚▖    ",
            "    ▗▄▄▞▘▗▄█▄▖▐▌  ▐▌▝▚▄▞▘▐▙▄▄▖▐▌ ▐▌ █ ▝▚▄▞▘▐▌ ▐▌    ",
        };
    for (int n = 0; n < 21; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            move_to(term_width / 2 - 26 + 4 * (20 - n), ecart_term_y + 2 * ecart_AT + 4 * ecart_Abs + i);
            printf(spriteSim[i]);
            fflush(stdout);
        }
        msleep(10);
    }
    msleep(500);
    clear_screen();
    char *spriteATARDES[2][9] = {
        {
            COLOR_GREEN "   ▄████████     ███        ▄████████    ▄████████ ████████▄     ▄████████    ▄████████ ",
            COLOR_GREEN "   ███    ███ ▀█████████▄   ███    ███   ███    ███ ███   ▀███   ███    ███   ███    ███",
            COLOR_GREEN "   ███    ███    ▀███▀▀██   ███    ███   ███    ███ ███    ███   ███    █▀    ███    █▀ ",
            COLOR_GREEN "   ███    ███     ███   ▀   ███    ███  ▄███▄▄▄▄██▀ ███    ███  ▄███▄▄▄       ███       ",
            COLOR_GREEN " ▀███████████     ███     ▀███████████ ▀▀███▀▀▀▀▀   ███    ███ ▀▀███▀▀▀     ▀███████████",
            COLOR_GREEN "   ███    ███     ███       ███    ███ ▀███████████ ███    ███   ███    █▄           ███",
            COLOR_GREEN "   ███    ███     ███       ███    ███   ███    ███ ███   ▄███   ███    ███    ▄█    ███",
            COLOR_GREEN "   ███    █▀     ▄████▀     ███    █▀    ███    ███ ████████▀    ██████████  ▄████████▀ ",
            COLOR_GREEN "                                         ███    ███                                     ",
        },
        {
            COLOR_GREEN "   ▄████████     ███        ▄████████    ▄████████ ████████▄     ▄████████    ▄████████ ",
            "   ███    ███ ▀█████████▄   ███    ███   ███    ███ ███   ▀███   ███    ███   ███    ███",
            "   ███    ███    ▀███▀▀██   ███    ███   ███    ███ ███    ███   ███    █▀    ███    █▀ ",
            "   ███    ███     ███   ▀   ███    ███  ▄███▄▄▄▄██▀ ███    ███  ▄███▄▄▄       ███       ",
            " ▀███████████     ███     ▀███████████ ▀▀███▀▀▀▀▀   ███    ███ ▀▀███▀▀▀     ▀███████████",
            "   ███    ███     ███       ███    ███ ▀███████████ ███    ███   ███    █▄           ███",
            "   ███    ███     ███       ███    ███   ███    ███ ███   ▄███   ███    ███    ▄█    ███",
            "   ███    █▀     ▄████▀     ███    █▀    ███    ███ ████████▀    ██████████  ▄████████▀ ",
            "                                         ███    ███                                     ",
        }};

    char *spriteTuto[2][9] = {
        {COLOR_GRAY "▗▄▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▖ ▗▄▄▖ ▗▄▄▄▖▗▄▄▄▖▗▖        ",
         COLOR_GRAY "  █  ▐▌ ▐▌  █ ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌        ",
         COLOR_GRAY "  █  ▐▌ ▐▌  █ ▐▌ ▐▌▐▛▀▚▖  █  ▐▛▀▀▘▐▌        ",
         COLOR_GRAY "  █  ▝▚▄▞▘  █ ▝▚▄▞▘▐▌ ▐▌▗▄█▄▖▐▙▄▄▖▐▙▄▄▖     ",
         "                                            ",
         COLOR_GREEN "█▄      ▗▖▗▄▄▄▖▗▖ ▗▖                        ",
         COLOR_GREEN "███▄    ▐▌▐▌   ▐▌ ▐▌                        ",
         COLOR_GREEN "███▀    ▐▌▐▛▀▀▘▐▌ ▐▌                        ",
         COLOR_GREEN "█▀   ▗▄▄▞▘▐▙▄▄▖▝▚▄▞▘                        "},
        {COLOR_GREEN "█▄   ▗▄▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▖ ▗▄▄▖ ▗▄▄▄▖▗▄▄▄▖▗▖   ",
         COLOR_GREEN "███▄   █  ▐▌ ▐▌  █ ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌   ",
         COLOR_GREEN "███▀   █  ▐▌ ▐▌  █ ▐▌ ▐▌▐▛▀▚▖  █  ▐▛▀▀▘▐▌   ",
         COLOR_GREEN "█▀     █  ▝▚▄▞▘  █ ▝▚▄▞▘▐▌ ▐▌▗▄█▄▖▐▙▄▄▖▐▙▄▄▖",
         "                                            ",
         COLOR_GRAY "   ▗▖▗▄▄▄▖▗▖ ▗▖                             ",
         COLOR_GRAY "   ▐▌▐▌   ▐▌ ▐▌                             ",
         COLOR_GRAY "   ▐▌▐▛▀▀▘▐▌ ▐▌                             ",
         COLOR_GRAY "▗▄▄▞▘▐▙▄▄▖▝▚▄▞▘                             "}};

    int tutorial_variable = 1;
    int tutorial_page = 0;
    while (true)
    {
        int c = get_key_press();
        if (c == 'A')
        {
            tutorial_variable = 1;
        }
        else if (c == 'B')
        {
            tutorial_variable = 0;
        }

        for (int i = 0; i < 9; i++)
        {
            move_to(term_width / 2 - 44, term_height / 2 - 21 + i);
            printf(spriteATARDES[0][i]);
            move_to(term_width / 2 - 44, term_height / 2 - 5 + i);
            printf(spriteTuto[tutorial_variable][i]);
            fflush(stdout);
        }
        if (c == 10 && tutorial_variable == 0)
        {
            break;
        }
        if (c == 10 && tutorial_variable == 1)
        {
            clear_screen();
            while (tutorial_page < 11)
            {
                int c = get_key_press();
                for (int y = 0; y < term_height - 5; y++)
                {
                    for (int x = 0; x < term_width - 20; x++)
                    {
                        printf(RESET);
                        move_to(20 + x, 5 + y);
                        if ((x == 0 || x == term_width - 40) && y <= term_height - 10)
                        {
                            printf("█");
                        }
                        else if (y == 0 && x <= term_width - 40)
                        {
                            printf("▀");
                        }
                        else if (y == term_height - 10 && x <= term_width - 40)
                        {
                            printf("▄");
                        }
                    }
                }
                move_to(term_width-26,term_height-7);
                printf("%d/10",tutorial_page);
                if (c == 10)
                {
                    tutorial_page++;
                }
            }
            break;
        }
    }

    if (bro_this_variable_is_actually_an_easter_egg == 1)
    {
        term_width += 1;
    }

    msleep(200);
}