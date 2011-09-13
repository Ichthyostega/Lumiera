#include <unistd.h>
#include <stdint.h>
#include "alsa.c"

#define SAMPLE_RATE 44100

int16_t quiet[SAMPLE_RATE], noisy[SAMPLE_RATE];

void main () {

    for (int i=0; i<SAMPLE_RATE; i++)
    {
        quiet[i] = 0;
        noisy[i] = i%30000;
    }
    audio_start(44100, 2);

    for (int i=0; i<10; i++)
    {
        audio_write(noisy, SAMPLE_RATE);
        printf("=================================\n");
        audio_write(quiet, SAMPLE_RATE);
        printf("\n");
    }

    audio_stop();
}
