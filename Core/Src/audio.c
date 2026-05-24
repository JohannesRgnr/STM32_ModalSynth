/**
 * @file audio.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 08.09.25
 * @note
 */



#include<stdint.h>
#include "audio.h"


#include "consts.h"
#include "filterbank.h"
#include "help_func.h"
#include "multiLFO.h"
#include "noise.h"
#include "reverb.h"
#include "spectra.h"
#include "stereo_delay.h"


#define DELAY_FX 1
#define REVERB_FX 1

/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t codecBuffer[BUFFER_SIZE]; // x samples X 2 channels (interleaved)

noise_t noise;
line_t exciterAmp, freq;
spectrum_t spectrum;
filterbank_t filterbank;
lfo_t lfo;

extern float delay_feedback;
extern float delay_wet;
float reverb_amount;
extern float reverb_feedback;
extern uint8_t LFO_btn, delay_btn, reverb_btn;

float delayLOut = 0 ;		// left output of ping pong delay
float delayROut = 0;		// right output of ping pong delay

float reverbIn, reverbLout, reverbRout;

float lfo_speed = 0.1f;
float lfo_amp = 0.5f;
float lfo_phaseShift = 0.1f;

/**
 * @brief Init audio
 *
 */
void AUDIO_Init()
{
    // initialize board audio device
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, INITIAL_VOLUME, BSP_AUDIO_FREQUENCY_44K);
    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02); // slots 0 and 2 activated for headphones out
    BSP_AUDIO_OUT_Play((uint16_t *)codecBuffer, BUFFER_SIZE * 2);

    // initialize audio objects
    noise.amp = 0.5f;
    spectrum_init(&spectrum, Bell1Partials, SawPartials, ExpAmp, SawAmp);
    spectrum_xfade(&spectrum, 0.0f);

    filterbank_init(&filterbank, &spectrum);
    multiLFO_init(&lfo, lfo_amp, lfo_speed);
    freq.val = freq.dst = 0.f;

#if DELAY_FX == 1
    Delay_init();
#endif

#if REVERB_FX == 1
    reverb_Init();
#endif
}

 void audioBlock(int16_t *output, const int32_t samples)
{
    freq.inc = (freq.dst - freq.val) * (float)samples * TS;
    // freq.inc = 0.f;
    // delay_wet = delay_btn * 0.5f;
    delay_wet = 0.25f;
    delay_feedback = 0.6f;
    // reverb_amount = reverb_btn * 0.7f;
    reverb_feedback = 0.6f;
    reverb_amount = 0.4f;

    lfo.freq = lfo_speed;
    lfo.amp = lfo_amp;
    lfo.phaseShift = lfo_phaseShift;

    for (int i = 0; i < samples; i++)
    {
        // Exciter
        float samp = filteredNoise(&noise);
        exciterAmp.val += exciterAmp.inc;
        if(exciterAmp.inc < 0.0f && exciterAmp.val < exciterAmp.dst)
            exciterAmp.val = exciterAmp.dst;
        if(exciterAmp.inc > 0.0f && exciterAmp.val > exciterAmp.dst)
            exciterAmp.val = exciterAmp.dst;

        // Process one sample of the multi LFO
        multiLFO_process(&lfo);

        // generate noise burst
       samp = samp * exciterAmp.val * exciterAmp.val;

        // going through filterbank
        filterbank.freq = freq.val;
        samp = filterbank_process(&filterbank, samp);


#if DELAY_FX == 1
        // Apply delay effect
        DelayInt16_process(samp, 15000, &delayLOut, &delayROut);

#else

        delayLOut = 0.5*samp;
        delayROut = 0.5*samp;

#endif

#if REVERB_FX == 1

        // Send to Reverb
        reverbIn = reverb_amount * (delayLOut+ delayROut);
        reverb_process(reverbIn, &reverbLout, &reverbRout);

        float sampleL = delayLOut + reverbLout;
        float sampleR = delayROut + reverbRout;

#else

        float sampleL = delayLOut;
        float sampleR = delayROut;

#endif


        sampleL = SoftClip(sampleL);
        sampleR = SoftClip(sampleR);

        // float to int16 conversion
        const int16_t sampleLOut = (int16_t)(32767.0f * sampleL);
        const int16_t sampleROut = (int16_t)(32767.0f * sampleR);

        // output to circular buffer
        output[i << 1] = sampleLOut;
        output[(i << 1) + 1]  = sampleROut;

        // increment smoothed values (lines)
        freq.val += freq.inc;
    }
}


void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
    audioBlock(codecBuffer, BUFFER_SIZE_DIV_4);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
    audioBlock(codecBuffer + BUFFER_SIZE_DIV_2, BUFFER_SIZE_DIV_4);
}

void BSP_AUDIO_OUT_Error_CallBack(void)
{
    /* Stop the program with an infinite loop */
    while (1)
    {
    }
}

void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}