/*
  OutputProbe  -  tool to investigate external output connections

  Copyright (C)         Lumiera.org
    2011,               Odin Omdal Hørthe <odin.omdal@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

* *****************************************************/


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
