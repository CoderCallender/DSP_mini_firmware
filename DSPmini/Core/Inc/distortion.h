/*
 *  distortion.h
 *
 *  Created on: Feb 04, 2025
 *  Author: chase callender
 *
 *  overdrive and distortion functions
 */



typedef struct {

	float threshold; 	//
	float gain;
	float asym_Q;
	float asym_d;

} distortion_t;

void innit_distortion(distortion_t *effect);
float distortion(distortion_t *effect, float input);
float asym_distortion(distortion_t *effect, float input);

#define _USE_MATH_DEFINES
