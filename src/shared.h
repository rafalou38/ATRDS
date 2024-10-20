#pragma once

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/*
#############
CONFIGURATION
#############
*/
#define HISTORY_SIZE 500

#define MIN_TERMINAL_WIDTH 120
#define MIN_TERMINAL_HEIGHT 20

// TODO: add max size

#define CELL_WIDTH 10
#define CELL_HEIGHT 5

#define GAP 2





#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
