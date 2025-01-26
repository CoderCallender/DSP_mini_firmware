/*
 * IIR_PeakingFilter.h
 *
 *  Created on: Dec 7, 2024
 *  Author: chase callender
 *
 *  IIR filter useful for equalisers. Can raise and lower gain at movable frequencies
 *  Taken from Philip Salmony's design
 *  https://www.youtube.com/watch?v=4o-_gUht_Xc&t=907s
 */

#include "IIR_PeakingFilter.h"

void IIR_peakingFilter_init(IIR_peakingFilter *filt, float sampleRate_Hz)
{
	//calculate the sample time
	filt ->sampleTime_s = 1.0f / sampleRate_Hz;

	//clear filter memory
	for (uint8_t n = 0; n < 3; n++)
	{
		filt -> x[n] = 0.0f;
		filt -> y[n] = 0.0f;
	}

	//set all parameters for an all pass filter
	IIR_peakingFilter_setParams(filt, 1.0f, 0.0f, 1.0f);

	//temporary test code, set a bass boost (boostcut max = 2.0)
	//IIR_peakingFilter_setParams(filt, 8000.0f, 50.0f, 2.0f);
}


//function to compute the parameters of the IIR filter. If boostCut_linear > 1, boost. If less than 1, cut
void IIR_peakingFilter_setParams(IIR_peakingFilter *filt, float centreFreq_Hz, float bandwidth_Hz, float boostCut_linear)
{
	//convert Hz to radians/sec, pre warp cut-off frequency (bilinear transform), multiply by sampling time
	float wcT = 2.0f * tanf( M_PI * centreFreq_Hz * filt ->sampleTime_s);

	//calculate Q
	float Q = centreFreq_Hz / bandwidth_Hz;

	//calculate filter coefficients
	filt -> a[0] = 4.0f + 2.0f * (boostCut_linear / Q) * wcT + wcT * wcT;
	filt -> a[1] = 2.0f * wcT * wcT - 8.0f;
	filt -> a[2] = 4.0f - 2.0f * (boostCut_linear / Q) * wcT + wcT * wcT;

	filt -> b[0] = 1.0f / (4.0f + 2.0f / Q * wcT + wcT * wcT);
	filt -> b[1] = -(2.0f * wcT  * wcT - 8.0f);
	filt -> b[2] = -(4.0f - 2.0f / Q * wcT + wcT * wcT);

}

float IIR_peakingFilter_update(IIR_peakingFilter *filt, float in)
{
	//shift input samples
	filt ->x[2] = filt ->x[1];
	filt ->x[1] = filt ->x[0];
	filt ->x[0] = in;

	//shift output samples
	filt ->y[2] = filt ->y[1];
	filt ->y[1] = filt ->y[0];

	//calculate new filter output
	filt ->y[0] = (filt -> a[0] * filt -> x[0] + filt -> a[1] * filt -> x[1] + filt -> a[2] * filt -> x[2]
						+ (filt -> b[1] * filt -> y[1] + filt -> b[2] * filt -> y[2])) * filt -> b[0];

	return (filt -> y[0]);
}
