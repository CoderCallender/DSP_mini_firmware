#include <stdlib.h>	//used for abs() who knew!?
#include "delay_line.h"
#include "main.h"

float memory_in_ram[DELAYLINE_MAXLENGTH];
float memory_in_ccm_ram[DELAYLINE_MAXLENGTH] __attribute__((section (".ccmram")));

void DelayLine_Init(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz) {

    // Set delay line length
    DelayLine_SetLength(dlyLn, delayTime_ms, sampleRate_Hz);


    // Clear delay line circular buffer, reset index
    dlyLn->index = 0;

    dlyLn->memory_bank_one = &memory_in_ram;
    dlyLn->memory_bank_two = &memory_in_ccm_ram;	//point mem2 at our ccm memory buffer

    for (uint32_t n = 0; n < DELAYLINE_MAXLENGTH; n++)
    {
       // dlyLn->memory[n] = 0.0f;
    	*(dlyLn->memory_bank_one + n) = 0.0f;
    	*(dlyLn->memory_bank_two + n) = 0.0f;
    }

    //set initial state
    dlyLn->delay_state = STATE_RUN;

    //
    dlyLn->fade_step = 1 / (SAMPLE_RATE_HZ * (float)DELAY_CROSSFADE_TIME_S);
    dlyLn->fade_multiplier = 0;

    dlyLn->delay_time_pot_value = 0;
    dlyLn->old_delay_time_pot_value = 0;

   // dlyLn->length_flag = 0;	//debug

}

float DelayLine_Update(delayline_t *dlyLn, float inp) {

	// Get current delay line output
	float out = *(dlyLn->memory_bank_one + dlyLn->index);

    // Store current input in delay line circular buffer
	*(dlyLn->memory_bank_one + dlyLn->index) = inp;
	*(dlyLn->memory_bank_two + dlyLn->index) = 0.0f;	//clear the backup buffer as we go



    //switch case for cross fading
	switch (dlyLn->delay_state)
	{
		static uint8_t old_switch = 0;

		case STATE_RUN:
			//output is simply the current sample in buffer, so do nothing but check pot value
			if(abs(dlyLn->delay_time_pot_value - dlyLn->old_delay_time_pot_value) >= 50)
			//if(HAL_GPIO_ReadPin(SWITCH_1_GPIO_Port, SWITCH_1_Pin) != old_switch)
			{
			    dlyLn->length = (uint32_t) (0.001f * dlyLn->delay_time_pot_value * SAMPLE_RATE_HZ);

			    if (dlyLn->length > DELAYLINE_MAXLENGTH) {

			        dlyLn->length = DELAYLINE_MAXLENGTH;

			    }

				dlyLn->old_delay_time_pot_value = dlyLn->delay_time_pot_value;
				//old_switch = HAL_GPIO_ReadPin(SWITCH_1_GPIO_Port, SWITCH_1_Pin);
				dlyLn->delay_state = STATE_SWITCH_BUFFERS;
			}
			break;

		case STATE_SWITCH_BUFFERS:

			float *temp_pointer = dlyLn->memory_bank_one;	//save the address of mem1

			dlyLn->memory_bank_one = dlyLn->memory_bank_two;	//when two become onnnneeee
			dlyLn->memory_bank_two = temp_pointer;

			//DO I NEED TO PUT THE CURRENT SAMPLE IN THE NEW BUFFER!?
		//	*(dlyLn->memory_bank_one + dlyLn->index) = inp;

			dlyLn->delay_state = STATE_FADE;	//move over to cross fading the buffers
			break;

		case STATE_FADE:

			*(dlyLn->memory_bank_one + dlyLn->index) = *(dlyLn->memory_bank_one + dlyLn->index) * dlyLn->fade_multiplier;		//fade in
			*(dlyLn->memory_bank_two + dlyLn->index) = *(dlyLn->memory_bank_two + dlyLn->index) * (1.0f -dlyLn->fade_multiplier); //fade out

			dlyLn->fade_multiplier += dlyLn->fade_step; //add some more to the multiplier

			out = *(dlyLn->memory_bank_one + dlyLn->index) + *(dlyLn->memory_bank_two + dlyLn->index);

		//	out = *(dlyLn->memory_bank_one + dlyLn->index);

			//check if we have done our routine
			if(dlyLn->fade_multiplier > 1.0f)
			{
				dlyLn->fade_multiplier = 0;
				//clear buffer 2
		/*	    for (uint32_t n = 0; n <= DELAYLINE_MAXLENGTH; n++)
			    {
			    	*(dlyLn->memory_bank_two + n) = 0.0f;
			    }
			    */
			    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
				dlyLn->delay_state = STATE_RUN;
			}

			break;

		default:
			dlyLn->delay_state = STATE_RUN;		//state machine is lost, reset to normal state
			break;

	}

    // Increment delay line index
    dlyLn->index++;
    if (dlyLn->index >= dlyLn->length) {

        dlyLn->index = 0;


    }

    // Return current output
    return out;

}

//soon to be obsolete code....
void DelayLine_SetLength(delayline_t *dlyLn, float delayTime_ms, float sampleRate_Hz) {

    dlyLn->length = (uint32_t) (0.001f * delayTime_ms * sampleRate_Hz);

    if (dlyLn->length > DELAYLINE_MAXLENGTH) {

        dlyLn->length = DELAYLINE_MAXLENGTH;

    }

}
