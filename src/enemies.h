#pragma once

#include "shared.h"
#include "grid.h"

// Enumérations et structures des éléments liées aux ennemis

enum EnemyType
{
    ENEMY_TUX,
    ENEMY_SPEED,
    ENEMY_BOSS,
    ENEMY_HYPERSPEED,
    ENEMY_SPIDER,
    ENEMY_HIGHTUX,
    ENEMY_SLOWBOSS,
    ENEMY_BOSS_STUN,
    ENEMY_COUNT // Doit être placé dernier Ceci n'est pas un vrai ennemi mais permet de définir automatiquement combien d’ennemis sont dans l'enum
};

enum EnemyState
{
    ENEMY_STATE_ALIVE,
    ENEMY_STATE_DEAD,
    ENEMY_STATE_ARRIVED
};

struct Enemy
{
    enum EnemyType type;
    float hp;
    float maxHP;
    float speed;
    int damage;
    int money;

    enum EnemyState state;

    bool on_last_cell;
    struct Cell next_cell;
    struct Cell previous_cell;

    bool has_effect;
    enum EffectType effet;
    float puissance_effet;
    float temps_recharge;
    float last_hit;
    float temps_rest;

    // Indice intentionnellement flottant pour permette un positionnement dynamique dans chaque cellule
    float grid_x;
    float grid_y;

    float term_x;
    float term_y;
};

EnemyPool AllocEnemyPool();
void freeEnemyPool(EnemyPool ep);
struct Enemy defEnemy(Grid grid, enum EnemyType type, int start_x, int start_y);
struct Enemy *addEnemy(Grid grid, EnemyPool *ep, enum EnemyType type, int start_x, int start_y);
void defragEnemyPool(EnemyPool *ep);
void drawEnemies(EnemyPool ep, Grid grid);
void updateEnemies(EnemyPool *ep, Grid grid, GameStats *gs, Labels *labels, float dt_sec);

/**
 * #################
 * ## WAVE SYSTEM ##
 * #################
 */

typedef struct WavePattern
{
    int target_HP;   // le nombre de HP ennemis qui doivent être spawnées durant cette vague
    int target_HPPS; // a quelle vitesse les HP ennemis doivent spawn (Health Points Per Second)
    // float HPPS_factor; // facteur sur le target_HPPS pour un unique ennemi: 1 => 1 ennemi a target_HPPS HP par seconde max; 0.5 => 1 ennemi a target_HPPS*0.5 HP par seconde max
    // Plutôt utile si PV des ennemis variable car sinon, seulement les ennemis choisis spawnent, si on a choisi de spawn un gros, il ne faut pas l’empêcher

    float random_coeffs[ENEMY_COUNT]; // coefficients de spawn pour chaque ennemi
    float coeff_sum;
    float min_spawns[ENEMY_COUNT]; // si nécessaire de faire spawn un minimum/nombre fixe d'un certain ennemi (ex: Boss)
} WavePattern;

typedef struct WaveSystem
{
    int current_wave_index;
    WavePattern current_wave_pattern;

    float wave_HP_left;
    float next_spawn_timer;
    float wave_timer;

    struct Enemy prev_spawn;
} WaveSystem;


void switchToWave(WaveSystem *ws, int waveIndex);
/**
 * Renvoie le pattern de wave correspondant a l'index, la structure des vagues est soi calculée soit prédéfinie dans cette fonction
 * Initialise target_HP et target_HPPS selon des fonctions ajustées au tableur
 *
 * */
WavePattern getWaveByIndex(int waveIndex);

/**
 * Mets a jour le système de vagues
 * tant que wave_HP_left > 0
 * alors
 * - choisit un ennemi E aléatoirement selon:
 *      - les coeff
 *      - de sorte a ce que E.hp < target_HP (un ennemi ne doit pas avoir plus de PV qu'une vague entière)
 *      - ( de sorte a ce que E.hp < target_HPPS * HPPS_factor pour empêcher de placer de gros ennemis involontairement)
 * - ajoute l'ennemi a l'EnemyPool
 * - wave_HP_left -= E.hp
 * - Attendre T = E.hp / target_HPPS (période avant le prochain spawn)
 *      ici on utilisera next_spawn_timer et prev_spawn=E, ainsi, le prochain spawn arrivera après un certain nombre de frames.
 * fin tant que
 */
int updateWaveSystem(WaveSystem *ws, Grid grid, EnemyPool *ep, float dt);

// Réalise et affiche un simulation sur plusieurs vagues
void testWaveSystem(Grid grid, EnemyPool *ep, int n);