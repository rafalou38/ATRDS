#pragma once

#include "shared.h"
#include "enemies.h"

void showTowerSelection(int ligne, bool hasTurret);
void selectionTower(int x0, int y0, bool hasTurret);
void updateTowers(Grid grid, EnemyPool ep, float dt);