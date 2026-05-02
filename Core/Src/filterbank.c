/**
 * @file filterbank.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 27.04.26
 * @note
 */

#include "filterbank.h"

#include <math.h>

#include "consts.h"
#include "filters.h"



reson_t reson[BANDS];

/**
 * Linear Crossfade between 2 spectra
 * @param freqRatios1
 * @param freqRatios2
 * @param amps1
 * @param amps2
 * @param s Resulting Spectrum
 */
void xfade_2Spectra(const float *freqRatios1, const float *freqRatios2, const float *amps1, const float *amps2, spectrum_t *s)
{
    for (int i = 0; i < BANDS; ++i)
    {
        s->freqRatios[i]    = (1.0f - s->xfade) * freqRatios1[i] + s->xfade * freqRatios2[i];
        s->amps[i]          = (1.0f - s->xfade) * amps1[i] + s->xfade * amps2[i];
    }
}



void spectrum_init(spectrum_t *s, const float *freqRatios, const float *amps)
{
    s->xfade = 0.0f;
    for (int i = 0; i < BANDS; ++i)
    {
        s->freqRatios[i] = freqRatios[i];
        s->amps[i] = amps[i];
    }
}




// /**
//  * Initialize the filterbank
//  * @param f instance of the filterbank
//  * @param freqRatios partials frequency ratios
//  * @param amps partials amplitudes
//  */
// void filterbank_init(filterbank_t *f, const float *freqRatios, const float *amps)
// {
//     f->freq = 110;
//     f->previousFreq = f->freq;
//     f->decay = 8.0f;
//     f->previousDecay = f->decay;
//
//     filterbank_spectrum(f, freqRatios, amps);
//     filterbank_update(f);
// }


void filterbank_init(filterbank_t *f, spectrum_t *s)
{
    f->freq = 110;
    f->previousFreq = f->freq;
    f->decay = 8.0f;
    f->previousDecay = f->decay;

    filterbank_spectrum(f, s);
    filterbank_update(f);
}


// /**
//  * Load new spectrum (freq ratios and amplitudes) into filterbank
//  * @param f instance of the filterbank
//  * @param freqRatios partials frequency ratios
//  * @param amps partials amplitudes
//  */
// void filterbank_spectrum(filterbank_t *f, const float *freqRatios, const float *amps)
// {
//     for (int i = 0; i < BANDS; i++)
//     {
//         f->band_freqratios[i] = freqRatios[i];
//         f->band_gains[i] = amps[i];
//     }
// }


void filterbank_spectrum(filterbank_t *f, spectrum_t *s)
{
    for (int i = 0; i < BANDS; i++)
    {
        f->band_freqratios[i] = s->freqRatios[i];
        f->band_gains[i] = s->amps[i];
    }
}



/**
 * Update the filters of the filterbank according to new frequency and decay inputs
 * @param f instance of the filterbank
 */
void filterbank_update(filterbank_t *f)
{
    if (f->decay != f->previousDecay)
    {
        for (int i = 0; i < BANDS; i++)
        {
            if ( f->freq * f->band_freqratios[i] < 0.45 * FS) // safe NYQUIST limit
            {
                reson[i].r = 1.0f / expf(f->decay);
            }
            else
            {
                reson[i].r = 0.0f;
            }

        }
    }
    if (f->freq != f->previousFreq)
    {
        for (int i = 0; i < BANDS; i++)
        {
            if ( f->freq * f->band_freqratios[i] < 0.45 * FS) // safe NYQUIST limit
            {
                reson[i].g = freq_to_g(f->freq * f->band_freqratios[i]);
            }
            else
            {
                reson[i].g  = 0.f;
            }
        }
    }

    f->previousFreq = f->freq;
    f->previousDecay = f->decay;
}


/**
 * Process one sample with the filterbank
 * @param f instance of the filterbank
 * @param sample sample to process
 * @return processed sample
 */
float filterbank_process(filterbank_t *f, const float sample)
{
    float sumOuts = 0.0f;

    // sum outputs of resonating filters
    for(int i = 0; i < BANDS; i++)
    {
        sumOuts = sumOuts + f->band_gains[i] * SVF_BP_compute(&reson[i], sample);
    }

    return sumOuts * ONEOVERBANDS;
}