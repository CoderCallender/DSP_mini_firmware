/*
 *  fir_filter.c
 *
 *  Created on: Jan 30, 2025
 *  Author: chase callender
 *
 *  fir filters
 */

#include "iir_filter.h"

void iir_filter_init(iir_filter_t *filt, float sampleRate_Hz)
{
	//calculate the sample time
	filt ->sampleTime_hz = sampleRate_Hz;

	//clear filter memory
	for (uint8_t n = 0; n < 3; n++)
	{
		filt -> x[n] = 0.0f;
		filt -> y[n] = 0.0f;
	}




}

//2nd order low pass filter
void iir_lowpass_set_params(iir_filter_t *filt, float cut_off_hz)
{
	float Y, D;

	Y = tanf( M_PI * cut_off_hz / filt ->sampleTime_hz);
	D = pow(Y,2) + sqrt(2) * Y + 1;

	filt -> a[0] = 1;
	filt -> a[1] = (2 * (pow(Y,2) - 1)) / D;
	filt -> a[2] = (pow(Y,2) - sqrt(2) * Y + 1) / D;
	filt -> b[0] = (pow(Y,2)) / D;
	filt -> b[1] = (2 * (pow(Y,2))) / D;
	filt -> b[2] = filt -> b[0];
}

void iir_highpass_set_params(iir_filter_t *filt, float cut_off_hz)
{
	float Y, D;

	Y = tanf( M_PI * cut_off_hz / filt ->sampleTime_hz);
	D = pow(Y,2) + sqrt(2) * Y + 1;

	filt -> a[0] = 1;
	filt -> a[1] = (2 * (pow(Y,2) - 1)) / D;
	filt -> a[2] = (pow(Y,2) - sqrt(2) * Y + 1) / D;
	filt -> b[0] = 1 / D;
	filt -> b[1] = -2 / D;
	filt -> b[2] = filt -> b[0];
}

float iir_filter_update(iir_filter_t *filt, float in)
{
	//shift input samples
	filt ->x[2] = filt ->x[1];
	filt ->x[1] = filt ->x[0];
	filt ->x[0] = in;

	//shift output samples
	filt ->y[2] = filt ->y[1];
	filt ->y[1] = filt ->y[0];

	//calculate new filter output
	filt ->y[0] = (filt ->b[0] * filt ->x[0]) + (filt ->b[1] * filt ->x[1]) + (filt ->b[2] * filt ->x[2]) - (filt ->a[1] * filt ->y[1]) - (filt ->a[2] * filt ->y[2]);

	return (filt -> y[0]);
}
