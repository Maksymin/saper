#include "include/game.h"
#include "include/types.h"

int main(void)
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