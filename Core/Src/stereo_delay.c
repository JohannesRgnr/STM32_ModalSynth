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

static float		delaylineL[DELAY_BUFF_SIZE];
static float		delaylineR[DELAY_BUFF_SIZE];

static int16_t		delaylineInt16L[DELAY_BUFF_SIZE];
static int16_t		delaylineInt16R[DELAY_BUFF_SIZE];


float delay_feedback	= INIT_FEEDB;
float delay_wet			= INIT_DELAY_WET;
uint32_t delay_time		= INIT_DELAY_TIME;

ZDFLP_t lp_L;

uint32_t mask;
uint32_t writePointerL = 0;
uint32_t writePointerR = 0;


void Delay_init(void)
{
	mask = DELAY_BUFF_SIZE - 1;
	writePointerL = 0;
	writePointerR = 0;

	SVF_LP_init(&lp_L); // init LPs in the feedback path at 2kHz with gentle resonance
}

static void Delay_writeBufferL(float value)
{
	*(delaylineL + (++writePointerL & mask)) = value;
}

static void Delay_writeBufferR(float value)
{
	*(delaylineR + (++writePointerR & mask)) = value;
}


static float Delay_readBufferL(uint32_t delay)
{
	uint32_t readPointer = (writePointerL - delay);
	return delaylineL[readPointer & mask];
}

static float Delay_readBufferR(uint32_t delay)
{
	uint32_t readPointer = (writePointerR - delay);
	return delaylineR[readPointer & mask];
}

static void DelayInt16_writeBufferL(float value)
{
	*(delaylineInt16L + (++writePointerL & mask)) = (int16_t)(32767.0f * value);
}

static void DelayInt16_writeBufferR(float value)
{
	*(delaylineInt16R + (++writePointerR & mask)) = (int16_t)(32767.0f * value);
}


static float DelayInt16_readBufferL(uint32_t delay)
{
	uint32_t readPointer = (writePointerL - delay);
	return ONEOVER16BITS * (float)delaylineInt16L[readPointer & mask];
}

static float DelayInt16_readBufferR(uint32_t delay)
{
	uint32_t readPointer = (writePointerR - delay);
	return ONEOVER16BITS * (float)delaylineInt16R[readPointer & mask];
}


/**
 * Ping pong delay effect, with crossfeedback, softclip and lowpass filtering
 * @note requires 2 delay lines (L and R)
 *
 * @param inputSample
 * @param delay
 * @param delayLOut
 * @param delayROut
 */
void Delay_process(float inputSample, uint32_t delay, float *delayLOut, float *delayROut)
{
	/***************************** read from delay line *******************************/
	// Without interpolation.. delay time truncated to integer samples
	float delayedSampleL = SoftClip(SVF_LP_compute(&lp_L,Delay_readBufferL(delay)));
	float delayedSampleR = SoftClip(Delay_readBufferR(delay));

	/***************************** dry/wet mix and output *****************************/
	float outputSampleL = Crossfade(inputSample, delayedSampleL, delay_wet);
	float outputSampleR = Crossfade(inputSample, delayedSampleR, delay_wet);

	*delayLOut = outputSampleL;
	*delayROut = outputSampleR;

	/**************************** write into delay line *******************************/
	Delay_writeBufferL(inputSample + delay_feedback * delayedSampleR);
	Delay_writeBufferR(delay_feedback * delayedSampleL);
}

/**
 * Ping pong delay effect, with crossfeedback, softclip and lowpass filtering
 * @note requires 2 * int16_t delay lines (L and R)
 * @param inputSample
 * @param delay
 * @param delayLOut
 * @param delayROut
 */
void DelayInt16_process(float inputSample, uint32_t delay, float *delayLOut, float *delayROut)
{
	/***************************** read from delay line *******************************/
	// Without interpolation.. delay time truncated to integer samples
	float delayedSampleL = SoftClip(SVF_LP_compute(&lp_L,DelayInt16_readBufferL(delay)));
	float delayedSampleR = SoftClip(DelayInt16_readBufferR(delay));

	/***************************** dry/wet mix and output *****************************/
	float outputSampleL = Crossfade(inputSample, delayedSampleL, delay_wet);
	float outputSampleR = Crossfade(inputSample, delayedSampleR, delay_wet);

	*delayLOut = outputSampleL;
	*delayROut = outputSampleR;

	/**************************** write into delay line *******************************/
	DelayInt16_writeBufferL(inputSample + delay_feedback * delayedSampleR);
	DelayInt16_writeBufferR(delay_feedback * delayedSampleL);
}



void DelayFeedback_set(uint8_t val)
{
	delay_feedback = val;
}


void DelayWet_set(uint8_t val)
{
	delay_wet = val;
}

