#include "raylib.h"
#include "raymath.h"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600

#define COLS 12
#define ROWS 12

const int TILE_HEIGHT = WINDOW_HEIGHT / ROWS;
const int TILE_WIDTH = WINDOW_WIDTH / COLS;

typedef struct
{
    int x;
    int y;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int nearbyMineCount;
} sTile;

sTile grid[COLS][ROWS];
int revealedTilesCount;
int currentMinesCount;

float timeStart;
float timeEnd;

const char* Victory = "Wygrales";
const char* Defeat = "Przegrales";
const char* EnterPrompt = "ENTER ABY WROCIC DO MENU";

bool musicEnabled = true;
bool soundEnabled = true;

#define MAX_TEXTURES 1
typedef enum
{
    TEXTURE_FLAG = 0
} texture_asset;

Texture2D textures[MAX_TEXTURES];

#define MAX_SOUNDS 3
typedef enum
{
    SOUND_ONE = 0,
    SOUND_TWO,
    SOUND_THREE
} sound_assets;

Sound sounds[MAX_SOUNDS];

#define MAX_MUS 1
typedef enum
{
    MUSIC = 0
} music_assets;

Music music[MAX_MUS];

typedef enum
{
    STATE_MAIN_MENU = 0,
    STATE_OPTIONS,
    STATE_GAME,
    STATE_DEFEAT,
    STATE_VICTORY
} states;

states gameState;

void GameStartup();
void GameUpdate();
void GameShutdown();
void GameRender();
void GameReset();
void ResetTiles();
void RenderTiles();
void RenderTile(sTile);
int CountNearbyMines(int, int);
bool IsTileIndexValid(int, int);
void RevealTile(int, int);
void FlagTile(int, int);
void RevealTiles(int, int);
void PlayGameSound(int sound);

void PlayGameSound(int sound)
{
    if (soundEnabled)
    {
        PlaySound(sounds[sound]);
    }
}

bool IsTileIndexValid(int col, int row)
{
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
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

            if (IsTileIndexValid(col + colOffset, row + rowOffset)) {
                if (grid[col + colOffset][row + rowOffset].isMine) {
                    count++;
                }
            }
        }
    }
    return count;
}

void ResetTiles()
{
    for (int i = 0; i < COLS; i++) 
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j] = (sTile)
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

void RenderTile(sTile tile)
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

void RenderTiles()
{
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            RenderTile(grid[i][j]);
        }
    }
}

void GameStartup() 
{
    InitAudioDevice();

    Image image1 = LoadImage("resources\\flag.png");
    textures[TEXTURE_FLAG] = LoadTextureFromImage(image1);
    UnloadImage(image1);

    sounds[SOUND_ONE] = LoadSound("resources\\dzwiek1.mp3");
    sounds[SOUND_TWO] = LoadSound("resources\\dzwiek2.mp3");
    sounds[SOUND_THREE] = LoadSound("resources\\dzwiek3.mp3");

    music[MUSIC] = LoadMusicStream("resources\\muzyka.mp3");
    PlayMusicStream(music[MUSIC]);

    gameState = STATE_MAIN_MENU;
}

void GameUpdate()
{
    UpdateMusicStream(music[MUSIC]);

    switch (gameState)
    {
    case STATE_MAIN_MENU:
        if (IsKeyPressed(KEY_N))
        {
            PlayGameSound(SOUND_TWO);
            GameReset();
        }
        else if (IsKeyPressed(KEY_O))
        {
            gameState = STATE_OPTIONS;
            PlayGameSound(SOUND_TWO);
        }
        break;

    case STATE_OPTIONS:
        if (IsKeyPressed(KEY_ENTER))
        {
            gameState = STATE_MAIN_MENU;
            PlayGameSound(SOUND_TWO);
        }

        if (IsKeyPressed(KEY_D))
        {
            soundEnabled = !soundEnabled;
            PlayGameSound(SOUND_ONE);
        }
        if (IsKeyPressed(KEY_M))
        {
            musicEnabled = !musicEnabled;
            PlayGameSound(SOUND_ONE);
            if (musicEnabled)
            {
                StopMusicStream(music[MUSIC]);
                PlayMusicStream(music[MUSIC]);
            }
            else
            {
                StopMusicStream(music[MUSIC]);
            }
        }
        break;

    case STATE_GAME:
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);

            if (IsTileIndexValid(col, row))
            {
                RevealTile(col, row);
                PlayGameSound(SOUND_ONE);
            }
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / TILE_WIDTH);
            int row = (int)(mousePos.y / TILE_HEIGHT);
            if (IsTileIndexValid(col, row))
            {
                FlagTile(col, row);
            }
        }
        break;

    case STATE_DEFEAT:
    case STATE_VICTORY:
        if (IsKeyPressed(KEY_ENTER))
        {
            PlayGameSound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }
        break;
    }
}

void GameRender()
{
    int seconds = 0;

    switch (gameState)
    {
    case STATE_MAIN_MENU:
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, PINK);
        DrawText("SAPER", 20, 20, 40, WHITE);
        DrawText("[N]owa Gra", 120, 220, 20, WHITE);
        DrawText("[O]pcje", 120, 250, 20, WHITE);
        DrawText("ESC aby WYJSC", 120, 280, 20, WHITE);
        break;

    case STATE_OPTIONS:
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GRAY);
        DrawText("SAPER - OPCJE", 20, 20, 40, WHITE);
        DrawText("[D]zwieki", 120, 220, 20, WHITE);
        if (soundEnabled)
        {
            DrawText("WLACZONE", 250, 220, 20, YELLOW);
            DrawText("/", 365, 220, 20, WHITE);
            DrawText("WYLACZONE", 385, 220, 20, WHITE);
        }
        else
        {
            DrawText("WLACZONE", 250, 220, 20, WHITE);
            DrawText("/", 365, 220, 20, WHITE);
            DrawText("WYLACZONE", 385, 220, 20, YELLOW);
        }
        DrawText("[M]uzyka", 120, 250, 20, WHITE);
        if (musicEnabled)
        {
            DrawText("WLACZONE", 250, 250, 20, YELLOW);
            DrawText("/", 365, 250, 20, WHITE);
            DrawText("WYLACZONE", 385, 250, 20, WHITE);
        }
        else
        {
            DrawText("WLACZONE", 250, 250, 20, WHITE);
            DrawText("/", 365, 250, 20, WHITE);
            DrawText("WYLACZONE", 385, 250, 20, YELLOW);
        }
        DrawText(EnterPrompt, WINDOW_WIDTH / 2 - MeasureText(EnterPrompt, 34) / 2, (int)(WINDOW_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        break;

    case STATE_GAME:
        RenderTiles();
        break;

    case STATE_DEFEAT:
        RenderTiles();
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(Defeat, WINDOW_WIDTH / 2 - MeasureText(Defeat, 60) / 2, WINDOW_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(EnterPrompt, WINDOW_WIDTH / 2 - MeasureText(EnterPrompt, 34) / 2, (int)(WINDOW_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeEnd - timeStart) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        break;

    case STATE_VICTORY:
        RenderTiles();
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(Victory, WINDOW_WIDTH / 2 - MeasureText(Victory, 60) / 2, WINDOW_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(EnterPrompt, WINDOW_WIDTH / 2 - MeasureText(EnterPrompt, 34) / 2, (int)(WINDOW_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeEnd - timeStart) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        break;
    }
}

void GameShutdown()
{
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        UnloadTexture(textures[i]);
    }

    StopMusicStream(music[MUSIC]);
    for (int i = 0; i < MAX_MUS; i++)
    {
        UnloadMusicStream(music[i]);
    }

    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        UnloadSound(sounds[i]);
    }

    CloseAudioDevice();
}

void GameReset()
{
    revealedTilesCount = 0;
    timeStart = (float)GetTime();
    ResetTiles();
    gameState = STATE_GAME;
}

int main()
{
    InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "SAPER");
    SetTargetFPS(60);
    GameStartup();

    while (!WindowShouldClose())
    {
        GameUpdate();
        BeginDrawing();
        ClearBackground(DARKBLUE);
        GameRender();
        EndDrawing();
    }

    GameShutdown();
    CloseWindow();

    return 0;
}