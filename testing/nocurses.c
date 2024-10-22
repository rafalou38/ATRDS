#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

void moveCursor(int x, int y)
{
    // ANSI escape code to move cursor to (x, y)
    printf("\033[%d;%dH", y + 1, x + 1);
}

int main(void)
{
    // Initialise random number generator
    srand(time(NULL));

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int COLS = w.ws_col;
    int LINES = w.ws_row;

    // Hide cursor to avoid distraction
    printf("\033[?25l");

    // Infinite loop to continuously update the screen
    while (1)
    {
        system("clear");
        // Loop through each position on the terminal
        for (int y = 0; y < LINES; y++)
        {
            for (int x = 0; x < COLS; x++)
            {
                // Move the cursor to the correct position
                moveCursor(x, y);

                // Randomly decide whether to print 'o' or '.'
                if (rand() % 3 == 0)
                {
                    printf("o");
                }
                else
                {
                    printf(".");
                }
            }
        }

        // Flush stdout to ensure the updates are shown immediately
        fflush(stdout);

        // Sleep for a short duration to simulate screen refresh
        usleep(10000); // 100 milliseconds
    }

    // Show cursor before exiting (though this code will run indefinitely)
    printf("\033[?25h");

    return 0;
}
