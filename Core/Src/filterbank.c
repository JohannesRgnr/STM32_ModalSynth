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
#include "help_func.h"
#include "multiLFO.h"

extern lfo_t lfo;

reson_t reson[BANDS];


/**
 * Initialize a spectrum to a set of frequency ratios and amplitudes
 * @param s
 * @param freqRatios1
 * @param freqRatios2
 * @param amps1
 * @param amps2
 */
void spectrum_init(spectrum_t *s, const float *freqRatios1, const float *freqRatios2, const float *amps1, const float *amps2)
{
    s->xfade = 0.0f;

    for (int i = 0; i < BANDS; ++i)
    {
        s->freqRatios1[i] = freqRatios1[i];
        s->amps1[i] = amps1[i];
        s->freqRatios2[i] = freqRatios2[i];
        s->amps2[i] = amps2[i];
    }

    spectrum_xfade(s, s->xfade);
}


/**
 * Linear Crossfade between 2 spectra
 * @param s Resulting Spectrum
 * @param xfade crossfade value between 0. and 1.
 */
void spectrum_xfade(spectrum_t *s, float xfade)
{
    s->xfade = xfade;

    for (int i = 0; i < BANDS; ++i)
    {
        s->freqRatios[i]    = Crossfade(s->freqRatios1[i], s->freqRatios2[i], s->xfade);
        s->amps[i]          = Crossfade(s->amps1[i], s->amps2[i], s->xfade);
    }
}


/**
 * Load a spectrum into the left or right slot
 * @param s
 * @param freqRatios
 * @param amps
 * @param leftOrRight
 */
void spectrum_load(spectrum_t *s, const float *freqRatios, const float *amps, uint8_t leftOrRight)
{
    if (leftOrRight == LEFT_SPECTRUM)
    {
        for (int i = 0; i < BANDS; i++)
        {
            s->freqRatios1[i] = freqRatios[i];
            s->amps1[i] = amps[i];
        }
    }
    else if (leftOrRight == RIGHT_SPECTRUM)
    {
        for (int i = 0; i < BANDS; i++)
        {
            s->freqRatios2[i] = freqRatios[i];
            s->amps2[i] = amps[i];
        }
    }

    // update resulting spectrum
    for (int i = 0; i < BANDS; ++i)
    {
        s->freqRatios[i]    = Crossfade(s->freqRatios1[i], s->freqRatios2[i], s->xfade);
        s->amps[i]          = Crossfade(s->amps1[i], s->amps2[i], s->xfade);
    }
}



/**
 * Initialize the filterbank
 * @param f instance of the filterbank
 * @param s instance of the spectrum
 */
void filterbank_init(filterbank_t *f, const spectrum_t *s)
{
    f->freq = 110;
    f->previousFreq = f->freq;
    f->decay = 8.0f;
    f->previousDecay = f->decay;

    filterbank_spectrum(f, s);
    filterbank_update(f);
}


/**
 * Load a spectrum into the filterbank
 * @param f instance of the filterbank
 * @param s instance of the spectrum
 */
void filterbank_spectrum(filterbank_t *f, const spectrum_t *s)
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
float filterbank_process(const filterbank_t *f, const float sample)
{
    float sumOuts = 0.0f;

    // sum outputs of resonating filters
    for(int i = 0; i < BANDS; i++)
    {
        sumOuts = sumOuts + lfo.output[i] * f->band_gains[i] * SVF_BP_compute(&reson[i], sample);
    }

    return sumOuts * ONEOVERBANDS;
}