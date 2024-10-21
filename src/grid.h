#include "shared.h"

void allocateGridCells(Grid *grid);
void freeGrid(Grid grid);
void genBasicPath(Grid *grid);
void drawCell(struct Cell cell, Grid grid);
void drawFullGrid(Grid grid);
void clearPath(Grid grid);