/*
  ALSA.h  -  sound output backend using the Advanced Linux Sound Architecture

   Copyright (C)
     2011,            Odin Omdal Hørthe <odin.omdal@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file alsa.h
 ** Interfacing to ALSA sound output.
 ** @remarks Example implementation from 2011
 ** 
 ** @todo for now this header defines some functions used for experimentation with ALSA 
 **
 ** @see output-slot.hpp
 */


#ifndef TOOL_ALSA_H
#define TOOL_ALSA_H


#include <stdlib.h>



size_t audio_offset();


void audio_init();


size_t audio_write(const void* data, size_t amount);


void audio_start(unsigned int rate, unsigned int channel_count);


void audio_stop();

#endif // TOOL_ALSA_H
