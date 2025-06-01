#include "include/leaderboard.h"
#include "include/types.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

Score scores[MAX_SCORES];

void InitLeaderboard(void) {
    for (int i = 0; i < MAX_SCORES; i++) {
        scores[i].seconds = 999;
        strcpy(scores[i].date, "...");
        strcpy(scores[i].time, "...");
        scores[i].isPlaceholder = true;
    }
    LoadLeaderboard();
}

void SaveLeaderboard(void) {
    FILE* file = fopen(SCORE_FILE, "wb");
    if (file != NULL) {
        fwrite(scores, sizeof(Score), MAX_SCORES, file);
        fclose(file);
    }
}

void LoadLeaderboard(void) {
    FILE* file = fopen(SCORE_FILE, "rb");
    if (file != NULL) {
        fread(scores, sizeof(Score), MAX_SCORES, file);
        fclose(file);
    }
}

void AddNewScore(float seconds) {
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);


    Score newScore;
    newScore.seconds = seconds;
    strftime(newScore.date, sizeof(newScore.date), "%Y-%m-%d", timeinfo);
    strftime(newScore.time, sizeof(newScore.time), "%H:%M:%S", timeinfo);
    newScore.isPlaceholder = false;

    int pos = -1;
    for (int i = 0; i < MAX_SCORES; i++) {
        if (scores[i].isPlaceholder || newScore.seconds < scores[i].seconds) {
            pos = i;
            break;
        }
    }

    if (pos >= 0) {        
        
        for (int i = MAX_SCORES - 1; i > pos; i--) {
            scores[i] = scores[i - 1];
        }
        scores[pos] = newScore;
        SaveLeaderboard();
    }
}

void RenderLeaderboard(void) {
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, DARKBLUE);
    DrawText("BEST SCORES", 20, 20, 40, WHITE);

    for (int i = 0; i < MAX_SCORES; i++) {
        Color color = scores[i].isPlaceholder ? GRAY : WHITE;
        char scoreText[100];
        sprintf(scoreText, "%2d. %.2f sec  %s  %s",
                i + 1, 
                scores[i].seconds,
                scores[i].date,
                scores[i].time);
        DrawText(scoreText, 50, 100 + i * 40, 20, color);
    }

    DrawText("ENTER - BACK TO MENU", 
             WINDOW_WIDTH / 2 - MeasureText("ENTER - BACK TO MENU", 20) / 2,
             WINDOW_HEIGHT - 40, 20, WHITE);
}