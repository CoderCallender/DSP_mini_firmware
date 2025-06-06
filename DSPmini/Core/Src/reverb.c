/*
 * reverb.c
 *
 *  Created on: Jun 5, 2025
 *      Author: chase callender
 *      Reverb effect based heavily on algorithms presented by Geraint Luff
 */

#include "reverb.h"

const int hadamard_four_four[4][4] = {{1, 1, 1, 1}, {1, -1, 1, -1}, {1, 1, -1, -1}, {1, -1, -1, 1}};
const int householder_four_four[4][4] = {{-0.9333, -0.2667, -0.4, -0.5333}, {-0.2667, 0.4667, -0.8, -1.0667}, {-0.4, -0.8, 0.2, -1.6}, {0.5333, -1.0667, -1.6, -0.7333}};

void reverb_delayLine_init(reverb_delayline_t *delay_line, float delayTime_ms, float sampleRate_Hz, float *array)
{

    // Set delay line length
	//TODO: REVERB_DELAYLINE_MAXLENGTH is not a safe feature currently
	delay_line->length = (uint32_t) (0.001f * delayTime_ms * sampleRate_Hz);

    if (delay_line->length > REVERB_DELAYLINE_MAXLENGTH) {

    	delay_line->length = REVERB_DELAYLINE_MAXLENGTH;
    }


    // Clear delay line circular buffer, reset index
    delay_line->index = 0;

    //save memory address
    delay_line->memory_bank = array;

    //ensure memory is clear
    for (uint32_t n = 0; n < REVERB_DELAYLINE_MAXLENGTH; n++)
    {
       // dlyLn->memory[n] = 0.0f;
    	*(delay_line->memory_bank + n) = 0.0f;

    }

}

float reverb_delayline_update(reverb_delayline_t *delay_line, float inp)
{
	// Get current delay line output
	float out = *(delay_line->memory_bank + delay_line->index);

    // Store current input in delay line circular buffer
	*(delay_line->memory_bank + delay_line->index) = inp;

    // Increment delay line index
	delay_line->index++;
    if (delay_line->index >= delay_line->length) {

    	delay_line->index = 0;
    }

    // Return current output
    return out;
}

void reverb_shuffle_inverter(float *input_array, float *output_array)
{

	#if MAX_DIFFUSER_CHANNELS != 4
	#error "only 4 channels currently supported in reverb_shuffle_inverter()"
	#endif

	//randomly shuffle and mess with things
	output_array[0] = input_array[1];
	output_array[1] = input_array[3] * -1;
	output_array[2] = input_array[2];
	output_array[3] = input_array[0];

}

//function to multiply input channels with a hadamard matrix to spread the results out over the channels
void reverb_hadamard_matrix(float *input_array, float *output_array)
{

	for(int row = 0; row < MAX_DIFFUSER_CHANNELS; row++)
	{
		for(int col = 0; col < MAX_DIFFUSER_CHANNELS; col++)
		{
			output_array[row] += input_array[row] * hadamard_four_four[row][col];
		}
	}

}

//householder 4x4 matrix multiplier
void reverb_householder_matrix(float *input_array, float *output_array)
{

	for(int row = 0; row < MAX_DIFFUSER_CHANNELS; row++)
	{
		for(int col = 0; col < MAX_DIFFUSER_CHANNELS; col++)
		{
			output_array[row] += input_array[row] * householder_four_four[row][col];
		}
	}

}
