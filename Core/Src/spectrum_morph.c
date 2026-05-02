/**
 * @file spectrum_morph.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 02.05.26
 * @note
 */

#include "../Inc/spectrum_morph.h"


/**
 * Linear Crossfade between 2 spectra
 * @param freqRatios1
 * @param freqRatios2
 * @param amps1
 * @param amps2
 * @param spectrum Resulting Spectrum
 * @param xfade Crossfade value between 0. and 1.
 */
void xfade_2Spectra(const float *freqRatios1, const float *freqRatios2, const float *amps1, const float *amps2, spectrum_t *spectrum)
{
    for (int i = 0; i < BANDS; ++i)
    {
        spectrum->freqRatios[i] = (1.0f - spectrum->xfade) * freqRatios1[i] + spectrum->xfade * freqRatios2[i];
        spectrum->amps[i] = (1.0f - spectrum->xfade) * amps1[i] + spectrum->xfade * amps2[i];
    }
}
