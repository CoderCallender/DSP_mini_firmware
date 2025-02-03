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
		  0.0026202690506943976f,
		  0.0023986901796699926f,
		  -0.004981994389198368f,
		  -0.007196253489198549f,
		  0.0032271931081656624f,
		  0.0046267827462922115f,
		  -0.009550774633449452f,
		  -0.005727195163456699f,
		  0.014251519018857066f,
		  0.002226894967881706f,
		  -0.021920750998186363f,
		  0.004137832984035147f,
		  0.029668281575561728f,
		  -0.017214748084747322f,
		  -0.037666510303603885f,
		  0.040856050616849764f,
		  0.04431630066474415f,
		  -0.09131257961172391f,
		  -0.048859318961933455f,
		  0.3131632234271398f,
		  0.5504305440512707f,
		  0.3131632234271398f,
		  -0.048859318961933455f,
		  -0.09131257961172391f,
		  0.04431630066474415f,
		  0.040856050616849764f,
		  -0.037666510303603885f,
		  -0.017214748084747322f,
		  0.029668281575561728f,
		  0.004137832984035147f,
		  -0.021920750998186363f,
		  0.002226894967881706f,
		  0.014251519018857066f,
		  -0.005727195163456699f,
		  -0.009550774633449452f,
		  0.0046267827462922115f,
		  0.0032271931081656624f,
		  -0.007196253489198549f,
		  -0.004981994389198368f,
		  0.0023986901796699926f,
		  0.0026202690506943976f
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
