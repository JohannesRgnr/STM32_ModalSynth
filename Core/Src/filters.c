/**
 * @file filters.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 15.09.25
 * @note
 */

#include "../Inc/filters.h"

#include <math.h>

#include "CONSTS.h"


/**
 * @brief A simple lowpass filter, useful to smooth data. Specified via coefficient alpha.
 * @param f
 * @param sample input sample
 * @param alpha Filter coeff. alpha = 0: no filtering. alpha close to 1: strong smoothing/filtering
 * @return filtered sample
 */
float smoothingLP(smoothingLP_t *f, const float sample, const float alpha){
    const float output = alpha * f->old_value + (1 - alpha) * sample;
    f->old_value = output;
    return output;
}

/**
 * @brief A simple lowpass filter. Specified via cutoff frequency in Hz.
 * @param f
 * @param sample input sample
 * @param alpha alpha = 0: no filtering. alpha close to 1: strong smoothing/filtering
 * @return filtered sample
 */
float onepoleLP(onepoleLP_t *f, const float sample, const float cutoff){
    float c = exp(-TWOPI * cutoff * TS);
    const float output = c * f->old_value + (1 - c) * sample;
    f->old_value = output;
    return output;
}

/**
 * @brief 2-pole resonant state variable filter, in bandpass mode
 * @param f
 * @param sample input sample
 * @return filtered sample
 */
float SVF_BP_compute(reson_t *f, const float sample)
{
    const float hp = (sample - 2.0f * f->r * f->s1 - f->g * f->s1 - f->s2) / (1.0f + 2.0f * f->r * f->g + f->g * f->g);
    const float bp = f->g * hp + f->s1;
    f->s1 = f->g * hp + bp; // state update in 1st integrator
    f->s2 += 2 * f->g * bp; // state update in 2nd integrator
    return bp;
}



void SVF_LP_init(ZDFLP_t * filter){
    filter->s1 = 0.0f;
    filter->s2 = 0.0f;
    filter->r = 0.5f;           // slight resonance
    filter->cutoff = 2000.f;
}


/**
 * @brief calculate g coefficient for SVF
 *
 * @param freq_hz cutoff frequency in Hz
 * @return float
 */
float freq_to_g(float freq_hz)
{
    return tanf(PI * freq_hz * TS);
}


/**
 * @brief 2-pole resonant state variable filter, in lowpass mode
 * @param f
 * @param sample
 * @return float
 */
float SVF_LP_compute(ZDFLP_t *f, float sample)
{
    float bp, lp, hp;
    f->g = freq_to_g(f->cutoff);
    hp = (sample - 2.0f * f->r * f->s1 - f->g * f->s1 - f->s2) / (1.0f + 2.0f * f->r * f->g + f->g * f->g);
    bp = f->g * hp + f->s1;
    f->s1 = f->g * hp + bp; // state in 1st trapezoidal integrator
    lp = f->g * bp + f->s2;
    f->s2 = f->g * bp + lp; // state in 2nd trapezoidal integrator
    return lp;
}