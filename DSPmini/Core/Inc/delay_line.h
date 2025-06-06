#ifndef DELAYLINE_H
#define DELAYLINE_H

#include <stdint.h>
#include <math.h>
#include "main.h"

// Pre-defined maximum delay line length
#define DELAYLINE_MAXLENGTH (uint32_t)SAMPLE_RATE_HZ / 4	//500ms - can cause RAM overflow if too high

#define DELAY_CROSSFADE_TIME_S		0.25

enum delay_state_t
{
	STATE_START,
	STATE_LOCK,
	STATE_SWITCH_BUFFERS,
	STATE_FADE,
	STATE_UNLOCK,
	STATE_RUN,
	STATE_ERROR
};


typedef struct {

	// Delay line length
	uint32_t length;

	// flag to indicate length change DEBUG
	//uint8_t length_flag;

	// Delay line circular buffer
	uint32_t index;

	// state machine variables
	enum delay_state_t delay_state;


	//delay time
	float delay_time_ms;

	//delay mix
	float mix;

	//delay feedback
	float feedback;


	//these must be pointers as the arrays are very large, meaning the memory has
	//to be carefully allocated with the __attribute__ macros
	float *memory_bank_one;
	float *memory_bank_two;	//ccmram

	float fade_multiplier;
	float fade_step;

	//changing the delay time needs careful planning
	//so we need to do it differently than usual to avoid
	//audio artifacts
	uint16_t delay_time_pot_value;
	uint16_t old_delay_time_pot_value;

} delayline_t;

void   DelayLine_Init(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);
float  DelayLine_Update(delayline_t *dlyLn, float inp);
void   DelayLine_SetLength(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);

#endif
