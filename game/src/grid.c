#include "include/grid.h"
#include "include/game.h"
#include "include/audio.h"
#include "raylib.h"

extern Tile grid[COLS][ROWS];
extern int revealedTilesCount;
extern int currentMinesCount;
extern states gameState;
extern float timeEnd;
extern Texture2D textures[];

bool IsTileIndexValid(int col, int row)
{
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
}

void ResetTiles(void)
{
    for (int i = 0; i < COLS; i++) 
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j] = (Tile)
            {
                .x = i,
                .y = j,
                .isMine = false,
                .isRevealed = false,
                .isFlagged = false,
                .nearbyMineCount = -1
            };
        }
    }

    currentMinesCount = (int)(ROWS * COLS * 0.15f);
    int minesToPlace = currentMinesCount;
    while (minesToPlace > 0)
    {
        int col = GetRandomValue(0, COLS - 1);
        int row = GetRandomValue(0, ROWS - 1);

        if (!grid[col][row].isMine)
        {
            grid[col][row].isMine = true;
            minesToPlace--;
        }
    }

    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (!grid[i][j].isMine)
            {
                grid[i][j].nearbyMineCount = CountNearbyMines(i, j);
            }
        }
    }
}

void RenderTile(Tile tile)
{
    if (tile.isRevealed)
    {
        if (tile.isMine)
        {
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
        }
        else
        {
            DrawRectangle(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, LIGHTGRAY);
            if (tile.nearbyMineCount > 0)
            {
                DrawText(TextFormat("%d", tile.nearbyMineCount), tile.x * TILE_WIDTH + 13, tile.y * TILE_HEIGHT + 4, TILE_HEIGHT - 8, DARKGRAY);
            }
        }
    }
    else if (tile.isFlagged)
    {
        Rectangle source = { 0, 0, (float)textures[TEXTURE_FLAG].width, (float)textures[TEXTURE_FLAG].height };
        Rectangle dest = { (float)(tile.x * TILE_WIDTH), (float)(tile.y * TILE_HEIGHT), (float)TILE_WIDTH, (float)TILE_HEIGHT };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(textures[TEXTURE_FLAG], source, dest, origin, 0.0f, WHITE);
    }
    DrawRectangleLines(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, WHITE);
}

void RenderTiles(void)
{
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            RenderTile(grid[i][j]);
        }
    }
}

int CountNearbyMines(int col, int row)
{
    int count = 0;
    for (int colOffset = -1; colOffset <= 1; colOffset++)
    {
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
        {
            if (colOffset == 0 && rowOffset == 0)
            {
                continue;
            }

            if (IsTileIndexValid(col + colOffset, row + rowOffset))
            {
                if (grid[col + colOffset][row + rowOffset].isMine)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

void RevealTile(int col, int row)
{
    if (grid[col][row].isFlagged ||
        grid[col][row].isRevealed)
    {
        return;
    }

    grid[col][row].isRevealed = true;

    if (grid[col][row].isMine)
    {
        gameState = STATE_DEFEAT;
        timeEnd = (float)GetTime();
        PlayGameSound(SOUND_TWO);
    }
    else 
    {
        if (grid[col][row].nearbyMineCount == 0)
        {
            RevealTiles(col, row);
        }
        revealedTilesCount++;

        if (revealedTilesCount >= (ROWS * COLS - currentMinesCount)) 
        {
            gameState = STATE_VICTORY;
            timeEnd = (float)GetTime();
        }
    }
}

void RevealTiles(int col, int row)
{
    for (int colOffset = -1; colOffset <= 1; colOffset++)
    {
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
        {
            if (colOffset == 0 && rowOffset == 0)
            {
                continue;
            }

            if (!IsTileIndexValid(col + colOffset, row + rowOffset))
            {
                continue;
            }

            RevealTile(col + colOffset, row + rowOffset);
        }
    }
}

void FlagTile(int col, int row)
{
    if (grid[col][row].isFlagged)
    {
        return;
    }
    grid[col][row].isFlagged = !grid[col][row].isFlagged;
    PlayGameSound(SOUND_THREE);
}