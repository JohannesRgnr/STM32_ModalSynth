/**
 * @file multiLFO.c
 * @author Johannes R.
 * @brief A multi-phase sinusoidal oscillator
 * @version 
 * @date 04.05.26
 * @note
 */

#include "multiLFO.h"
#include "help_func.h"
#include "consts.h"
#include "LUTs.h"


void multiLFO_init(lfo_t *lfo, float amp, float freq)
{
    lfo->amp = amp;
    lfo->freq = freq;
    lfo->phase = 0;

    for (int i = 0; i < BANDS; ++i)
    {
        lfo->output[i] = 0;
    }
}

/**
 * Generate n=BANDS sine waves, driven by the same phasor
 * @param lfo
 */
void multiLFO_process(lfo_t *lfo)
{
    lfo->phase = wrap(lfo->phase, 1.0f);

    for (int i = 0; i < BANDS; ++i)
    {
        float phase = lfo->phase + 0.12 * i;
        phase = wrap(phase, 1.0f);
        float sine = lutLerp(lut_sine, LUT_SINE_SIZE,LUT_SINE_SIZE * phase); // linear-interpolated sinewave

        lfo->output[i] = 1.0f - lfo->amp * (sine * 0.5f + 0.5f);

    }
    lfo->phase += TS * lfo->freq; // increment phase (phase normalized from 0 to 1)

}