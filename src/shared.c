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
    // printf("\033[%d;%dH", y, x);
    move(y, x);
}
void clear_screen()
{
    erase();
    // printf("\033[2J");
}
void hide_cursor()
{
    curs_set(false);
    // printf("\033[?25l");
}
void show_cursor()
{
    curs_set(true);
    // printf("\033[?25h");
}

void printCritical(char *errorMessage)
{
    fprintf(stderr, "\033[91;4mCRITICAL\033[24m: %s \033[0m\n", errorMessage);
}

void get_terminal_size(int *width, int *height)
{
    // struct winsize w;
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // *width = w.ws_col;
    // *height = w.ws_row;

    *width = COLS;
    *height = LINES;
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

        // fflush(stdout);

        msleep(200);
    }
}
