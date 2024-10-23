#pragma once

#include "shared.h"
#include "enemies.h"

void allocateGridCells(Grid *grid);
void freeGrid(Grid grid);
void genBasicPath(Grid *grid);
void fillBG(int xmin, int ymin, int xmax, int ymax);
void drawCell(struct Cell cell, Grid grid);
void drawFullGrid(Grid grid);
void clearPath(Grid grid);
void clearUsedPath(Grid grid, EnemyPool ep);