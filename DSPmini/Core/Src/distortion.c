/*
 *  distortion.c
 *
 *  Created on: Feb 04, 2025
 *  Author: chase callender
 *
 *  overdrive and distortion functions
 */

#include "distortion.h"
#include <math.h>
#include "main.h"

void innit_distortion(distortion_t *effect)
{
	effect->threshold = 0.33f;
	effect->gain = 1.0f;
	effect->asym_Q = -0.5;
	effect->asym_d = 8.0f;
}


float distortion(distortion_t *effect, float input)
{
	float clipIn = (effect->gain * input);// / 13107.2;	//converts to volts
	float absClipIn = fabs(clipIn);
	float signClipIn = (clipIn >= 0.0f) ? 1.0f : -1.0f;

	float clipOut = 0.0f;

	if(absClipIn < (effect->threshold))
	{
		clipOut = 2.0f * absClipIn;
	}

	else if(absClipIn >= (effect->threshold) && (absClipIn <= (2 * effect->threshold)))
	{
		clipOut = (3.0f - ((2.0f - (3.0f * absClipIn)) * (2.0f - (3.0f * absClipIn)))) / 3.0f;
	//	clipOut = (-3.0 * powf(absClipIn,2)) + (4.0 * absClipIn) -  (1.f/3.f);

		//clipOut = 2.0f * clipIn;
	}
	else
	{
		clipOut = 1.0;
		//clipOut = 2.0f * clipIn;
	}

	return (clipOut * signClipIn);// * 13107.2;	//put back the sign and convert to bits
	//return clipIn;
}


float asym_distortion(distortion_t *effect, float input)
{
	//function largely copied from youtube. Can make better
	float clipOut;
	float xGain = input;	//
	static const float d = 10.0f;
	//float xGain = (input) / (float)0x7FFF;
	//static const float q = -1.1f;

	xGain = xGain * effect->gain;

/*	if (xGain < -1.0f) {
		xGain = -1.0f;
	} else if (xGain > 1.0f) {
		xGain =  1.0f;
	}
*/
	clipOut = effect->asym_Q / (1.0f - expf(effect->asym_d * effect->asym_Q));
	//clipOut = q / (1.0f - expf(d * q));

//	if(fabs(xGain - effect->asym_Q) >= 0.00001f)	//catches divide by zero error. Should make this better
//	{
	clipOut += (xGain - effect->asym_Q) / (1.0f - expf(-effect->asym_d * (xGain - effect->asym_Q)));
	//clipOut += (xGain - q) / (1.0f - expf(-d * (xGain - q)));

//	}


/*
	if(xGain <= effect->asym_Q)
	{
		clipOut = effect->asym_Q;
	}

	else if(xGain >= -effect->asym_Q)
	{
	//	clipOut = -effect->asym_Q;
	}

	else
	{
		clipOut = xGain;
	}

*/
	return clipOut;// * 13107.2;	//convert to bits
//	return clipOut * (float)0x7FFF;
}

