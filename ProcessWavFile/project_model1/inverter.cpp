/**
 *
 * Swaps upper and lower half of audio samples. Mixing an inverted sample on top of
 * the original with a slight delay can produce effects that sound like resonance.
 * Creating a stereo sample from a mono source, with one channel inverted produces wide-stereo sounds.
 *
 */
#include "inverter.h"
#include "common.h"
#include <stdio.h>

void audio_invert_init(inverter_data_t * data, float degree, float gain)
{
	data->degree = degree;
	data->gain = gain;
}

void gst_audio_invert_transform(inverter_data_t * data, double * input, double * output)
{
  int i;
  float dry = 1.0 - data->degree;
  float val;

  for(i = 0; i < BLOCK_SIZE; i++)
  {
	val = (*input) * dry - (1.0 + (*input++)) * data->degree;
	(*output++) = (double)(val * data->gain);
  }
}