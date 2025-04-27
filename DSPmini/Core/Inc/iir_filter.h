/*
 * iir_filter.h
 *
 *  Created on: Feb 03, 2025
 *  Author: chase callender
 *
 *  IIR filters
 */

#ifndef INC_IIR_FILTER_H_
#define INC_IIR_FILTER_H_

#include <math.h>
#include <stdint.h>



typedef struct {

	float sampleTime_hz; //Sample time in seconds
	float x[3]; 		//filter inputs (x[0] = current input sample)
	float y[3]; 		//filter outputs (y[0] = current output sample)
	float a[3]; 		//x[n] coefficients
	float b[3]; 		//y[n] coefficients

} iir_filter_t;

void iir_filter_init(iir_filter_t *filt, float sampleRate_Hz);
void iir_lowpass_set_params(iir_filter_t *filt, float cut_off_hz);
void iir_highpass_set_params(iir_filter_t *filt, float cut_off_hz);
float iir_filter_update(iir_filter_t *filt, float in);

#endif /* INC_IIR_PEAKINGFILTER_H_ */
