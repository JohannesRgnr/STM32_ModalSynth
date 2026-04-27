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
// #include "spectra.h"


reson_t reson[BANDS];

void Update_Filterbank(filterbank_t *f, const float *freqRatios, const float *amps)
{
    int i;
    if (f->decay != f->previousDecay)
    {
        for (i = 0; i < BANDS; i++)
        {
            reson[i].r = 1.0f / (expf(f->decay));
        }
    }
    if (f->freq != f->previousFreq)
    {
        for (i = 0; i<16; i++)
        {
            reson[i].wd = TWOPI * f->freq * freqRatios[i];
            reson[i].wa = 88200.0f * tanf(reson[i].wd * TS * 0.5f);
            reson[i].g  = reson[i].wa * TS * 0.5f;
        }
    }


    f->previousFreq = f->freq;
    f->previousDecay = f->decay;
}
