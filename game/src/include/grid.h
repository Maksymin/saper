#ifndef GRID_H
#define GRID_H

#include "types.h"

void ResetTiles(void);
void RenderTiles(void);
void RenderTile(Tile tile);
int CountNearbyMines(int col, int row);
bool IsTileIndexValid(int col, int row);
void RevealTile(int col, int row);
void FlagTile(int col, int row);
void RevealTiles(int col, int row);

#endif