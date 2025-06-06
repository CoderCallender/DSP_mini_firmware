/*
 * reverb.h
 *
 *  Created on: Jun 5, 2025
 *      Author: chase callender
 */

#ifndef INC_REVERB_H_
#define INC_REVERB_H_

#include <stdint.h>
#include "main.h"

#define REVERB_DELAYLINE_MAXLENGTH (uint32_t)SAMPLE_RATE_HZ / 10	//100ms - can cause RAM overflow if too high
#define MAX_DIFFUSER_CHANNELS	4




typedef struct {

	// Delay line length
	uint32_t length;

	// Delay line circular buffer index
	uint32_t index;

	//delay time
	float delay_time_ms;

	//pointer to delay line memory
	float *memory_bank;

} reverb_delayline_t;

void reverb_delayLine_init(reverb_delayline_t *delay_line, float delayTime_ms, float sampleRate_Hz, float *array);
float reverb_delayline_update(reverb_delayline_t *delay_line, float inp);
void reverb_shuffle_inverter(float *input_array, float *output_array);
void reverb_hadamard_matrix(float *input_array, float *output_array);
void reverb_householder_matrix(float *input_array, float *output_array);


#endif /* INC_REVERB_H_ */
