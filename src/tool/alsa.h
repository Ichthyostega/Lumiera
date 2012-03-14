/*
  ALSA.h  -  sound output backend using the Advanced Linux Sound Architecture

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

*/

/** @file alsa.h
 ** Interfacing to ALSA sound output.
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
