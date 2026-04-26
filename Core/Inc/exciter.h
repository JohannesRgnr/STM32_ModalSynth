/**
 * @file exciter.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 26.04.26
 * @note
 */

#ifndef MODAL_SYNTH_EXCITER_H
#define MODAL_SYNTH_EXCITER_H

#include "help_func.h"

typedef struct
{
    float amp;
    float output;
} noise_t;


void Trigger_Note(line_t *line);
float whiteNoise(noise_t *osc);

#endif //MODAL_SYNTH_EXCITER_H
