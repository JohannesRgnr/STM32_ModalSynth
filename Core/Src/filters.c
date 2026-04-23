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
*  @brief A simple lowpass filter, useful to smooth data.
 * @param f
 * @param sample
 * @param alpha alpha = 0: no filtering. alpha close to 1: strong smoothing/filtering
 * @return filtered sample
 */
float smoothingLP(onepoleLP_t *f, const float sample, const float alpha){
    const float output = alpha * f->old_value + (1 - alpha) * sample;
    f->old_value = output;
    return output;
}
