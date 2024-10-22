#include "shared.h"

int msleep(long ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    return nanosleep(&ts, &ts);
}

/*
#############
##    IO   ##
#############
*/

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
        printf("Colones: %d/%d", *width, MIN_TERMINAL_WIDTH);
        move_to(*width / 2 - 7, *height / 2 + 1);
        printf("Rangées: %d/%d", *height, MIN_TERMINAL_HEIGHT);

        get_terminal_size(width, height);

        fflush(stdout);

        msleep(200);
    }
}
