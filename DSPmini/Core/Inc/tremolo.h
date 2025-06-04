/*
 * tremolo.h
 *
 *  Created on: Jun 4, 2025
 *  Author: chase callender
 */

#ifndef INC_TREMOLO_H_
#define INC_TREMOLO_H_

#include "stdint.h"
#include "iir_filter.h"
#include <stdbool.h>

typedef struct {

	// tremolo frequency/rate
	float rate;

	// effect depth (sine amplitude)
	float depth;

	// 1/f so we can work out when to wrap the sine effects
	//float cycle_time;

	//As we know that every sample happens at a defined time,
	//we should be able to calculate the sine wave "on the fly".
	//the only potential issue is making sure we restart the count at
	//the point where the wave starts to repeat
	uint32_t cycle_count;

	uint32_t cycle_end_point;

	//this may make my life easier rather than pasing it in to the functions all the time
	float sample_rate_hz;

	float sample_time_s;

	//declare variables to store our filters in for the harmonic tremolo
	iir_filter_t low_pass;
	iir_filter_t high_pass;

	float omega;

} tremolo_t;

void init_tremolo(tremolo_t *effect, float sample_rate_hz);
void update_tremolo_rate(tremolo_t *effect, float rate);
void update_tremolo_depth(tremolo_t *effect, float depth);
float process_tremolo_effect(tremolo_t *effect, float input, bool harmonic);


#endif /* INC_TREMOLO_H_ */
