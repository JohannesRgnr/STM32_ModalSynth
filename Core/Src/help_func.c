/**
 * @file help_func.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 11.09.25
 * @note
 */

#include "help_func.h"



/**
 * @brief Constrain input value between lower and upper limits
 *
 * @param input
 * @param lower
 * @param upper
 * @return
 */
float clip(float input, float lower, float upper) {
    return max(lower, min(input, upper));
}


/**
 * @brief Linear interpolation within a given lookup table. Useful for wavetable oscillators.
 *
 * @param index
 * @param table_size
 * @param table
 * @return float
 */
float lutLerp(const float *table, uint16_t table_size, float index){
    GET_INTEGRAL_FRACTIONAL(index); // get integral and fractional parts
    uint16_t mask = table_size - 1;
    float x0 = *(table + (index_integral & mask));
    float x1 = *(table + ((index_integral + 1) & mask));

    // while (index_integral > table_size)
    //     index_integral = index_integral - table_size;

    // get the interpolated output
   // return table[index_integral] + (table[index_integral + 1] - table[index_integral]) * index_fractional;
    return x0 + (x1 - x0) * index_fractional;
}


/**
 * Scale the input value from an input range to an output range
 * @param lower_input
 * @param upper_input
 * @param lower_output
 * @param upper_output
 * @param value scaled value
 * @return
 */
float scale(const float lower_input, const float upper_input, const float lower_output, const float upper_output, const float value)
{
    float slope = (upper_output - lower_output) / (upper_input - lower_input);

    return lower_output + slope * (value - lower_input);
}