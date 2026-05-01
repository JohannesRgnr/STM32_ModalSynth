/**
 * @file stereo_delay.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 01.05.26
 * @note
 */

#include "../Inc/stereo_delay.h"
#include "filters.h"
#include "help_func.h"

static float		delaylineL[DELAY_BUFF_SIZE + 2];
static float		delaylineR[DELAY_BUFF_SIZE + 2];
static float		*readptrL , *readptrR ;
static float 		*writeptrL , *writeptrR ;
static uint16_t		delay_time_L , delay_time_R;


float		feedback  = INIT_FEEDB;
float		delay_wet  = INIT_DELAY_WET;

ZDFLP_t lp_L;
ZDFLP_t lp_R;





void Delay_init(void)
{
	readptrL = delaylineL;
	readptrR = delaylineR ;
	writeptrL = delaylineL + INIT_DELAY_L;
	writeptrR = delaylineR + INIT_DELAY_R;

	SVF_LP_init(&lp_L);
	SVF_LP_init(&lp_R);
}


void Delay_time_set(uint32_t time_L, uint32_t time_R)
{
	float 	*posL, *posR;
	delay_time_L = time_L;
	delay_time_R = time_R;
	posL = writeptrL - delay_time_L;
	if (posL >= delaylineL)
		readptrL = posL;
	else
		readptrL = posL + DELAY_BUFF_SIZE - 1;
	posR = writeptrR - delay_time_R;
	if (posR >= delaylineR)
		readptrR = posR;
	else
		readptrR = posR + DELAY_BUFF_SIZE - 1;

}


void DelayFeedback_set(uint8_t val)
{
	feedback = val;
}


void DelayWet_set(uint8_t val)
{
	delay_wet = val;
}


/**
 * @brief Ping pong delay effect, with crossfeedback, softclip and lowpass filtering
 * @note requires 2 delay lines (L and R)
 *
 * @param x
 * @param delayLOut
 * @param delayROut
 */
void pingpongDelay_process (float input_sample, float * delayLOut, float * delayROut)
{
	float delayed_sampleL, delayed_sampleR, sampleL, sampleR;

	// read first so that we can have feedback, apply lowpass filtering
	delayed_sampleL = SVF_LP_compute(&lp_L, *readptrL);
	delayed_sampleR = SVF_LP_compute(&lp_R, *readptrR);

	// apply soft clipping
	sampleL = SoftClip(feedback * delayed_sampleL);
	sampleR = SoftClip(input_sample + feedback * delayed_sampleR);

	// write then update pointers
	*writeptrL = sampleR;
	*writeptrR = sampleL;
	writeptrL++;
	readptrL++;
	writeptrR++;
	readptrR++;

	if ((writeptrL - delaylineL) >= DELAY_BUFF_SIZE)
		writeptrL = delaylineL;

	if ((readptrL - delaylineL) >= DELAY_BUFF_SIZE)
		readptrL = delaylineL;

	if ((writeptrR - delaylineR) >= DELAY_BUFF_SIZE)
		writeptrR = delaylineR;

	if ((readptrR - delaylineR) >= DELAY_BUFF_SIZE)
		readptrR = delaylineR;

	// linear crossfade
	*delayLOut = (delay_wet * sampleL + (1 - delay_wet) * input_sample);
	*delayROut = (delay_wet * sampleR + (1 - delay_wet) * input_sample);
}