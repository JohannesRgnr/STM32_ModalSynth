/**
 * @file filters.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 15.09.25
 * @note
 */

#include "../Inc/filters.h"


/**
 * @brief A simple lowpass filter, useful to smooth data.
 * @param f
 * @param sample input sample
 * @param alpha alpha = 0: no filtering. alpha close to 1: strong smoothing/filtering
 * @return filtered sample
 */
float smoothingLP(onepoleLP_t *f, const float sample, const float alpha){
    const float output = alpha * f->old_value + (1 - alpha) * sample;
    f->old_value = output;
    return output;
}

/**
 * @brief a 2-pole resonating bandpass filter
 * @param f
 * @param sample input sample
 * @return filtered sample
 */
float resonBP(reson_t *f, const float sample)
{
    float hp = (sample - 2.0f * f->r * f->s1 - f->g * f->s1 - f->s2) / (1.0f + 2.0f * f->r * f->g + f->g * f->g);
    float bp = f->g * hp + f->s1;
    f->s1 = f->g * hp + bp; // state update in 1st integrator
    float lp = f->g * bp + f->s2;
    f->s2 = f->g * bp + lp; // state update in 2nd integrator
    return bp;
}