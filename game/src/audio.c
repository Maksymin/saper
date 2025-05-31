#include "include/audio.h"
#include "raylib.h"

extern bool musicEnabled;
extern bool soundEnabled;
extern Sound sounds[MAX_SOUNDS];
extern Music music[MAX_MUS];

void PlayGameSound(int sound)
{
    if (soundEnabled)
    {
        PlaySound(sounds[sound]);
    }
}

void InitAudio(void)
{
    InitAudioDevice();
    
    sounds[SOUND_ONE] = LoadSound("resources\\dzwiek1.mp3");
    sounds[SOUND_TWO] = LoadSound("resources\\dzwiek2.mp3");
    sounds[SOUND_THREE] = LoadSound("resources\\dzwiek3.mp3");

    music[MUSIC] = LoadMusicStream("resources\\muzyka.mp3");
    PlayMusicStream(music[MUSIC]);
}

void ShutdownAudio(void)
{
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

void UpdateAudio(void)
{
    UpdateMusicStream(music[MUSIC]);
}