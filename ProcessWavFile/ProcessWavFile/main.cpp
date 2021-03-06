
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "WAVheader.h"
#include "inverter.h"

#define BLOCK_SIZE 16
#define MAX_NUM_CHANNEL 8
#define CHANNEL_NUMBER 5

#define MINUS_TWO_DB 0.794328
#define MINUS_SIX_DB 0.501187

double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE];

// Initialization constants
//-------------------------------------------------
const double initial_input_gain = 0.501187;			// (-6dB)
const double initial_headroom_gain = 0.707946;		// (-3dB)
//-------------------------------------------------

void processing(inverter_data_t* inverter);

int main(int argc, char* argv[])
{
	FILE *wav_in=NULL;
	FILE *wav_out=NULL;
	char WavInputName[256];
	char WavOutputName[256];
	WAV_HEADER inputWAVhdr,outputWAVhdr;	
	inverter_data_t inverter;
	float argv3 = (float)atof(argv[3]);
	float argv4 = (float)atof(argv[4]);

	// Init channel buffers
	for(int i=0; i<MAX_NUM_CHANNEL; i++)
		memset(&sampleBuffer[i],0,BLOCK_SIZE);

	// Open input and output wav files
	//-------------------------------------------------
	strcpy(WavInputName,argv[1]);
	wav_in = OpenWavFileForRead (WavInputName,"rb");
	strcpy(WavOutputName,argv[2]);
	wav_out = OpenWavFileForRead (WavOutputName,"wb");
	//-------------------------------------------------

	// Get degree and gain parameters
	//-------------------------------------------------
	audio_invert_init(&inverter, argv3, argv4);
	//-------------------------------------------------

	// Read input wav header
	//-------------------------------------------------
	ReadWavHeader(wav_in,inputWAVhdr);
	//-------------------------------------------------
	
	// Set up output WAV header
	//-------------------------------------------------	
	outputWAVhdr = inputWAVhdr;
	//outputWAVhdr.fmt.NumChannels = inputWAVhdr.fmt.NumChannels; // change number of channels
	outputWAVhdr.fmt.NumChannels  = CHANNEL_NUMBER;

	int oneChannelSubChunk2Size = inputWAVhdr.data.SubChunk2Size/inputWAVhdr.fmt.NumChannels;
	int oneChannelByteRate = inputWAVhdr.fmt.ByteRate/inputWAVhdr.fmt.NumChannels;
	int oneChannelBlockAlign = inputWAVhdr.fmt.BlockAlign/inputWAVhdr.fmt.NumChannels;
	
	outputWAVhdr.data.SubChunk2Size = oneChannelSubChunk2Size*outputWAVhdr.fmt.NumChannels;
	outputWAVhdr.fmt.ByteRate = oneChannelByteRate*outputWAVhdr.fmt.NumChannels;
	outputWAVhdr.fmt.BlockAlign = oneChannelBlockAlign*outputWAVhdr.fmt.NumChannels;


	// Write output WAV header to file
	//-------------------------------------------------
	WriteWavHeader(wav_out,outputWAVhdr);


	// Processing loop
	//-------------------------------------------------	
	{
		int sample;
		int BytesPerSample = inputWAVhdr.fmt.BitsPerSample/8;
		const double SAMPLE_SCALE = -(double)(1 << 31);		//2^31
		int iNumSamples = inputWAVhdr.data.SubChunk2Size/(inputWAVhdr.fmt.NumChannels*inputWAVhdr.fmt.BitsPerSample/8);


		// exact file length should be handled correctly...
		for(int i=0; i<iNumSamples/BLOCK_SIZE; i++)
		{	
			for(int j=0; j<BLOCK_SIZE; j++)
			{
				for(int k=0; k<inputWAVhdr.fmt.NumChannels; k++)
				{	
					sample = 0; //debug
					fread(&sample, BytesPerSample, 1, wav_in);
					sample = sample << (32 - inputWAVhdr.fmt.BitsPerSample); // force signextend
					sampleBuffer[k][j] = sample / SAMPLE_SCALE;				// scale sample to 1.0/-1.0 range		
				}
			}

			processing(&inverter);

			for(int j=0; j<BLOCK_SIZE; j++)
			{
				for(int k=0; k<outputWAVhdr.fmt.NumChannels; k++)
				{	
					sample = sampleBuffer[k][j] * SAMPLE_SCALE ;	// crude, non-rounding 			
					sample = sample >> (32 - inputWAVhdr.fmt.BitsPerSample);
					fwrite(&sample, outputWAVhdr.fmt.BitsPerSample/8, 1, wav_out);		
				}
			}		
		}
	}

	// Close files
	//-------------------------------------------------	
	fclose(wav_in);
	fclose(wav_out);
	//-------------------------------------------------	

	return 0;
}

void processing(inverter_data_t* inverter)
{
	// L  sampleBuffer[0]
	// R  sampleBuffer[1]
	// C  sampleBuffer[2]
	// Ls sampleBuffer[3]
	// Rs sampleBuffer[4]

	int i;
	double leftTmpBuffer[BLOCK_SIZE];
	double rightTmpBuffer[BLOCK_SIZE];

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing left and right channel with -6dB gain and saving current state to tmp buffers.
		sampleBuffer[0][i] *= initial_input_gain;			// L*(-6dB)
		sampleBuffer[1][i] *= initial_input_gain;			// R*(-6dB)

		leftTmpBuffer[i] = sampleBuffer[0][i];				// Saving state to LEFT tmp buffer
		rightTmpBuffer[i] = sampleBuffer[1][i];				// Saving state to RIGHT tmp buffer
	}

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing central channel as sum of L and R channel and -3dB gain
		sampleBuffer[2][i] = (sampleBuffer[0][i] + sampleBuffer[1][i])*initial_headroom_gain;
	}
	
	// Passing left surround with inverter and -2dB gain
	gst_audio_invert_transform(inverter, leftTmpBuffer, leftTmpBuffer, BLOCK_SIZE);
	
	// Passing left surround with inverter and -2dB gain
	gst_audio_invert_transform(inverter, rightTmpBuffer, rightTmpBuffer, BLOCK_SIZE);

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing left and right channel with last -6dB frin central channel
		sampleBuffer[0][i] = sampleBuffer[2][i] * MINUS_SIX_DB;			// L = C*(-6dB)
		sampleBuffer[1][i] = sampleBuffer[2][i] * MINUS_SIX_DB;			// R = C*(-6dB)

		// Passing left surrond to original Ls buffer with -2dB gain plus left channel
		sampleBuffer[3][i] = (leftTmpBuffer[i] * MINUS_TWO_DB) + sampleBuffer[0][i];

		// Passing right surrond to original Rs buffer with -2dB gain plus right channel
		sampleBuffer[4][i] = (rightTmpBuffer[i] * MINUS_TWO_DB) + sampleBuffer[1][i];
	}
}