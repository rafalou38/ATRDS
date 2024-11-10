#pragma once
#include <ncurses.h>
#include <locale.h>
#include <string.h>

#define MAX_COLOR_PAIRS 256 // Limit for color pair allocation in ncurses

/*
#############
##   IO    ##
#############
*/
#define BUFFER_SIZE 1024
#define fflush(stdout) refresh()

// Colors and styles
#define ERASE_LINE "\33[2K"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define UNDERLINE_RST "\033[24m"
#define BOLD "\033[1m"
#define BOLD_RST "\033[22m"
#define BG_RESET "\033[48;5;0m"
// General colors
#undef COLOR_RED
#define COLOR_RED "\033[38;5;1m"
#undef COLOR_GRAY
#define COLOR_GRAY "\033[38;5;243m"
#undef COLOR_GREEN
#define COLOR_GREEN "\033[38;5;42m"
#undef COLOR_YELLOW
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
// Tux colors
#define COLOR_TUX_BASE "\033[38;5;22m"
#define COLOR_TUX_EYES "\033[38;5;160m"
// Speed colors
#define COLOR_SPEED_BASE "\033[38;5;227m"
#define COLOR_SPEED_EYES "\033[38;5;229m"
// Spider colors
#define COLOR_SPIDER_BASE "\033[38;5;90m"
#define COLOR_SPIDER_EYES "\033[38;5;125m"
#define COLOR_SPIDER_LEGS "\033[38;5;57m"
// Hyperspeed colors
#define COLOR_HYPERSPEED_BASE "\033[38;5;227m"
#define COLOR_HYPERSPEED_BOX "\033[38;5;222m"
#define COLOR_HYPERSPEED_EYES "\033[38;5;81m"
// Hightux colors
#define COLOR_HIGHTUX_BASE "\033[38;5;43m"
#define COLOR_HIGHTUX_CROWN "\033[38;5;226m"
#define COLOR_HIGHTUX_EYES "\033[38;5;124m"
// Boss_slime colors
#define COLOR_BOSS_SLIME_BASE "\033[38;5;21m"
#define COLOR_BOSS_SLIME_CROWN "\033[38;5;226m"
#define COLOR_BOSS_SLIME_EYES "\033[38;5;88m"
#define COLOR_BOSS_SLIME_MOUTH "\033[38;5;90m"
// Tank colors
#define COLOR_TANK_BASE "\033[38;5;28m"
#define COLOR_TANK_CANON "\033[38;5;240m"
#define COLOR_TANK_URSS "\033[38;5;196m" \
                        "\033[1m"        \
                        "\033[4m"
// Boss stun colors
#define COLOR_BOSS_STUN_BASE "\033[38;5;184m"
#define COLOR_BOSS_STUN_LIGHTNING "\033[38;5;50m"
#define COLOR_BOSS_STUN_SPARKS "\033[38;5;229m"
// Terrain colors

#define COLOR_STANDARD_BG "\033[42;222;7;233m"
#define COLOR_TOWER_SLOT_BG "\033[42;223;7;236m"
#define COLOR_PATH_BORDER "\033[38;5;240m"
// #define COLOR_SELECTED_SLOT "\033[38;5;221m"
#define COLOR_SELECTED_SLOT_PLUS_BG "\033[42;228;221;236m"
// Health bar
// #define COLOR_HEALTH_BG "\033[48;5;236m"
#define COLOR_HEALTH_0 "\033[42;224;196;236m"
#define COLOR_HEALTH_25 "\033[42;225;166;236m"
#define COLOR_HEALTH_50 "\033[42;226;178;236m"
#define COLOR_HEALTH_75 "\033[42;227;82;236m"

// ANSI to ncurses mapping structure
typedef struct
{
    const char *ansi_code;
    int ncurses_attr;
    int color_code;
} AnsiMapping;

// Initialize color pairs for 256-color mode
void init_colors();

void custom_printf(const char *format, ...);

#define printf custom_printf
