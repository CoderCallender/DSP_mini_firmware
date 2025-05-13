#ifndef DELAYLINE_H
#define DELAYLINE_H

#include <stdint.h>

// Pre-defined maximum delay line length
#define DELAYLINE_MAXLENGTH 32500/2	//500ms - can cause RAM overflow if too high

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
	float memory[DELAYLINE_MAXLENGTH];

} delayline_t;

void   DelayLine_Init(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);
float  DelayLine_Update(delayline_t *dlyLn, float inp);
void   DelayLine_SetLength(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz);

#endif
