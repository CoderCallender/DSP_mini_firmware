/*
 *  tremolo.c
 *
 *  Created on: Jun 04, 2025
 *  Author: chase callender
 *
 *  tremolo effects algorithm
 */

#include "tremolo.h"
#include <math.h>


void init_tremolo(tremolo_t *effect, float sample_rate_hz)
{
	//initialise the tremolo effect
	effect->cycle_count = 0;
	effect->rate = 0.5f;
	effect->depth = 0.1f;
	effect->sample_rate_hz = sample_rate_hz;
	effect->sample_time_s = 1 / sample_rate_hz;
	effect->cycle_end_point = (uint32_t)((1 / effect->rate) / (1 / effect->sample_rate_hz)); //sine wave cycle time (T) / sample time (Ts)
	effect->omega = 2 * M_PI * effect->rate;
	//initialise filters for harmonic trem function
	iir_filter_init(&effect->low_pass, effect->sample_rate_hz);
	iir_filter_init(&effect->high_pass, effect->sample_rate_hz);
	iir_lowpass_set_params(&effect->low_pass, 2000.0f);
	iir_highpass_set_params(&effect->high_pass, 200.0f);
}

void update_tremolo_rate(tremolo_t *effect, float rate)
{
	effect->rate = rate;
	effect->omega = 2 * M_PI * effect->rate;
	effect->cycle_end_point = (uint32_t)((1 / effect->rate) / (1 / effect->sample_rate_hz)); //sine wave cycle time (T) / sample time (Ts)
}

void update_tremolo_depth(tremolo_t *effect, float depth)
{
	effect->depth = depth;
}

float process_tremolo_effect(tremolo_t *effect, float input, bool harmonic)
{
	float out, low_pass_out, high_pass_out, pan_multiplier, t;

	effect->cycle_count++;

	if(effect->cycle_count > effect->cycle_end_point)
	{
		effect->cycle_count = 0;
	}

	if(harmonic)
	{
		//create 2 differently filtered data streams
		low_pass_out = iir_filter_update(&effect->low_pass, input);
		high_pass_out = iir_filter_update(&effect->high_pass, input);
	//	high_pass_out = input;
	//	low_pass_out = input;

		//calculate sine wave value for panning between the 2 outputs
		pan_multiplier = ((effect->depth - 0.1) * sin((2 * M_PI * effect->rate) * (effect->cycle_count * effect->sample_time_s))) + (0.5);

	//	out = high_pass_out;
		out = (low_pass_out * pan_multiplier) + (high_pass_out * (1 - pan_multiplier));
	//	t = effect->cycle_count * effect->sample_time_s;
		//phased tremolo
	//	out = (((sin(effect->omega * t)) + 0.5) * low_pass_out)
				//+ (((sin(effect->omega * t)) + 0.5) * high_pass_out) / 2;
						//+ high_pass_out;
	}

	else
	{
		// A * sin(wT) + offset
		out = input * (effect->depth * sin((2 * M_PI * effect->rate) * (effect->cycle_count * effect->sample_time_s)) + (1 - effect->depth));
	}

	return out;
}

float process_harmonic_tremolo_effect(tremolo_t *effect, float input)
{

}
