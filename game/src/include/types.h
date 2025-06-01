#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600
#define COLS 12
#define ROWS 12
#define MAX_TEXTURES 1
#define MAX_SOUNDS 3
#define MAX_MUS 1
#define TILE_HEIGHT (WINDOW_HEIGHT / ROWS)
#define TILE_WIDTH (WINDOW_WIDTH / COLS)

typedef struct {
    int x;
    int y;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int nearbyMineCount;
} Tile;

typedef enum {
    TEXTURE_FLAG = 0
} texture_asset;

typedef enum {
    SOUND_ONE = 0,
    SOUND_TWO,
    SOUND_THREE
} sound_assets;

typedef enum {
    MUSIC = 0
} music_assets;

typedef enum {
    STATE_MAIN_MENU = 0,
    STATE_OPTIONS,
    STATE_LEADERBOARD,
    STATE_GAME,
    STATE_DEFEAT,
    STATE_VICTORY
} states;

#endif