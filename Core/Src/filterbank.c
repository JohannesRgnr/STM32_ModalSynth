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



void filterbank_init(filterbank_t *f, const float *freqRatios, const float *amps)
{
    f->freq = 110;
    f->previousFreq = f->freq;
    f->decay = 8.0f;
    f->previousDecay = f->decay;
    for (int i = 0; i < BANDS; i++)
    {
        reson[i].r = 1.0f / (expf(f->decay));
        reson[i].wd = TWOPI * f->freq * freqRatios[i];
        reson[i].wa = 2 * FS * tanf(reson[i].wd * TS * 0.5f);
        reson[i].g  = reson[i].wa * TS * 0.5f;
    }
}




void filterbank_update(filterbank_t *f, const float *freqRatios, const float *amps)
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
            if ( f->freq * freqRatios[i] < 0.4 * FS) // safe NYQUIST limit
            {
                reson[i].wd = TWOPI * f->freq * freqRatios[i];
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

    for (int i = 0; i < BANDS; i++)
    {
        f->gain[i] = amps[i];
    }


    f->previousFreq = f->freq;
    f->previousDecay = f->decay;
}


float filterbank_process(filterbank_t *f, const float sample)
{
    float SumOuts = 0.0f;
    for(int i = 0; i < BANDS; i++)
    {
        SumOuts = SumOuts + f->gain[i] * resonBP(&reson[i], sample) * ONEOVERBANDS;
    }

    return SumOuts;
}