/**
 * @file reverb.h
 * @author Johannes R., code originally from Tom Erbe
 * @brief 
 * @version 
 * @date 01.05.26
 * @note
 */

#ifndef MODAL_SYNTH_REVERB_H
#define MODAL_SYNTH_REVERB_H

#include <stdint.h>

#define AUDIO_BUF 16384
#define DELAY_BUF 25090


void reverb_Init(void);
void reverb_process(float in,float *reverbLout,float *reverbRout); // Tom's code
static float allpass(int32_t location, int32_t *writePos, float input, int32_t delaySamps, float gain); // Tom's code
static void delay_write(int32_t location, int32_t delSize, int32_t *wPos, float input); // Tom's code

static float delay_read_linear(int32_t location, int32_t delSize, int32_t wPos, float time); // Tom's code


#endif //MODAL_SYNTH_REVERB_H
