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

void audio_invert_init(__memY inverter_data_t * data, DSPfract degree, DSPfract gain)
{
	data->degree = degree; // FRACT_NUM(0.0);
	data->gain = gain;  //FRACT_NUM(-0.99);
}

void gst_audio_invert_transform(__memY inverter_data_t * data, __memX DSPfract * input, __memX DSPfract * output)
{
	DSPint i;
	DSPfract dry = FRACT_NUM(0.9999) - data->degree;
	DSPaccum val;
	DSPfract tmp;
	DSPfract tmp1 = FRACT_NUM(0.5);
	dry = dry + FRACT_NUM(0.0001);

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		tmp = (tmp1 + ((*input) >> 1));
		val = ((*input) >> 1) * (dry)-tmp * data->degree;
		input++;
		(*output) = ((DSPfract)val * data->gain) << 1;
		output++;
	}
}
