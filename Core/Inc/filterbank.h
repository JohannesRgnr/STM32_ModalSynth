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
#define ONEOVERBANDS (1.f/BANDS)


typedef struct
{
    float freq;
    float previousFreq;
    float decay;
    float previousDecay;
    float band_gains[BANDS];
    float band_freqratios[BANDS];
    float bp[BANDS];
}	filterbank_t;

/**
 * Spectrum = linear crossfade between 2 spectra
 */
typedef struct
{
    float freqRatios1[BANDS];
    float freqRatios2[BANDS];
    float amps1[BANDS];
    float amps2[BANDS];
    float amps[BANDS];
    float freqRatios[BANDS];
    float xfade;
}	spectrum_t;

void spectrum_init(spectrum_t *s, const float *freqRatios1, const float *freqRatios2, const float *amps1, const float *amps2);
void spectrum_xfade(spectrum_t *s, float xfade);

void filterbank_init(filterbank_t *f, const spectrum_t *s);
void filterbank_spectrum(filterbank_t *f, const spectrum_t *s);
void filterbank_update(filterbank_t *f);
float filterbank_process(const filterbank_t *f, float sample);

#endif //MODAL_SYNTH_FILTERBANK_H
