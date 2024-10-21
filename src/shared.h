#pragma once

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>    

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


typedef struct Grid
{
    int width;
    int height;
    struct Cell **cells;
}  Grid;



/* msleep(): Sleep for the requested number of milliseconds.
    Origin: https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
 */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}