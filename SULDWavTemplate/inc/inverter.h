#include "common.h"

typedef struct {
	DSPfract degree;
	DSPfract gain;
} inverter_data_t;


extern __memY inverter_data_t inverter;

void audio_invert_init(__memY inverter_data_t * data, DSPfract degree, DSPfract gain);
void gst_audio_invert_transform(__memY inverter_data_t * data, __memX DSPfract * input, __memX DSPfract * output);
