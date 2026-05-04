/**
 * @file multiLFO.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 04.05.26
 * @note
 */

#ifndef MODAL_SYNTH_MULTILFO_H
#define MODAL_SYNTH_MULTILFO_H
#include "filterbank.h"


typedef struct
{
    float amp;
    float freq;
    float phase;
    float output[BANDS];
} lfo_t;


void multiLFO_init(lfo_t *lfo, float amp, float freq);
void multiLFO_process(lfo_t *lfo);



#endif //MODAL_SYNTH_MULTILFO_H
