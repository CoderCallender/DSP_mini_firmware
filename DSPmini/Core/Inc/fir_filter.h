/*
 *  fir_filter.h
 *
 *  Created on: Jan 30, 2025
 *  Author: chase callender
 *
 *  fir filters
 */

#ifndef INC_FIR_FILTER_H_
#define INC_FIR_FILTER_H_



#define FIR_TOTAL_TAPS 		47
//#define FIR_TOTAL_TAPS 		31

typedef struct {

	float samples[FIR_TOTAL_TAPS]; 	//array to hold all the historic input samples
	//float *coeffs; 					//pointer to the FIR filter coefficients
	uint8_t total_taps; 			//total taps of the filter
	uint8_t index;					//value to keep track of the latest sample (so we can use ring buffering)

} fir_filter_t;


void init_fir_filter(fir_filter_t *filter);
float process_fir_filter(fir_filter_t *filter, float latest_sample);






#endif /* INC_FIR_FILTER_H_ */
