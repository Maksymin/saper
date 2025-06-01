#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <time.h>
#include <stdbool.h> 

#define MAX_SCORES 10
#define SCORE_FILE "scores.dat"

typedef struct {
    float seconds;
    char date[20];
    char time[9];
    bool isPlaceholder;
} Score;

void InitLeaderboard(void);
void SaveLeaderboard(void);
void LoadLeaderboard(void);
void AddNewScore(float seconds);
void RenderLeaderboard(void);

#endif