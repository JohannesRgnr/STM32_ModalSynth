/**
 * @file filterbank.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 27.04.26
 * @note
 */

#ifndef MODAL_SYNTH_FILTERBANK_H
#define MODAL_SYNTH_FILTERBANK_H


#define BANDS 16


typedef struct
{
    float freq;
    float previousFreq;
    float decay;
    float previousDecay;
    float spectra[BANDS];
    float bp[BANDS];
}	filterbank_t;


void filterbank_init(filterbank_t *f, const float *freqRatios, const float *amps);

void filterbank_update(filterbank_t *f, const float *freqRatios, const float *amps);

#endif //MODAL_SYNTH_FILTERBANK_H
