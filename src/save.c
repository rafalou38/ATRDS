#include "save.h"

const char *save_path = "./save/";

bool saveProgress(Grid grid, GameStats gameStats, int waveIndex)
{

    int save_index = 0;
    char save_name[64];
    sprintf(save_name, "save_%d_%d-%d_%d.bin", save_index, grid.width, grid.height, waveIndex);

    struct stat folder_stats;
    if (stat(save_path, &folder_stats) != 0)
    {
        mkdir(save_path, 0774);
    }
    else if (!S_ISDIR(folder_stats.st_mode))
    {
        return false;
    }

    char *file_path = alloca(strlen(save_path) + strlen(save_name) + 1);
    memset(file_path, 0, strlen(save_path) + strlen(save_name) + 1);

    strcat(file_path, save_path);
    strcat(file_path, save_name);

    FILE *file = fopen(file_path, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to write save file to path %s", file_path);
        return 0;
    }

    // Write generic data
    struct SerializedGameData data;
    data.grid = grid;
    data.grid.cells = NULL;
    data.gameStats = gameStats;
    data.waveIndex = waveIndex;
    size_t result = fwrite(&data, sizeof(struct SerializedGameData), 1, file);

    if (result == 0)
        return 0;

    for (int x = 0; x < grid.width; x++)
    {
        result = fwrite(grid.cells[x], sizeof(struct Cell), grid.height, file);
        if (result == 0)
            return 0;
    }

    return 1;
}

bool loadProgress(const char *save_name, Grid *grid, GameStats *gameStats, WaveSystem *waveSystem, EnemyPool *enemyPool, Labels *labels)
{
    char *file_path = alloca(strlen(save_path) + strlen(save_name) + 1 + 4);
    memset(file_path, 0, strlen(save_path) + strlen(save_name) + 1 + 4);

    strcat(file_path, save_path);
    strcat(file_path, save_name);
    strcat(file_path, ".bin");

    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to write save file to path %s", file_path);
        return 0;
    }

    struct SerializedGameData data;

    size_t n = fread(&data, sizeof(struct SerializedGameData), 1, file);

    if (n == 0 || data.grid.width > grid->width || data.grid.height > grid->height)
    {
        return 0;
    }

    *gameStats = data.gameStats;
    switchToWave(waveSystem, data.waveIndex);

    freeGrid(*grid);
    *grid = data.grid;
    allocateGridCells(grid);
    for (int x = 0; x < grid->width; x++)
    {
        n = fread(grid->cells[x], sizeof(struct Cell), grid->height, file);
        if (n == 0)
            exit(1);

        for (int y = 0; y < grid->height; y++)
        {
            grid->cells[x][y].selected = false;
        }
    }

    enemyPool->count = 0;
    labels->count = 0;

    return 1;
}

char **listSaves(int *cnt)
{
    // struct dirent *dir;
    DIR *dir = opendir(save_path);
    if (dir == NULL)
    {
        return NULL;
    }
    struct dirent *file_info;
    // char saves[100][256];
    char **saves = calloc(sizeof(char *), 100);
    // memset(saves, 0, sizeof(saves));

    *cnt = 0;
    while ((file_info = readdir(dir)) != NULL)
    {
        if (strncmp(file_info->d_name, ".", 1) == 0 || strncmp(file_info->d_name, "..", 2) == 0)
            continue;
        int n = strlen(file_info->d_name) + 1;
        saves[*cnt] = malloc(n);
        memset(saves[*cnt], 0, n);
        strcpy(saves[*cnt], file_info->d_name);
        (*cnt)++;
    }
    free(dir);

    return saves;
}