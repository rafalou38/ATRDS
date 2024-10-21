#pragma once

#define _POSIX_C_SOURCE 199309L
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <math.h>

/*
#############
CONFIGURATION
#############
*/
#define HISTORY_SIZE 500

#define MIN_TERMINAL_WIDTH 120
#define MIN_TERMINAL_HEIGHT 20

// TODO: add max size

#define CELL_WIDTH 12
#define CELL_HEIGHT 6

#define GAP 2

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a,b) MIN(MAX(x,a),b)


/*
##################
## Types custom ##
##################
*/

enum CellType
{
    CHEMIN,
    TERRAIN
};

struct Cell
{
    int x;
    int y;
    int index;
    enum CellType type;
    bool visited;
};

typedef struct Grid
{
    int width;
    int height;
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    struct Cell **cells;
} Grid;

/*
#############
##  UTILS  ##
#############
*/

int msleep(long ms);
void move_to(int x, int y);
void clear_screen();
void hide_cursor();
void show_cursor();
void printCritical(char *errorMessage);
void get_terminal_size(int *width, int *height);
void checkTerminalSize(int *width, int *height);
