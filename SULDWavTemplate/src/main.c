#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dsplib\wavefile.h>
#include <stdio.h>
#include "inverter.h"
#include "processing.h"
#include "stdfix_emu.h"


inverter_data_t inverter;
DSPfract sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE];

DSPint main(DSPint argc, char* argv[])
{
		WAVREAD_HANDLE *wav_in;
	    WAVWRITE_HANDLE *wav_out;

		char WavInputName[256];
		char WavOutputName[256];

	    int nChannels;
		int bitsPerSample;
	    int sampleRate;
	    int iNumSamples;
	    DSPint i;
	    DSPint j;

	    //DSPfract argv3 = 0.0;
	    //DSPfract argv4 = -0.99;

		// Init channel buffers
	    for (i = 0; i < MAX_NUM_CHANNEL; i++)
	    	for (j = 0; j < BLOCK_SIZE; j++)
	    		sampleBuffer[i][j] = FRACT_NUM(0.0);

		// Open input wav file
		//-------------------------------------------------
		strcpy(WavInputName, "../TestStreams/titanic_horn.wav");
		wav_in = cl_wavread_open(WavInputName);
		 if(wav_in == NULL)
	    {
	        printf("Error: Could not open wavefile.\n");
	        return -1;
	    }
		//-------------------------------------------------

		// Read input wav header
		//-------------------------------------------------
		nChannels = cl_wavread_getnchannels(wav_in);
	    bitsPerSample = cl_wavread_bits_per_sample(wav_in);
	    sampleRate = cl_wavread_frame_rate(wav_in);
	    iNumSamples =  cl_wavread_number_of_frames(wav_in);
		//-------------------------------------------------

		// Open output wav file
		//-------------------------------------------------
		strcpy(WavOutputName, "../OutStreams/titanic_horn_model3.wav");
		wav_out = cl_wavwrite_open(WavOutputName, bitsPerSample, nChannels+3, sampleRate);
		if(!wav_out)
	    {
	        printf("Error: Could not open wavefile.\n");
	        return -1;
	    }
		//-------------------------------------------------

		audio_invert_init(&inverter, 0.0r, -0.99r);

		// Processing loop
		//-------------------------------------------------
	    {
			int i;
			int j;
			int k;
			int sample;

			// exact file length should be handled correctly...
			for(i=0; i<iNumSamples/BLOCK_SIZE; i++)
			{
				for(j=0; j<BLOCK_SIZE; j++)
				{
					for(k=0; k<nChannels; k++)
					{
						sample = cl_wavread_recvsample(wav_in);
	        			sampleBuffer[k][j] = rbits(sample);
					}
				}

				processing();

				for(j=0; j<BLOCK_SIZE; j++)
				{
					for(k=0; k<nChannels+3; k++)
					{
						sample = bitsr(sampleBuffer[k][j]);
						cl_wavwrite_sendsample(wav_out, sample);
					}
				}
			}
		}

		// Close files
		//-------------------------------------------------
	    cl_wavread_close(wav_in);
	    cl_wavwrite_close(wav_out);
		//-------------------------------------------------

	    return 0;
}
