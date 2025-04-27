/*
 *  fir_filter.c
 *
 *  Created on: Jan 30, 2025
 *  Author: chase callender
 *
 *  fir filters
 */

#include "main.h"
#include "fir_filter.h"

float filter_taps[FIR_TOTAL_TAPS] = {
		  0.0017504304353826478f,
		  0.001540672792197758f,
		  -0.002247181332656693f,
		  -0.0010784030445493903f,
		  0.003947229431883231f,
		  0.00020708499492591936f,
		  -0.0062683706249557665f,
		  0.0023426173310772016f,
		  0.008218534013512075f,
		  -0.006706637871606385f,
		  -0.009114322419535822f,
		  0.013372563206201124f,
		  0.007296828870355186f,
		  -0.021865867942259358f,
		  -0.0011837135337320409f,
		  0.03162566784201656f,
		  -0.01184596089027074f,
		  -0.041327735714919815f,
		  0.036347802534478106f,
		  0.04970509818618133f,
		  -0.08827470848472999f,
		  -0.0553271401641764f,
		  0.31209179134129f,
		  0.5573465536171263f,
		  0.31209179134129f,
		  -0.0553271401641764f,
		  -0.08827470848472999f,
		  0.04970509818618133f,
		  0.036347802534478106f,
		  -0.041327735714919815f,
		  -0.01184596089027074f,
		  0.03162566784201656f,
		  -0.0011837135337320409f,
		  -0.021865867942259358f,
		  0.007296828870355186f,
		  0.013372563206201124f,
		  -0.009114322419535822f,
		  -0.006706637871606385f,
		  0.008218534013512075f,
		  0.0023426173310772016f,
		  -0.0062683706249557665f,
		  0.00020708499492591936f,
		  0.003947229431883231f,
		  -0.0010784030445493903f,
		  -0.002247181332656693f,
		  0.001540672792197758f,
		  0.0017504304353826478f
	//	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

		};

//initialise filter variables
void init_fir_filter(fir_filter_t *filter)
{
	filter->total_taps = FIR_TOTAL_TAPS;
	//filter->coeffs = &filter_taps[0];	//point the pointer to our array of coefficients

	for(int x = 0; x == filter->total_taps - 1; x++ )
	{
		filter->samples[x] = 0.0f;
	}

	filter->index = 0;
}




//process the signal
float process_fir_filter(fir_filter_t *filter, float latest_sample)
{
	float acc = 0.0f;
	static double debug = 0.0023f;
//	float *coefficients = filter->coeffs;
	uint8_t temp_index;


	//adjust the sample index as we are adding a new sample
	filter->index++;

	//make sure the index is not out of range by wrapping around!
	if(filter->index == filter->total_taps)
	{
		filter->index = 0;
	}

	//add the latest sample to the data array at the updated index
	filter->samples[filter->index] = latest_sample;

	//duplicate the index so we can manipulate it later
	temp_index = filter->index;

	for(int n = 0; n < FIR_TOTAL_TAPS; n++ )
	{

		acc += filter_taps[n] * filter->samples[temp_index];

		if(temp_index > 0)
		{
			temp_index--;
		}
		else
		{
			temp_index = FIR_TOTAL_TAPS - 1;
		}

	}

	return acc;
}
