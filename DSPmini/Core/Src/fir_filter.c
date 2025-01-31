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


//initialise filter variables
void init_fir_filter(fir_filter_t *filter)
{
	filter->total_taps = FIR_TOTAL_TAPS;
	filter->coeffs = &filter_taps[0];	//point the pointer to our array of coefficients

	for(int x = 0; x < filter->total_taps; x++ )
	{
		filter->samples[x] = 0.0f;
	}
}




//process the signal
float process_fir_filter(fir_filter_t *filter, float latest_sample)
{
	float acc = 0;
	float *coefficients = filter->coeffs;
	int16_t temp_index;

	//adjust the sample index as we are adding a new sample
	filter->index--;

	//make sure the index is not out of range by wrapping around!
	if(filter->index < 0)
	{
		//slightly weird way of doing this but I want the latest data to be
		//at the lowest number so that the data array and coefficients array
		//are adjusted in the same direction during processing
		filter->index = filter->total_taps;
	}

	//add the latest sample to the data array at the updated index
	filter->samples[filter->index] = latest_sample;

	//duplicate the index so we can manipulate it later
	temp_index = filter->index;

	for(int n = 0; n < filter->total_taps; n++ )
	{
		acc += *(coefficients + n) * filter->samples[temp_index];
		temp_index++;
		if(temp_index > filter->total_taps)
		{
			temp_index = 0;
		}

	}
	return acc;
}
