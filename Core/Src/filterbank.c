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
 * Initialize the filterbank
 * @param f instance of the filterbank
 * @param freqRatios partials frequency ratios
 * @param amps partials amplitudes
 */
void filterbank_init(filterbank_t *f, const float *freqRatios, const float *amps)
{
    f->freq = 110;
    f->previousFreq = f->freq;
    f->decay = 8.0f;
    f->previousDecay = f->decay;

    filterbank_newspectra(f, freqRatios, amps);
    filterbank_update(f);
}


/**
 * Update the filterbank parameters according to the new spectra specifications (freq ratios and amplitudes)
 * @param f instance of the filterbank
 * @param freqRatios partials frequency ratios
 * @param amps partials amplitudes
 */
void filterbank_newspectra(filterbank_t *f, const float *freqRatios, const float *amps)
{
    for (int i = 0; i < BANDS; i++)
    {
        f->band_freqratios[i] = freqRatios[i];
        f->band_gains[i] = amps[i];
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
            reson[i].r = 1.0f / expf(f->decay);
        }
    }
    if (f->freq != f->previousFreq)
    {
        for (int i = 0; i < BANDS; i++)
        {
            if ( f->freq * f->band_freqratios[i] < 0.4 * FS) // safe NYQUIST limit
            {
                reson[i].wd = TWOPI * f->freq * f->band_freqratios[i];
                reson[i].wa = 88200.0f * tanf(reson[i].wd * TS * 0.5f);
                reson[i].g  = reson[i].wa * TS * 0.5f;
            }
            else
            {
                reson[i].wd = 0.f;
                reson[i].wa = 0.f;
                reson[i].g  = 0.f;
            }
        }
    }

    f->previousFreq = f->freq;
    f->previousDecay = f->decay;
}

float filterbank_process(filterbank_t *f, const float sample)
{
    float SumOuts = 0.0f;
    for(int i = 0; i < BANDS; i++)
    {
        SumOuts = SumOuts + f->band_gains[i] * resonBP(&reson[i], sample) * ONEOVERBANDS;
    }

    return SumOuts;
}