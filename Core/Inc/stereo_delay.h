/**
 * @file stereo_delay.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 01.05.26
 * @note
 */

#ifndef MODAL_SYNTH_STEREO_DELAY_H
#define MODAL_SYNTH_STEREO_DELAY_H



#include <stdint.h>



#define DELAY_BUFF_SIZE         32768                   // has to be smaller when using the float buffers
#define INIT_DELAY_L            16384
#define INIT_DELAY_R            16384
#define INIT_DELAY_TIME         16384
#define INIT_FEEDB              0.5f
#define INIT_DELAY_WET			0.5f
#define ONEOVER16BITS           (1.0f / 32767.0f)





void 	DelayFeedback_set(uint8_t val);
void 	DelayWet_set(uint8_t val);



void Delay_init(void);
void Delay_process(float x, uint32_t delay, float *delayLOut, float *delayROut);
void DelayInt16_process(float x, uint32_t delay, float *delayLOut, float *delayROut);

static void Delay_writeBufferL(float value);
static void Delay_writeBufferR(float value);
static float Delay_readBufferL(uint32_t delay);
static float Delay_readBufferR(uint32_t delay);

static void DelayInt16_writeBufferL(float value);
static void DelayInt16_writeBufferR(float value);
static float DelayInt16_readBufferL(uint32_t delay);
static float DelayInt16_readBufferR(uint32_t delay);


#endif //MODAL_SYNTH_STEREO_DELAY_H
