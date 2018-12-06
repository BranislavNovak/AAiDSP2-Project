#include "processing.h"
#include "inverter.h"

extern __memY DSPfract sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE];
extern __memY inverter_data_t inverter;
DSPfract initial_input_gain = 0.501187;				// (-6dB)
DSPfract initial_headroom_gain = 0.707946;			// (-3dB)
__memX DSPfract leftTmpBuffer[BLOCK_SIZE];
__memX DSPfract rightTmpBuffer[BLOCK_SIZE];
/*
void processing()
{
	// L  sampleBuffer[0]		R  sampleBuffer[1]		C  sampleBuffer[2]		Ls sampleBuffer[3]		Rs sampleBuffer[4]  //

	__memY DSPfract* ptr = NULL;
	__memX DSPfract* ptr_tmp_left = leftTmpBuffer;
	__memX DSPfract* ptr_tmp_right = rightTmpBuffer;
	DSPint i;

	ptr = sampleBuffer[0];

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing left and right channel with -6dB gain and saving current state to tmp buffers.
		*ptr = *ptr * initial_input_gain;															// L*(-6dB)
		*ptr_tmp_left = (*ptr);															// Saving state to LEFT tmp buffer
		ptr_tmp_left++;

		*(ptr + BLOCK_SIZE) = *(ptr + BLOCK_SIZE) * initial_input_gain;								// R*(-6dB)
		*ptr_tmp_right = (*(ptr + BLOCK_SIZE));											// Saving state to RIGHT tmp buffer
		ptr_tmp_right++;

		ptr++;
	}

	ptr = sampleBuffer[0];
	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing central channel as sum of L and R channel and -3dB gain
		*((ptr + (2 * BLOCK_SIZE))) = (*ptr + (*(ptr + BLOCK_SIZE)))*initial_headroom_gain;
		ptr++;
	}

	// Passing left surround with inverter and -2dB gain
	gst_audio_invert_transform(&inverter, leftTmpBuffer, leftTmpBuffer);

	// Passing left surround with inverter and -2dB gain
	gst_audio_invert_transform(&inverter, rightTmpBuffer, rightTmpBuffer);

	// Reset tmp values
	ptr_tmp_left = leftTmpBuffer;
	ptr_tmp_right = rightTmpBuffer;

	ptr = sampleBuffer[0];
	for (i = 0; i < BLOCK_SIZE; i++)
	{
		// Passing left and right channel with last -6dB frin central channel
		*ptr = *((ptr + (2 * BLOCK_SIZE))) * (MINUS_SIX_DB);								// L = C*(-6dB)
		*(ptr + BLOCK_SIZE) = *((ptr + (2 * BLOCK_SIZE))) * (MINUS_SIX_DB);				// R = C*(-6dB)

																									// Passing left surrond to original Ls buffer with -2dB gain plus left channel
		*((ptr + (3 * BLOCK_SIZE))) = ((*ptr_tmp_left) * (MINUS_TWO_DB)) + *ptr;
		ptr_tmp_left++;

		// Passing right surrond to original Rs buffer with -2dB gain plus right channel
		*((ptr + (4 * BLOCK_SIZE))) = ((*ptr_tmp_right) * (MINUS_TWO_DB)) + *(ptr + BLOCK_SIZE);
		ptr_tmp_right++;

		ptr++;
	}
}
*/
