#include "shared.h"

int msleep(long ms) // Fonction de repos (temps pendant lequel le terminal ne lis pas la suite du programme), ici en millisecondes
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    return nanosleep(&ts, &ts);
}

void updateLabels(Labels *labels, float dt) // Mets a jour les labels (l'argent drop par les monstres)
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
        printf(COLOR_STANDARD_BG COLOR_YELLOW "%d" RESET, labels.labels[i].text);
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

// Animation de début de jeu / tutoriel (il ne s'agit que de boucle qui font se déplacer le curseur afin que l'annimation se produise)

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
        msleep(2);
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
        msleep(2);
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
        msleep(2);
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
        msleep(2);
    }
    int ecart_term_y = 20;
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
        msleep(15);
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
        msleep(15);
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
        msleep(15);
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
        msleep(15);
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
        msleep(15);
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
        msleep(15);
    }
    msleep(1000);
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
        if (c == 'q')
        {
            exit(0);
        }
        if (c == 10 && tutorial_variable == 0)
        {
            break;
        }
        if (c == 10 && tutorial_variable == 1)
        {
            clear_screen();
            int tutorial_page = 1;
            while (tutorial_page < 4)
            {
                int c = get_key_press();
                int ecartx = 40;
                int ecarty = 10;
                for (int y = 0; y < term_height - ecarty; y++)
                {
                    for (int x = 0; x < term_width - ecartx; x++)
                    {
                        printf(RESET);
                        move_to(ecartx + x, ecarty + y);
                        if ((x == 0 || x == term_width - 2 * ecartx) && y <= term_height - 2 * ecarty)
                        {
                            printf("█");
                        }
                        else if (y == 0 && x <= term_width - 2 * ecartx)
                        {
                            printf("▀");
                        }
                        else if (y == term_height - 2 * ecarty && x <= term_width - 2 * ecartx)
                        {
                            printf("▄");
                        }
                    }
                }
                if (tutorial_page == 1)
                {
                    char *spriteWelcome[4] =
                        {
                            "▗▄▄▖ ▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▖ ▗▖▗▄▄▄▖    ▗▄▄▄  ▗▄▖ ▗▖  ▗▖ ▗▄▄▖     ▗▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖ ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖",
                            "▐▌ ▐▌  █  ▐▌   ▐▛▚▖▐▌▐▌  ▐▌▐▌   ▐▛▚▖▐▌▐▌ ▐▌▐▌       ▐▌  █▐▌ ▐▌▐▛▚▖▐▌▐▌       ▐▌ ▐▌ █ ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌   ▐▌   ",
                            "▐▛▀▚▖  █  ▐▛▀▀▘▐▌ ▝▜▌▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌▐▌ ▐▌▐▛▀▀▘    ▐▌  █▐▛▀▜▌▐▌ ▝▜▌ ▝▀▚▖    ▐▛▀▜▌ █ ▐▛▀▜▌▐▛▀▚▖▐▌  █▐▛▀▀▘ ▝▀▚▖",
                            "▐▙▄▞▘▗▄█▄▖▐▙▄▄▖▐▌  ▐▌ ▝▚▞▘ ▐▙▄▄▖▐▌  ▐▌▝▚▄▞▘▐▙▄▄▖    ▐▙▄▄▀▐▌ ▐▌▐▌  ▐▌▗▄▄▞▘    ▐▌ ▐▌ █ ▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀▐▙▄▄▖▗▄▄▞▘",
                        };

                    for (int i = 0; i < 4; i++)
                    {
                        move_to(term_width / 2 - 55, ecarty + 2 + i);
                        printf(spriteWelcome[i]);
                    }
                    int indentation = term_width / 2 - 35;
                    int ecart_en_plus_pour_aligner_txt = 9;
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("ANTIQUE TERMINAL : Absolute Routing Defense Epic Simulator");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("est un jeu de type \"Tower Defense\" dans lequel vous aller");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("pouvoir poser des tourelles de défense afin de se protéger");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("de la horde grandissante d'ennemis qui veulent sceller votre");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Terminal Antique. Ainsi, votre objectif est de défendre le");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Saint Terminal des malicieux chenapans qui veulent récupérer");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("son pouvoir absolu ! Et pour ce faire, vous aller devoir");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("utiliser les flèches directionnelles de la Souveraineté");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("afin de déplacer votre seléction. Lorsque vous estimez la");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("case seléctionnée digne d'obtenir une merveilleuse tourelle");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("de défense, vous pouvez utiliser le pouvoir terminalogique");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("en appuyant sur \"espace\" pour arrêter et reprendre le temps !");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Ce pouvoir dénommé ZA WARUDO possède d'autres outils extrêmement");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("intéressants, tel que la capacité d'échanger de l'EURO (€),");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("l'Esprit Ultra Réaliste Obtenu des ennemis contre des tourelles !");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Cependant, pour invoquer l'intervention divine et par correspondance");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("l'apparition d'une tourelle de défense, il est nécessaire");
                    move_to(indentation, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 3;
                    printf("d'exécuter une action complexe, cachée depuis des générations...");
                    char *spriteEntrer[4] =
                        {
                            " ▗▄▖ ▗▄▄▖ ▗▄▄▖ ▗▖ ▗▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▄▖     ▗▄▄▖▗▖ ▗▖▗▄▄▖     ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖▗▄▄▖ ",
                            "▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌ ▝▚▞▘ ▐▌      ▗▞▘    ▐▌   ▐▌ ▐▌▐▌ ▐▌    ▐▌   ▐▛▚▖▐▌  █  ▐▌ ▐▌▐▌   ▐▌ ▐▌",
                            "▐▛▀▜▌▐▛▀▘ ▐▛▀▘ ▐▌ ▐▌  ▐▌  ▐▛▀▀▘ ▗▞▘       ▝▀▚▖▐▌ ▐▌▐▛▀▚▖    ▐▛▀▀▘▐▌ ▝▜▌  █  ▐▛▀▚▖▐▛▀▀▘▐▛▀▚▖",
                            "▐▌ ▐▌▐▌   ▐▌   ▝▚▄▞▘  ▐▌  ▐▙▄▄▖▐▙▄▄▄▖    ▗▄▄▞▘▝▚▄▞▘▐▌ ▐▌    ▐▙▄▄▖▐▌  ▐▌  █  ▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌",
                        };

                    for (int i = 0; i < 4; i++)
                    {
                        move_to(term_width / 2 - 45, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteEntrer[i]);
                    }
                    fflush(stdout);
                }
                if (tutorial_page == 2)
                {
                    char *spriteElu[4] =
                        {
                            "▗▖  ▗▖ ▗▄▖ ▗▖ ▗▖ ▗▄▄▖    ▗▄▄▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▄▖    ▗▖  ▐▌ ▗▄▄▄▖▗▖   ▗▖ ▗▖",
                            "▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌▐▌       ▐▌     █  ▐▌   ▐▌       ▐▌     ▐▌   ▐▌   ▐▌ ▐▌",
                            "▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌ ▝▀▚▖    ▐▛▀▀▘  █  ▐▛▀▀▘ ▝▀▚▖    ▐▌     ▐▛▀▀▘▐▌   ▐▌ ▐▌",
                            " ▝▚▞▘ ▝▚▄▞▘▝▚▄▞▘▗▄▄▞▘    ▐▙▄▄▖  █  ▐▙▄▄▖▗▄▄▞▘    ▐▙▄▄▖  ▐▙▄▄▖▐▙▄▄▖▝▚▄▞▘",
                        };

                    for (int i = 0; i < 4; i++)
                    {
                        move_to(term_width / 2 - 35, ecarty + 2 + i);
                        printf(spriteElu[i]);
                    }
                    int indentation2 = term_width / 2 - 40;
                    int ecart_en_plus_pour_aligner_txt = 9;
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("IMPOSSIBLE !!! Vous avez trouvé le code secret gardé depuis des anciens temps...");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Vous êtes donc vraiment... le SEUL, l'UNIQUE ! Vous êtes l'ELU !!!");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Très bien, si c'est le cas, j'ai le devoir de vous guider : Je vais vous expliquer");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("les utilités des différentes tourelles.");
                    char *spriteSniper[7] = {COLOR_SNIPER_BASE "     ▄██▄     ",
                                             "   ▄██████▄   ",
                                             " ▄████▀▀████▄ ",
                                             COLOR_SNIPER_HEAD "<" COLOR_SNIPER_CANNON "===" COLOR_SNIPER_BASE "█ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                                             " ▀████▄▄████▀ ",
                                             "   ▀██████▀   ",
                                             "     ▀██▀     "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteSniper[i]);
                    }
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Le Sniper" RESET " : La tourelle la plus basique, néanmoins si puissante, elle est dotée d'une");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("grande portée et fait de gros dégats sur un ennemi malgré son temps de recharge assez long.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Ses tirs sont d'une précision redoutable, et il sera sûrement un de vos meilleurs alliés.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Vous pouvez même améliorer ces tourelles en les re-sélectionnant et celle-ci peut être améliorée 2 fois !");


                    char *spriteInferno[7] = {COLOR_INFERNO_BASE "     ▄██▄     ",
                                              "  ▄▀█▀▀▀▀█▀▄  ",
                                              " ▄█▀ " COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_FIRE2 "▀ ▄" COLOR_INFERNO_BASE " ▀█▄ ",
                                              "███  " COLOR_INFERNO_FIRE1 "▀▄█" COLOR_INFERNO_FIRE2 " ▄" COLOR_INFERNO_BASE " ███",
                                              " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                                              "  ▀▄█▄▄▄▄█▄▀  ",
                                              "     ▀██▀     "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteInferno[i]);
                    }
                    
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("L'Inferno" RESET " : Une tourelle venue tout droit du 4ème cercle des enfers dans le but de vous aider");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("dans votre tâche, elle est capable d'immoler tout et n'importe quoi à l'aide de ses 4 canons lance-flamme.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 4;
                    printf("Ses attaques peuvent toucher jusqu'à 4 ennemis à la fois dans une portée d'une case autour de l'inferno.");

                    char *spriteMortier[7] = {COLOR_MORTIER_BASE "  ██████████  ",
                                              "██ ▄█▀▀▀▀█▄ ██",
                                              "███▀ " COLOR_MORTIER_FIRING "▄██▄" COLOR_MORTIER_BASE " ▀███",
                                              "███ " COLOR_MORTIER_FIRING "██" COLOR_MORTIER_FIRING_CENTER "██" COLOR_MORTIER_FIRING "██" COLOR_MORTIER_BASE " ███",
                                              "███▄ " COLOR_MORTIER_FIRING "▀██▀" COLOR_MORTIER_BASE " ▄███",
                                              "██ ▀█▄▄▄▄█▀ ██",
                                              "  ██████████  "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteMortier[i]);
                    }
                    
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Le Mortier" RESET " : Une tourelle d'artillerie lourde utilisée pour envoyer d'énormes Terminal Shells");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("sur vos oppsants, explosant et attaquant ces pauvres gueux dans une zone considérable.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Cette tourelle est spécialisée pour le bombardement à distance, ainsi inefficace sur des cibles trop proches.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Un grand homme à un jour dit : Si vous n'avez pas de trébuchet, visez pour le mortier ! --Nodnach Imér");

                    char *spriteGatling[7] = {COLOR_GATLING_BASE "  ████▀▀████  ",
                                              "██  ██▄▄██  ██",
                                              "█████▀  ▀█████",
                                              "█  █  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █  █",
                                              "█████▄  ▄█████",
                                              "██  ██▀▀██  ██",
                                              "  ████▄▄████  "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteGatling[i]);
                    }
                    
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("La Gatling" RESET " : Une arme dignement importée d'un état inter-dimensionnel nommé \"USA\"");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("et montée sur un socle de tourelle pour tirer en continu de manière très rapide sur des ennemis proches.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Cette tourelle est extrêmement efficace contre les cibles singulières, surtout si elle est bien placée !");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Un moins grand homme à un jour dit : RATATATATATATA ! --Zenitram Leafar");

                    fflush(stdout);
                }

                if (tutorial_page == 3)
                {
                    char *spriteTourelles[4] =
                        {
                            " ▗▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖ ▗▄▄▖    ▗▄▄▄▖▗▄▖ ▗▖ ▗▖▗▄▄▖ ▗▄▄▄▖▗▖   ▗▖   ▗▄▄▄▖ ▗▄▄▖",
                            "▐▌ ▐▌▐▌ ▐▌  █  ▐▌ ▐▌▐▌   ▐▌         █ ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▌   ▐▌   ",
                            "▐▛▀▜▌▐▌ ▐▌  █  ▐▛▀▚▖▐▛▀▀▘ ▝▀▚▖      █ ▐▌ ▐▌▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘▐▌   ▐▌   ▐▛▀▀▘ ▝▀▚▖",
                            "▐▌ ▐▌▝▚▄▞▘  █  ▐▌ ▐▌▐▙▄▄▖▗▄▄▞▘      █ ▝▚▄▞▘▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▄▖▐▙▄▄▖▗▄▄▞▘",
                        };

                    for (int i = 0; i < 4; i++)
                    {
                        move_to(term_width / 2 - 39, ecarty + 2 + i);
                        printf(spriteTourelles[i]);
                    }
                    int indentation2 = term_width / 2 - 40;
                    int ecart_en_plus_pour_aligner_txt = 9;

                    char *spriteFreezer[7] = {COLOR_FREEZER_BASE"      ██      ",
                                              " ▄  ██▀▀██  ▄ ",
                                              "  ██▀  " COLOR_FREEZER_FIRING_CENTER "▄" COLOR_FREEZER_BASE " ▀██  ",
                                              "███  " COLOR_FREEZER_FIRING_CENTER "▀  ▄" COLOR_FREEZER_BASE "  ███",
                                              "  ██▄ " COLOR_FREEZER_FIRING_CENTER "▀" COLOR_FREEZER_BASE "  ▄██  ",
                                              " ▀  ██▄▄██  ▀ ",
                                              "      ██      "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteFreezer[i]);
                    }
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Le Freezer" RESET " : Une tourelle ayant pour seul objectif de tout geler et SUR-geler, elle");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("vous ferait perdre votre sang FROID, vous deviendriez GIVRÉS ! Zut, je voulais briser la GLACE,");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("mais je crois que j'ai jeté un FROID. Enfin bon, elle ralentit vos ennemis, vous aidant");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("à gagner votre guerre FROIDE d'une manière FRISSONANTE ! HAH, Vous GLISSEZ avec moi ? (Aled)");

                    char *spritePetrificateur[7] = {COLOR_PETRIFICATEUR_BASE"   ▄▄    ▄▄   ",
                                                    " ▄▀  █▄▄█  ▀▄ ",
                                                    " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                                                    "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █   ",
                                                    " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                                                    " ▀▄  █▀▀█  ▄▀ ",
                                                    "   ▀▀    ▀▀   "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spritePetrificateur[i]);
                    }
                    
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Le Pétrificateur" RESET " : Une tourelle mystique qui, d'après la légende, aurait rendu fou son créateur");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("de par sa complexité et sa beauté. De toute manière, c'est une tourelle très puissante pouvant figer ");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("vos adversaires sur place grâce à des forces mystérieuses. Il peut arrêter vos opposants dans un rayon ");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("plutôt rapproché. Une voix mystérieuse vous susurre :\"20 Pu-Diantre de Frame d'animations, Corneguidouille\"");

                    char *spriteBanque[7] = {COLOR_BANQUE_BASE"    ▄▄▄▄▄▄    ",
                      " ▄███▄▄▄▄███▄ ",
                      " ███" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ███" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ██▀▀▀▀▀██▀▀█ ",
                      " ████████████ ",
                      "              "};
                    for (int i = 0; i < 7; i++)
                    {
                        move_to(term_width / 2 - 56, ecarty + ecart_en_plus_pour_aligner_txt + i);
                        printf(spriteBanque[i]);
                    }
                    
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("La Banque" RESET " : L'arrivé du capitalisme dans le monde Terminalogique, qui distribue l'EURO à la populasse.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("L'EURO étant un Esprit Ultra Réaliste Obtenu des ennemis, on pourrais se demander comment cette dite banque");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("en génère à une vitesse affolante, mais je préfère ne pas y penser. De toute façon, elle nous génère moultes");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 4;
                    printf("ressources nous permettant de poser encore plus de tourelles, et ça, on apprécie.");

                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Ceci étant dit, vous êtes fin prêt à démarrer votre ultime altercation avec les forces du mal !");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("Ainsi vous n'avez qu'à appuyer sur la touche suprême une dernière fois, et tout commenceras.");
                    move_to(indentation2, ecarty + ecart_en_plus_pour_aligner_txt);
                    ecart_en_plus_pour_aligner_txt += 2;
                    printf("N'oubliez pas, le power du ZA WARUDO (touche \"espace\") vous permet d'arrêter le temps et de réfléchir.");

                    fflush(stdout);
                }

                move_to(term_width - 6 - ecartx, term_height - 2 - ecarty);
                printf("%d/3", tutorial_page);
                
                if (c == 10)
                {
                    tutorial_page++;
                    clear_screen();
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