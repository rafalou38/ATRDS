#include "ncursesWrapper.h"

// void init_colors()
// {
//     if (has_colors() && COLORS >= 256)
//     {
//         start_color();
//         use_default_colors();
//         for (int i = 0; i < 256; i++)
//         {
//             init_extended_pair(i + 1, i, 233); // -1 for default background
//         }
//     }
// }

bool pairs[256] = {0};

void apply_ansi_code(const char *code)
{
    int color_value_fg;
    int color_value_bg;
    int id;
    if (sscanf(code, "\033[38;5;%d", &color_value_fg) == 1)
    { // Foreground color
        if (color_value_fg >= 0 && color_value_fg < 256)
        {
            if (!pairs[color_value_fg])
            {
                pairs[color_value_fg] = true;
                init_extended_pair(color_value_fg + 1, color_value_fg, 233);
            }
            attron(COLOR_PAIR(color_value_fg + 1));
        }
    }
    else if (sscanf(code, "\033[42;%d;%d;%d", &id, &color_value_fg, &color_value_bg) == 3)
    { // Background color
        if (!pairs[id])
        {
            pairs[id] = true;
            init_extended_pair(id, color_value_fg, color_value_bg);
        }
        attron(COLOR_PAIR(id));
    }
    else if (strcmp(code, "\033[0m") == 0)
    {
        attrset(A_NORMAL);
    }
    else if (strcmp(code, "\033[1m") == 0)
    {
        attron(A_BOLD);
    }
    else if (strcmp(code, "\033[4m") == 0)
    {
        attron(A_UNDERLINE);
    }
    // Add further cases as needed
}

// Wrapper function for ncurses printing with ANSI escape interpretation

void custom_printf(const char *format, ...)
{
    char buffer[BUFFER_SIZE];
    va_list args;

    // Format the string using vsnprintf
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    char *p = buffer;
    while (*p)
    {
        if (*p == '\033')
        { // Start of ANSI code
            const char *code_start = p;
            while (*p && *p != 'm')
                p++;
            if (*p == 'm')
                p++;
            char ansi_code[20];
            strncpy(ansi_code, code_start, p - code_start);
            ansi_code[p - code_start] = '\0';

            apply_ansi_code(ansi_code);
        }
        else
        {
            printw("%c", *p);
            p++;
        }
    }

    va_end(args);
}