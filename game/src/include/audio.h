#ifndef AUDIO_H
#define AUDIO_H

#include "types.h"

void PlayGameSound(int sound);
void InitAudio(void);
void ShutdownAudio(void);
void UpdateAudio(void);

#endif