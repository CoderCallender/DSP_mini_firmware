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

#ifndef INC_IIR_PEAKINGFILTER_H_
#define INC_IIR_PEAKINGFILTER_H_

#include <math.h>
#include <stdint.h>

typedef struct {

	float sampleTime_s; //Sample time in seconds
	float x[3]; 		//filter inputs (x[0] = current input sample)
	float y[3]; 		//filter outputs (y[0] = current output sample)
	float a[3]; 		//x[n] coefficients
	float b[3]; 		//y[n] coefficients

} IIR_peakingFilter;

void IIR_peakingFilter_init(IIR_peakingFilter *filt, float sampleRate_Hz);
void IIR_peakingFilter_setParams(IIR_peakingFilter *filt, float centreFreq_Hz, float bandwidth_Hz, float boostCut_linear);
float IIR_peakingFilter_update(IIR_peakingFilter *filt, float in);

#endif /* INC_IIR_PEAKINGFILTER_H_ */
