/**
 * @file spectrum_morph.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 02.05.26
 * @note
 */

#ifndef MODAL_SYNTH_SPECTRUM_MORPH_H
#define MODAL_SYNTH_SPECTRUM_MORPH_H
#include "filterbank.h"


typedef struct
{
    float amps[BANDS];
    float freqRatios[BANDS];
    float xfade;
}	spectrum_t;




void xfade_2Spectra(const float *freqRatios1, const float *freqRatios2, const float *amps1, const float *amps2, spectrum_t *spectrum);



#endif //MODAL_SYNTH_SPECTRUM_MORPH_H
