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

#define MIN_TERMINAL_WIDTH 232
#define MIN_TERMINAL_HEIGHT 74

#define MAX_TERMINAL_WIDTH 350
#define MAX_TERMINAL_HEIGHT 100

#define TARGET_FPS 144
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

/*
#############
##   IO    ##
#############
*/

// Colors and styles
#define ERASE_LINE "\33[2K"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define UNDERLINE_RST "\033[24m"
#define BOLD "\033[1m"
#define BOLD_RST "\033[22m"
#define BG_RESET "\033[48;5;0m"
// General colors
#define COLOR_RED "\033[91m"
#define COLOR_GRAY "\033[38;5;243m"
#define COLOR_GREEN "\033[38;5;42m"
#define COLOR_YELLOW "\033[38;5;11;1m"
// Turrets colors
// Sniper colors
#define COLOR_SNIPER_BASE "\033[38;5;48m"
#define COLOR_SNIPER_CANNON "\033[38;5;33m"
#define COLOR_SNIPER_HEAD "\033[38;5;196m"
// Inferno colors
#define COLOR_INFERNO_BASE "\033[38;5;202m"
#define COLOR_INFERNO_FIRE1 "\033[38;5;196m"
#define COLOR_INFERNO_FIRE2 "\033[38;5;227m"
#define COLOR_INFERNO_TORCH "\033[38;5;243m"
// Mortier colors
#define COLOR_MORTIER_BASE "\033[38;5;243m"
#define COLOR_MORTIER_FIRING "\033[38;5;208m"
#define COLOR_MORTIER_FIRING_CENTER "\033[38;5;184m"
#define COLOR_MORTIER_COOLING "\033[38;5;14m"
// Banque colors
#define COLOR_BANQUE_BASE "\033[38;5;130m"
#define COLOR_BANQUE_MONEY "\033[38;5;184m"
#define COLOR_BANQUE_GENERATION "\033[38;5;40m"
// Freezer colors
#define COLOR_FREEZER_BASE "\033[38;5;14m"
#define COLOR_FREEZER_FIRING "\033[38;5;50m"
#define COLOR_FREEZER_FIRING_CENTER "\033[38;5;15m"
// Gatling colors
#define COLOR_GATLING_BASE "\033[38;5;238m"
#define COLOR_GATLING_FIRING "\033[38;5;125m"
#define COLOR_GATLING_COOLING "\033[38;5;50m"
// Gatling colors
#define COLOR_PETRIFICATEUR_BASE "\033[38;5;22m"
#define COLOR_PETRIFICATEUR_FIRING "\033[38;5;34m"
#define COLOR_PETRIFICATEUR_SORON "\033[38;5;214m"
#define COLOR_PETRIFICATEUR_BASE_LVL2 "\033[38;5;82m"
#define COLOR_BG_SORON "\033[48;5;214m"
// Ennemies colors
    //Tux colors
    #define COLOR_TUX_BASE "\033[38;5;22m"
    #define COLOR_TUX_EYES "\033[38;5;160m"
    //Speed colors
    #define COLOR_SPEED_BASE "\033[38;5;227m"
    #define COLOR_SPEED_EYES "\033[38;5;229m"
    //Spider colors
    #define COLOR_SPIDER_BASE "\033[38;5;90m"
    #define COLOR_SPIDER_EYES "\033[38;5;125m"
    #define COLOR_SPIDER_LEGS "\033[38;5;57m"
    //Hyperspeed colors
    #define COLOR_HYPERSPEED_BASE "\033[38;5;227m"
    #define COLOR_HYPERSPEED_BOX "\033[38;5;222m"
    #define COLOR_HYPERSPEED_EYES "\033[38;5;81m"
    //Hightux colors
    #define COLOR_HIGHTUX_BASE "\033[38;5;43m"
    #define COLOR_HIGHTUX_CROWN "\033[38;5;226m"
    #define COLOR_HIGHTUX_EYES "\033[38;5;124m"
    //Boss_slime colors
    #define COLOR_BOSS_SLIME_BASE "\033[38;5;21m"
    #define COLOR_BOSS_SLIME_CROWN "\033[38;5;226m"
    #define COLOR_BOSS_SLIME_EYES "\033[38;5;88m"
    #define COLOR_BOSS_SLIME_MOUTH "\033[38;5;90m"
    //Tank colors
    #define COLOR_TANK_BASE "\033[38;5;28m"
    #define COLOR_TANK_CANON "\033[38;5;240m"
    #define COLOR_TANK_URSS "\033[38;5;196m" "\033[1m" "\033[4m"
    //Boss stun colors
    #define COLOR_BOSS_STUN_BASE "\033[38;5;184m"
    #define COLOR_BOSS_STUN_LIGHTNING "\033[38;5;50m"
    #define COLOR_BOSS_STUN_SPARKS "\033[38;5;229m"
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
// Speed controller colors
#define SPEED_CONTROL "\033[38;5;172m"
#define SPEED_CONTROL_2 "\033[38;5;32m"
#define SPEED_CONTROL_3 "\033[38;5;46m"
#define SPEED_CONTROL_4 "\033[38;5;91m"
#define SPEED_CONTROL_5 "\033[38;5;196m"
#define SPEED_CONTROL_PAUSE "\033[38;5;222m"

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
void drawGameSpeed(int game_speed_control, int term_width, int game_speed_anim_pause);