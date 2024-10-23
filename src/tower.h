#include "shared.h"

void showTowerSelection()
{
    int terminal_width, terminal_height;
    get_terminal_size(&terminal_width, &terminal_height);

    int width = 80;//terminal_width/2;
    int height = 30;//terminal_height/2;
    assert(width < terminal_width);
    assert(height < terminal_height);

    int x0 = terminal_width/2.0f - width/2.0f;
    int y0 = terminal_height/2.0f - height/2.0f + 1;

    printf(RESET);
    printf(COLOR_STANDARD_BG);

    for (int y = 0; y < height; y++)
    {
        move_to(x0, y0 + y);

        for (int x = 0; x < width; x++)
        {
            if (x == 0 || x == width - 1)
                printf("█");
            else if (y == 0)
                printf("▀");
            else if (y == height - 1)
                printf("▄");
            else
                printf(" ");
        }
    }

    printf(RESET);
}