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



#define DELAY_BUFF_SIZE         15000
#define INIT_DELAY_L            12000
#define INIT_DELAY_R            12000
#define INIT_FEEDB              0.8f
#define INIT_DELAY_WET			0.4f





void 	Delay_init(void);
void 	DelayFeedback_set(uint8_t val);
void 	DelayWet_set(uint8_t val);
void    Delay_time_set(uint32_t time_L, uint32_t time_R);
void    pingpongDelay_process(float x, float *delayLOut, float *delayROut);



#endif //MODAL_SYNTH_STEREO_DELAY_H
