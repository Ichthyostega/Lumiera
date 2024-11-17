/*
  ALSA  -  sound output backend using the Advanced Linux Sound Architecture

   Copyright (C)
     2011,            Odin Omdal Hørthe <odin.omdal@gmail.com>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file alsa.c
 ** Example implementation how to access the ALSA sound backend
 */


#include "alsa.h"

#include <alsa/asoundlib.h>

static snd_pcm_t* playback_handle = 0;
static snd_pcm_sw_params_t* sw_params = 0;
static snd_pcm_hw_params_t* hw_params = 0;
static snd_pcm_sframes_t buffer_size = 0;

static snd_pcm_sframes_t written = 0;
static snd_pcm_sframes_t delay = 0;

static unsigned int rate = 44100;

static int audio_initialised = 0;

size_t
audio_offset()
{
  snd_pcm_delay(playback_handle, &delay);

  return written - delay;
}

void
audio_init()
{
  unsigned int buffer_time = 50000;
  const char* device;
  int err;

  if(audio_initialised)
    return;

  audio_initialised = 1;

  device = getenv("ALSA_DEVICE");

  if(!device)
    device = "default";

  if(0 > (err = snd_pcm_open(&playback_handle, device,
                             SND_PCM_STREAM_PLAYBACK, 0/*SND_PCM_NONBLOCK*/)))
    errx(EXIT_FAILURE, "Audio: Cannot open device %s: %s", device, snd_strerror(err));

  if(0 > (err = snd_pcm_sw_params_malloc(&sw_params)))
    errx(EXIT_FAILURE, "Audio: Could not allocate software parameter structure: %s",
         snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_malloc(&hw_params)))
    errx(EXIT_FAILURE, "Audio: Could not allocate hardware parameter structure: %s",
         snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_any(playback_handle, hw_params)))
    errx(EXIT_FAILURE, "Audio: Could not initializa hardware parameters: %s",
         snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_set_access(playback_handle, hw_params,
                                             SND_PCM_ACCESS_RW_INTERLEAVED)))
    errx(EXIT_FAILURE, "Audio: Could not set access type: %s", snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_set_format(playback_handle, hw_params,
                                             SND_PCM_FORMAT_S16)))
    errx(EXIT_FAILURE, "Audio: Could not set sample format to signed 16 bit "
         "native endian: %s", snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params,
                                                &rate, 0)))
    errx(EXIT_FAILURE, "Audio: Could not set sample rate %uHz: %s", rate,
         snd_strerror(err));

  if(0 > (err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)))
    errx(EXIT_FAILURE, "Audio: Could not set channel count to %u: %s",
         2, snd_strerror(err));

  snd_pcm_hw_params_set_buffer_time_near(playback_handle, hw_params, &buffer_time, 0);

  if(0 > (err = snd_pcm_hw_params(playback_handle, hw_params)))
    errx(EXIT_FAILURE, "Audio: Could not set hardware parameters: %s", snd_strerror(err));

  fprintf(stderr, "Buffer time is %.3f seconds\n", buffer_time / 1.0e6);

  if(0 > (err = snd_pcm_sw_params_current(playback_handle, sw_params)))
    errx(EXIT_FAILURE, "Audio: Could not initialise software parameters: %s",
         snd_strerror(err));

  snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, 0);
  snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, 1024);

  snd_pcm_uframes_t min;
  snd_pcm_sw_params_get_avail_min(sw_params, &min);
  fprintf(stderr, "Minimum %u\n", (unsigned) min);

  if(0 > (err = snd_pcm_sw_params(playback_handle, sw_params)))
    errx(EXIT_FAILURE, "Audio: Could not set software parameters: %s",
        snd_strerror(err));

  buffer_size = snd_pcm_avail_update(playback_handle);
}

size_t
audio_write(const void* data, size_t amount)
{
  int err;

  amount /= 4;

  for(;;)
    {
      err = snd_pcm_writei(playback_handle, data, amount);

      if(err == -EAGAIN)
        return 0;

      if(err < 0)
        {
          err = snd_pcm_recover(playback_handle, err, 0);

          if(err < 0)
            errx(EXIT_FAILURE, "Audio playback failed: %s", strerror(-err));
        }


      break;
    }

  written += err;

  err *= 4;

  return err;
}

void
audio_start(unsigned int rate, unsigned int channel_count)
{
  audio_init();

  snd_pcm_prepare(playback_handle);
}

void
audio_stop()
{
  snd_pcm_drain(playback_handle);
}
