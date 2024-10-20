#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


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
