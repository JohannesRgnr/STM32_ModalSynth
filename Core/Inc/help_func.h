/**
 * @file help_func.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 11.09.25
 * @note
 */

#ifndef MODAL_SYNTH_HELP_FUNC_H
#define MODAL_SYNTH_HELP_FUNC_H


#include <math.h>
#include <stdint.h>




#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


// convenient DEFINE from Mutable Instruments
#define GET_INTEGRAL_FRACTIONAL(x)\
int32_t x ## _integral = (int32_t)(x); \
float x ## _fractional = x - (float)(x ## _integral);




typedef struct
{
    float val;		// current value
    float dst;		// value to move to
    float inc;		// increment per sample to get from val to dst
} line_t;



/**
 * @brief Keep value between 0 and max
 *
 * @param value
 * @param max
 * @return float
 */
static inline float wrap(float value, const float max)
{
    if (value < 0.f)
        value += max;
    if (value >= max)
        value -= max;

    return value;
}

static inline float Crossfade(const float a, const float b, const float fade)
{
    return a + (b - a) * fade;
}


/**
 * @brief Pade-approximation of tanh for soft clipping
 * @from https://www.musicdsp.org/en/latest/Other/238-rational-tanh-approximation.html
 * @param x float value to soft-clip
 * @return float
 */
static inline float SoftClip(float x)
{
    if (x < -3.0f)
    {
        return -1.0f;
    }
    if (x > 3.0f)
    {
        return 1.0f;
    }

    return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}


inline float clip(float input, float lower, float upper);


float lutLerp(const float *table, uint16_t table_size, float index); // linear interpolation within a given lookup table


static float mtof(float note) // midi note to frequency (Hz)
{
    const float frequency = 440.0f * expf(0.057762265f * (note - 69.0f));
    return frequency;
}


float scale(float lower_input, float upper_input, float lower_output, float upper_output, float value);




#endif //MODAL_SYNTH_HELP_FUNC_H