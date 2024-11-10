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

#include "ncursesWrapper.h"

/*
#############
CONFIGURATION
#############
*/
#define HISTORY_SIZE 500

#define MIN_TERMINAL_WIDTH 232
#define MIN_TERMINAL_HEIGHT 74

#define MAX_TERMINAL_WIDTH 510
#define MAX_TERMINAL_HEIGHT 150

#define TARGET_FPS 1000
#define WAVE_DELAY 0.5 // (secondes)

#define CELL_WIDTH 18
#define CELL_HEIGHT 9

#define GAP 2

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) MIN(MAX(x, a), b)
#define ABS(a) ((a < 0) ? (-a) : (a))

#define PI 3.14159265359

/*
##################
## Types custom ##
##################
*/

enum TurretType
{
    Sniper,
    Inferno,
    Mortier,
    Freezer,
    Petrificateur,
    Banque,
    Gatling,
};

enum EffectType
{
    Money,
    Stun,
    Slow,
    Fire,
    BOSS_STUN,
};

struct Turret
{
    enum TurretType type;      // Type de tourelle
    int lvl;                   // Niveau de la tourelle et également l'indice des différente valeurs qui changent lors de l'upgrade
    float compteur;            // Compteur pour savoir quand la tourelle doit tirer
    float compteur_mortier;    // Compteur pour clear les tirs de mortier
    bool in_range;             // sert a savoir si un ennemi est a portée de la tourrelle
    float range_min[3];        // détermine la portée minimale de la tourelle
    float range_max[3];        // détermine la portée maximale de la tourelle
    float damage[3];           // détermine les dégats de la tourelle
    float reload_delay[3];     // détermine la fréquence de tir de la tourelle
    float splash[3];           // détermine la taille des dégats de zone de la tourelle
    int nb_ennemi[3];          // détermine le nombre d'ennemi que la tourelle peut toucher lors de son attaque
    bool has_effect;           // determine si la tourelle possède un effet particulier
    enum EffectType effet;     // Type de l'effet
    float puissance_effet[3];  // determine la puissance de l'effet (a un role different selon les effets)
    bool has_malus;           // determine si la tourelle subbit un effet particulier
    enum EffectType malus; // Type de l'effet subbit
    float puissance_malus; // Puissance de l'effet subbit
    float last_shot_dx;
    float last_shot_dy;
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
    bool has_effect;
    enum EffectType effect;
    float effect_counter;
};

typedef struct Grid
{
    int width; // Largeur en cases
    int height; // hauteur en cases
    int start_x; // position de la première case du chemin
    int start_y;
    int end_x; // position de la dernière case du chemin
    int end_y;
    struct Cell **cells;
} Grid;

typedef struct GameStats
{
    int cash;
    int health;
} GameStats;

struct Label
{
    int x;
    int y;
    int text;
    float duration;
    float counter;
};

typedef struct Labels
{
    int size;
    int count;
    struct Label *labels;
} Labels;
void updateLabels(Labels *labels, float dt);
void drawLabels(Labels labels);
void freeLabels(Labels labels);

/*
#############
##  UTILS  ##
#############
*/

int msleep(long ms);

char get_key_press();
void move_to(int x, int y);
void clear_screen();
void clear_line();
void hide_cursor();
void show_cursor();
void printCritical(char *errorMessage);
void get_terminal_size(int *width, int *height);
void checkTerminalSize(int *width, int *height);
void drawRange(int term_width, int term_height, float range, float grid_x, float grid_y, bool fill);
void anim_debut(int term_width, int term_height);