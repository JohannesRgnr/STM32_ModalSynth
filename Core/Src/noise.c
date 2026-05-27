/**
 * @file noise.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 26.04.26
 * @note
 */

#include "../Inc/noise.h"

#include <stdint.h>

#include "filters.h"


smoothingLP_t filterLP;

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


float filteredNoise(noise_t *osc)
{
    static uint32_t seed = 1;
    seed *= 16807;
    osc->output = smoothingLP(&filterLP, osc->amp * ((float)seed * 4.6566129e-010f - 1.f), 0.565); // 4 kHz lowpass
    return osc->output;
}