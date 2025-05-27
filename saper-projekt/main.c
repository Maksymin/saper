#include "raylib.h"
#include "raymath.h"

#define WYSOKOWSC_OKNA 600
#define SZEROKOSC_OKNA 600

#define COLS 12
#define ROWS 12

const int KAFELEK_WYSOKOSC = WYSOKOWSC_OKNA / ROWS;
const int KAFELEK_SZEROKOSC = SZEROKOSC_OKNA / COLS;

typedef struct
{
    int x;
    int y;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int nearbyMineCount;
} sKafelek;

sKafelek grid[COLS][ROWS];
int LiczbaWyswietlonychKafelkow;
int LiczbObecnychMin;

float czasStart;
float czasKoniec;

const char* Wygrana = "Wygrales";
const char* Porazka = "Przegrales";
const char* Enterowanie = "ENTER ABY WROCIC DO MENU";

bool muzykaWloczona = true;
bool dzwiekiWloczone = true;

#define MAX_TEXTURES 1
typedef enum
{
    TEXTURE_FLAG = 0
} texture_asset;

Texture2D textures[MAX_TEXTURES];

#define MAX_SOUNDS 3
typedef enum
{
    MUZYKA_JEDEN = 0,
    MUZYKA_DWA,
    MUZYKA_TRZY
}dzwieki_assety;

Sound dzwieki[MAX_SOUNDS];

#define MAX_MUS 1
typedef enum
{
    MUZYKA = 0
} muzyka_assety;

Music muzyka[MAX_MUS];


typedef enum
{
    STAN_MAIN_MENU = 0,
    STAN_OPCJE,
    STAN_GRA,
    STAN_PRZEGRANA,
    STAN_WYGRANA
} stany;

stany stanyGry;

void GameStartup();
void GameUpdate();
void GameShutdown();
void GameRender();
void GameReser();
void ResetKafelkow();
void RenderKafelki();
void RenderKafelka(sKafelek);
int LiczMinyWokol(int, int);
bool IndexKafelkajestPrawidlowy(int, int);
void WyswietlKafelek(int, int);
void FlagujKafelek(int, int);
void WyswietlKafelki(int, int);
void WydajDzwiek(int sound);

void WydajDzwiek(int sound)
{
    if (dzwiekiWloczone)
    {
    PlaySound(dzwieki[sound]);
    }
}

bool IndexKafelkajestPrawidlowy(int col, int row)
{
    return col >= 0 && col < COLS && row >= 0 && row < ROWS;
}

void WyswietlKafelki(int col, int row)
{
    for (int colOffset = -1; colOffset <= 1; colOffset++)
    {
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
        {
            if (colOffset == 0 && rowOffset == 0)
            {
                continue;
            }

            if (!IndexKafelkajestPrawidlowy(col + colOffset, row + rowOffset))
            {
                continue;
            }

            WyswietlKafelek(col + colOffset, row + rowOffset);
        }
    }
}

void FlagujKafelek(int col, int row)
{
    if (grid[col][row].isFlagged)
    {
        return;
    }
    grid[col][row].isFlagged = !grid[col][row].isFlagged;
    WydajDzwiek(MUZYKA_TRZY);
    
}

void WyswietlKafelek(int col, int row)
{
    if (grid[col][row].isFlagged ||
        grid[col][row].isRevealed)
    {
        return;
    }

    grid[col][row].isRevealed = true;

    if (grid[col][row].isMine)
    {
        //Koniec gry - przegrana
        stanyGry = STAN_PRZEGRANA;
        czasKoniec = (float)GetTime();
        WydajDzwiek(MUZYKA_DWA);
    }
    else 
    {
        if (grid[col][row].nearbyMineCount == 0)
        {
            WyswietlKafelki(col, row);
        }
        LiczbaWyswietlonychKafelkow++;

        if (LiczbaWyswietlonychKafelkow >= (ROWS * COLS - LiczbObecnychMin)) 
        {
            //Koniec Gry - wygrana
            stanyGry = STAN_WYGRANA;
            czasKoniec = (float)GetTime();
        }
    }
}

int LiczMinyWokol(int col, int row)
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

            if (IndexKafelkajestPrawidlowy(col + colOffset, row + rowOffset)) {
                if (grid[col + colOffset][row + rowOffset].isMine) {
                    count++;
                }
            }

        }
    }
    return count;
}


void ResetKafelkow()
{
    for (int i = 0; i < COLS; i++) 
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j] = (sKafelek)
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

    LiczbObecnychMin = (int)(ROWS * COLS * 0.15f);
    int minyDoUmiejscowienia = LiczbObecnychMin;
    while (minyDoUmiejscowienia > 0)
    {
        int col = GetRandomValue(0, COLS - 1);
        int row = GetRandomValue(0, ROWS - 1);

        if (!grid[col][row].isMine)
        {
            grid[col][row].isMine = true;
            minyDoUmiejscowienia--;
        }
    }

    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            if (!grid[i][j].isMine)
            {
                grid[i][j].nearbyMineCount = LiczMinyWokol(i, j);
            }
        }
    }
}


void RenderKafelka(sKafelek kafelek)
{
    if (kafelek.isRevealed)
    {
        if (kafelek.isMine)
        {
            DrawRectangle(kafelek.x * KAFELEK_SZEROKOSC, kafelek.y * KAFELEK_WYSOKOSC, KAFELEK_SZEROKOSC, KAFELEK_WYSOKOSC, RED);

        }
        else
        {
            DrawRectangle(kafelek.x * KAFELEK_SZEROKOSC, kafelek.y * KAFELEK_WYSOKOSC, KAFELEK_SZEROKOSC, KAFELEK_WYSOKOSC, LIGHTGRAY);
            if (kafelek.nearbyMineCount > 0)
            {
                DrawText(TextFormat("%d", kafelek.nearbyMineCount), kafelek.x * KAFELEK_SZEROKOSC + 13, kafelek.y * KAFELEK_WYSOKOSC + 4, KAFELEK_WYSOKOSC - 8, DARKGRAY);

            }
        }
    }
    else if (kafelek.isFlagged)
    {
        Rectangle source = { 0, 0, (float)textures[TEXTURE_FLAG].width, (float)textures[TEXTURE_FLAG].height };
        Rectangle dest = { (float)(kafelek.x * KAFELEK_SZEROKOSC), (float)(kafelek.y * KAFELEK_WYSOKOSC), (float)KAFELEK_SZEROKOSC, (float)KAFELEK_WYSOKOSC };
        Vector2 orgin = { 0, 0 };
        DrawTexturePro(textures[TEXTURE_FLAG], source, dest, orgin, 0.0f, WHITE);
    }
    DrawRectangleLines(kafelek.x * KAFELEK_SZEROKOSC, kafelek.y * KAFELEK_WYSOKOSC, KAFELEK_SZEROKOSC, KAFELEK_WYSOKOSC, WHITE);
}


void RenderKafelki()
{
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            RenderKafelka(grid[i][j]);
        }
    }
}

void GameStartup() 
{
    InitAudioDevice();

    Image image1 = LoadImage("assety\\flag.png");
    textures[TEXTURE_FLAG] = LoadTextureFromImage(image1);
    UnloadImage(image1);

    dzwieki[MUZYKA_JEDEN] = LoadSound("assety\\dzwiek1.mp3");
    dzwieki[MUZYKA_DWA] = LoadSound("assety\\dzwiek2.mp3");
    dzwieki[MUZYKA_TRZY] = LoadSound("assety\\dzwiek3.mp3");

    muzyka[MUZYKA] = LoadMusicStream("assety\\muzyka.mp3");

    PlayMusicStream(muzyka[MUZYKA]);

    stanyGry = STAN_MAIN_MENU;
}
void GameUpdate()
{
    UpdateMusicStream(muzyka[MUZYKA]);

    switch (stanyGry)
    {
    case STAN_MAIN_MENU:
        
        if (IsKeyPressed(KEY_N))
        {
            WydajDzwiek(MUZYKA_DWA);
            GameReser();
        }
        else if (IsKeyPressed(KEY_O))
        {
            stanyGry = STAN_OPCJE;
            WydajDzwiek(MUZYKA_DWA);
        }
        break;
    case STAN_OPCJE:

        if (IsKeyPressed(KEY_ENTER))
        {
            stanyGry = STAN_MAIN_MENU;
            WydajDzwiek(MUZYKA_DWA);
        }

        if (IsKeyPressed(KEY_D))
        {
            dzwiekiWloczone = !dzwiekiWloczone;
            WydajDzwiek(MUZYKA_JEDEN);
        }
        if (IsKeyPressed(KEY_M))
        {
            muzykaWloczona = !muzykaWloczona;
            WydajDzwiek(MUZYKA_JEDEN);
            if (muzykaWloczona)
            {
                StopMusicStream(muzyka[MUZYKA]);
                PlayMusicStream(muzyka[MUZYKA]);
            }
            else
            {
                StopMusicStream(muzyka[MUZYKA]);
            }
        }
        break;
    case STAN_GRA:
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 pozycjaMyszy = GetMousePosition();
            int col = (int)(pozycjaMyszy.x / KAFELEK_SZEROKOSC);
            int row = (int)(pozycjaMyszy.y / KAFELEK_WYSOKOSC);

            if (IndexKafelkajestPrawidlowy(col, row))
            {
                WyswietlKafelek(col, row);
                WydajDzwiek(MUZYKA_JEDEN);
            }
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vector2 pozycjaMyszy = GetMousePosition();
            int col = (int)(pozycjaMyszy.x / KAFELEK_SZEROKOSC);
            int row = (int)(pozycjaMyszy.y / KAFELEK_WYSOKOSC);
            if (IndexKafelkajestPrawidlowy(col, row))
            {
                FlagujKafelek(col, row);
            }
        }
        break;
    case STAN_PRZEGRANA:

        if (IsKeyPressed(KEY_ENTER))
        {
            WydajDzwiek(MUZYKA_DWA);

            stanyGry = STAN_MAIN_MENU;
        }
        break;
    case STAN_WYGRANA:

        if (IsKeyPressed(KEY_ENTER))
        {
            WydajDzwiek(MUZYKA_DWA);

            stanyGry = STAN_MAIN_MENU;
        }
        break;
    }


}
void GameRender()
{
    int sekundy = 0;

    switch (stanyGry)
    {
    case STAN_MAIN_MENU:
        DrawRectangle(0, 0, SZEROKOSC_OKNA, WYSOKOWSC_OKNA, PINK);
        DrawText("SAPER", 20, 20, 40, WHITE);
        DrawText("[N]owa Gra", 120, 220, 20, WHITE);
        DrawText("[O]pcje", 120, 250, 20, WHITE);
        DrawText("ESC aby WYJSC", 120, 280, 20, WHITE);
        break;
    case STAN_OPCJE:
        DrawRectangle(0, 0, SZEROKOSC_OKNA, WYSOKOWSC_OKNA, GRAY);
        DrawText("SAPER - OPCJE", 20, 20, 40, WHITE);
        DrawText("[D]zieki", 120, 220, 20, WHITE);
        if (dzwiekiWloczone)
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
        if (muzykaWloczona)
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
        DrawText(Enterowanie, SZEROKOSC_OKNA / 2 - MeasureText(Enterowanie, 34) / 2, (int)(WYSOKOWSC_OKNA * 0.75f) - 10, 34, DARKGRAY);
        break;
    case STAN_GRA:
        RenderKafelki();
        break;
    case STAN_PRZEGRANA:
        RenderKafelki();
        DrawRectangle(0, 0, SZEROKOSC_OKNA, WYSOKOWSC_OKNA, Fade(WHITE, 0.8f));
        DrawText(Porazka, SZEROKOSC_OKNA / 2 - MeasureText(Porazka, 60) / 2, WYSOKOWSC_OKNA / 2 - 10, 60, DARKGRAY);
        DrawText(Enterowanie, SZEROKOSC_OKNA / 2 - MeasureText(Enterowanie, 34) / 2, (int)(WYSOKOWSC_OKNA * 0.75f) - 10, 34, DARKGRAY);
        sekundy = (int)(czasKoniec - czasStart) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", sekundy), 20, 40, 34, DARKGRAY);
        break;
    case STAN_WYGRANA:
        RenderKafelki();
        DrawRectangle(0, 0, SZEROKOSC_OKNA, WYSOKOWSC_OKNA, Fade(WHITE, 0.8f));
        DrawText(Wygrana, SZEROKOSC_OKNA / 2 - MeasureText(Porazka, 60) / 2, WYSOKOWSC_OKNA / 2 - 10, 60, DARKGRAY);
        DrawText(Enterowanie, SZEROKOSC_OKNA / 2 - MeasureText(Enterowanie, 34) / 2, (int)(WYSOKOWSC_OKNA * 0.75f) - 10, 34, DARKGRAY);
        sekundy = (int)(czasKoniec - czasStart) % 60;
        DrawText(TextFormat("TIME PLAYED: %d s", sekundy), 20, 40, 34, DARKGRAY);
        break;
    };
}
void GameShutdown()
{
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
        UnloadTexture(textures[i]);
    }

    StopMusicStream(muzyka[MUZYKA]);
    for (int i = 0; i < MAX_MUS; i++)
    {
        UnloadMusicStream(muzyka[i]);
    }

    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        UnloadSound(dzwieki[i]);
    }

    CloseAudioDevice();
}
void GameReser()
{
    LiczbaWyswietlonychKafelkow = 0;
    czasStart = (float)GetTime();
    ResetKafelkow();
    stanyGry = STAN_GRA;
}



int main()
{
    InitWindow(WYSOKOWSC_OKNA, SZEROKOSC_OKNA, "SAPER");
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

