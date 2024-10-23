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
#include <signal.h>
#include <termios.h>
#include <malloc.h>

/*
#############
CONFIGURATION
#############
*/
#define HISTORY_SIZE 500

#define MIN_TERMINAL_WIDTH 120
#define MIN_TERMINAL_HEIGHT 20

#define TARGET_FPS 40

// TODO: add max size

#define CELL_WIDTH 10
#define CELL_HEIGHT 5

#define GAP 2

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) MIN(MAX(x, a), b)

/*
##################
## Types custom ##
##################
*/

enum TurretType
{
    Sniper
};

struct Turret
{
    enum TurretType type;
    int lvl;
    float compteur;
    int range;
    int damage;
    float reload_delay;
};

typedef struct EnemyPool
{
    int length; // Taille du tableau
    int count;  // Nombre d’ennemis dans le tableau
    struct Enemy *enemies;
} EnemyPool;

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
    bool selected;
    struct Turret turret;
    bool hasTurret;
    bool drawn;
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

/*
#############
##   IO    ##
#############
*/

#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define UNDERLINE_RST "\033[24m"
#define BOLD "\033[1m"
#define BOLD_RST "\033[22m"

// General colors
#define COLOR_RED "\033[91m"
#define COLOR_GRAY "\033[38;5;243m"
#define COLOR_GREEN "\033[38;5;42m"
#define COLOR_YELLOW "\033[38;5;11;1m"

// Terrain colors
#define COLOR_STANDARD_BG "\033[48;5;233m"
#define COLOR_TOWER_SLOT_BG "\033[48;5;236m"
#define COLOR_PATH_BORDER "\033[38;5;240m"
#define COLOR_SELECTED_SLOT "\033[38;5;221m"
// Health bar
#define COLOR_HEALTH_BG "\033[48;5;236m"
#define COLOR_HEALTH_0 "\033[38;5;196m"
#define COLOR_HEALTH_25 "\033[38;5;166m"
#define COLOR_HEALTH_50 "\033[38;5;178m"
#define COLOR_HEALTH_75 "\033[38;5;82m"

char get_key_press();
void move_to(int x, int y);
void clear_screen();
void clear_line();
void hide_cursor();
void show_cursor();
void printCritical(char *errorMessage);
void get_terminal_size(int *width, int *height);
void checkTerminalSize(int *width, int *height);
