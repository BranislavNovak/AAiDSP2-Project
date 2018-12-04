typedef struct {
	float degree;
	float gain;
} inverter_data_t;

extern inverter_data_t inverter;

void audio_invert_init(inverter_data_t * data, float degree, float gain);
void gst_audio_invert_transform(inverter_data_t * data, double * input, double * output, unsigned int num_samples);