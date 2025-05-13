#include "delay_line.h"

void DelayLine_Init(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz) {

    // Set delay line length
    DelayLine_SetLength(dlyLn, delayTime_ms, sampleRate_Hz);


    // Clear delay line circular buffer, reset index
    dlyLn->index = 0;

    for (uint32_t n = 0; n < DELAYLINE_MAXLENGTH; n++) {
        dlyLn->memory[n] = 0.0f;
    }

}

float DelayLine_Update(delayline_t *dlyLn, float inp) {

	// Get current delay line output
	float out = dlyLn->memory[dlyLn->index];

    // Store current input in delay line circular buffer
    dlyLn->memory[dlyLn->index] = inp;

    // Increment delay line index
    dlyLn->index++;
    if (dlyLn->index >= dlyLn->length) {

        dlyLn->index = 0;

    }

    // Return current output
    return out;

}

void DelayLine_SetLength(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz) {

    dlyLn->length = (uint32_t) (0.001f * delayTime_ms * sampleRate_Hz);

    if (dlyLn->length > DELAYLINE_MAXLENGTH) {

        dlyLn->length = DELAYLINE_MAXLENGTH;

    }

}
