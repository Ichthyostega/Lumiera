/*
  OutputProbe  -  tool to investigate external output connections

   Copyright (C)
     2011,            Odin Omdal Hørthe <odin.omdal@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file main.c
 ** A stand alone application to test integration with output frameworks
 ** @remarks the plan was to evolve this into a command line tool eventually,
 **          which would use Lumiera's backend functionality to setup and probe
 **          the operation environment for diagnostic purpose
 */


#include "alsa.h"

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>


#define SAMPLE_RATE 44100

int16_t quiet[SAMPLE_RATE],
        noisy[SAMPLE_RATE];


int
main()
  {
    
    for (int i = 0; i < SAMPLE_RATE; i++)
      {
        quiet[i] = 0;
        noisy[i] = i % 30000;
      }
    audio_start (44100, 2);
    
    for (int i = 0; i < 10; i++)
      {
        audio_write (noisy, SAMPLE_RATE);
        printf ("=================================\n");
        audio_write (quiet, SAMPLE_RATE);
        printf ("\n");
      }
    
    audio_stop ();
    
    return 0;
  }
