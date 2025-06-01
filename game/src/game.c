#include "include/types.h"
#include "include/game.h"
#include "include/grid.h"
#include "include/audio.h"
#include "include/leaderboard.h"

#include "raylib.h"

// Global variables
Tile grid[COLS][ROWS];
int revealedTilesCount;
int currentMinesCount;
float timeStart;
float timeEnd;
bool musicEnabled = true;
bool soundEnabled = true;
bool scoreAdded = false;
Texture2D textures[MAX_TEXTURES];
Sound sounds[MAX_SOUNDS];
Music music[MAX_MUS];
states gameState;

const char* Victory = "Wygrales";
const char* Defeat = "Przegrales";
const char* EnterPrompt = "ENTER ABY WROCIC DO MENU";

void GameStartup(void)
{
    InitAudio();
    InitLeaderboard();
    Image image1 = LoadImage("resources\\flag.png");
    textures[TEXTURE_FLAG] = LoadTextureFromImage(image1);
    UnloadImage(image1);

    gameState = STATE_MAIN_MENU;
}

void GameUpdate(void)
{
    UpdateAudio();

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
        else if (IsKeyPressed(KEY_L)) {
            gameState = STATE_LEADERBOARD;
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

    case STATE_LEADERBOARD:
        if (IsKeyPressed(KEY_ENTER)) {
            gameState = STATE_MAIN_MENU;
            PlayGameSound(SOUND_TWO);
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
        if (!scoreAdded) {
            AddNewScore((int)(timeEnd - timeStart));
            scoreAdded = true;
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            PlayGameSound(SOUND_TWO);
            gameState = STATE_MAIN_MENU;
        }
        break;
    }
}

void GameRender(void)
{
    int seconds = 0;

    switch (gameState)
    {
    case STATE_MAIN_MENU:
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, PINK);
        DrawText("SAPER", 20, 20, 40, WHITE);
        DrawText("[N]owa Gra", 120, 220, 20, WHITE);
        DrawText("[O]pcje", 120, 250, 20, WHITE);
        DrawText("[L]eaderboard", 120, 280, 20, WHITE);
        DrawText("ESC aby WYJSC", 120, 310, 20, WHITE);
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

    case STATE_LEADERBOARD:
        RenderLeaderboard();
        break;

    case STATE_GAME:
        RenderTiles();
        break;

    case STATE_DEFEAT:
        RenderTiles();
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(Defeat, WINDOW_WIDTH / 2 - MeasureText(Defeat, 60) / 2, WINDOW_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(EnterPrompt, WINDOW_WIDTH / 2 - MeasureText(EnterPrompt, 34) / 2, (int)(WINDOW_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeEnd - timeStart);
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        break;

    case STATE_VICTORY:
        RenderTiles();
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(WHITE, 0.8f));
        DrawText(Victory, WINDOW_WIDTH / 2 - MeasureText(Victory, 60) / 2, WINDOW_HEIGHT / 2 - 10, 60, DARKGRAY);
        DrawText(EnterPrompt, WINDOW_WIDTH / 2 - MeasureText(EnterPrompt, 34) / 2, (int)(WINDOW_HEIGHT * 0.75f) - 10, 34, DARKGRAY);
        seconds = (int)(timeEnd - timeStart);
        DrawText(TextFormat("TIME PLAYED: %d s", seconds), 20, 40, 34, DARKGRAY);
        break;
    }
}

void GameShutdown(void)
{
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        UnloadTexture(textures[i]);
    }

    ShutdownAudio();
}

void GameReset(void)
{
    revealedTilesCount = 0;
    timeStart = (float)GetTime();
    ResetTiles();
    gameState = STATE_GAME;
    scoreAdded = false;
}