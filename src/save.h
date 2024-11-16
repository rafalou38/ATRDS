#include "shared.h"
#include "enemies.h"

#include "string.h"
#include "alloca.h"

#include "sys/stat.h"
#include <dirent.h>

struct SerializedGameData
{
    Grid grid; // tout le contenu de la grid avec cells en NULL
    GameStats gameStats;
    int waveIndex;
};

bool saveProgress(Grid grid, GameStats gameStats, int waveIndex, int save_index);
bool loadProgress(const char *save_name, Grid *grid, GameStats *gameStats, WaveSystem *waveSystem, EnemyPool *enemyPool, Labels *labels);
char **listSaves(int *cnt);