/**
 * @file noise.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 26.04.26
 * @note
 */

#ifndef MODAL_SYNTH_NOISE_H
#define MODAL_SYNTH_NOISE_H

typedef struct
{
    float amp;
    float output;
} noise_t;

float whiteNoise(noise_t *osc);

#endif //MODAL_SYNTH_NOISE_H
