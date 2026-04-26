/**
 * @file exciter.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 26.04.26
 * @note
 */

#include "../Inc/exciter.h"

#include "consts.h"




void Trigger_Note(line_t *line)
{
    line->val = 0.8f;
    line->dst = 0.0f;

    line->inc = (line->dst - line->val)/(0.1f * FS);
}


/**
 * @brief Fast white noise generator
 * @return random float value between -1 and 1
 * @note from https://www.musicdsp.org/en/latest/Other/273-fast-float-random-numbers.html
 */
float whiteNoise(noise_t *osc)
{
    static uint32_t seed = 1;
    seed *= 16807;
    osc->output = osc->amp * ((float)seed * 4.6566129e-010f - 1.f);
    return osc->output;
}