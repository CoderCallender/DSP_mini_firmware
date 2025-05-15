#ifndef DELAYLINE_H
#define DELAYLINE_H

#include <stdint.h>

// Pre-defined maximum delay line length
#define DELAYLINE_MAXLENGTH 32500/4	//500ms - can cause RAM overflow if too high


typedef struct {

	// Delay line length
	uint32_t length;

	// Delay line circular buffer
	uint32_t index;

	//delay time
	float delay_time_ms;

	//delay mix
	float mix;

	//delay feedback
	float feedback;
	//float memory[DELAYLINE_MAXLENGTH];
	//float memory2[DELAYLINE_MAXLENGTH/2];

	//these must be pointers as the arrays are very large, meaning the memory has
	//to be carefully allocated with the __attribute__ macros
	float *memory_bank_one;
	float *memory_bank_two;	//ccmram





} delayline_t;

void   DelayLine_Init(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);
float  DelayLine_Update(delayline_t *dlyLn, float inp);
void   DelayLine_SetLength(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);

#endif
