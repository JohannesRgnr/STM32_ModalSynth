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
#include "noise.h"
#include "reverb.h"
#include "spectra.h"
#include "stereo_delay.h"

/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t codecBuffer[BUFFER_SIZE]; // x samples X 2 channels (interleaved)

noise_t noise;
line_t exciterAmp, freq;
spectrum_t spectrum;
filterbank_t filterbank;

extern float delay_feedback;
extern float delay_wet;
float reverb_amount;
float reverbsend;
extern float reverb_feedback;

float delayLOut = 0 ;		// left output of ping pong delay
float delayROut = 0;		// right output of ping pong delay

float reverbLout, reverbRout;

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
    spectrum_init(&spectrum, SquarePartials, Bell2Partials, SquareAmp, ExpAmp);
    spectrum_xfade(&spectrum, 0.5f);

    filterbank_init(&filterbank, &spectrum);
    freq.val = freq.dst = 0.f;
    Delay_init();
    reverb_Init();
}

 void audioBlock(int16_t *output, const int32_t samples)
{
    freq.inc = (freq.dst - freq.val) * (float)samples * TS;
    float samp, sampleL, sampleR;

    delay_wet = 0.5f;
    delay_feedback = 0.6f;
    reverb_amount = 0.7f;
    reverb_feedback = 0.9f;

    for (int i = 0; i < samples; i++)
    {
        // Exciter
        samp = whiteNoise(&noise);
        exciterAmp.val += exciterAmp.inc;
        if(exciterAmp.inc < 0.0f && exciterAmp.val < exciterAmp.dst)
            exciterAmp.val = exciterAmp.dst;
        if(exciterAmp.inc > 0.0f && exciterAmp.val > exciterAmp.dst)
            exciterAmp.val = exciterAmp.dst;

        // generate noise burst
        samp = samp * exciterAmp.val * exciterAmp.val;

        // going through filterbank
        filterbank.freq = freq.val;
        samp = filterbank_process(&filterbank, samp);

        // Apply delay effect
        pingpongDelay_process(samp, &delayLOut, &delayROut);

        // Send to Reverb
        reverbsend = reverb_amount * (delayLOut*0.707f + delayROut*0.707f); // send to reverb
        reverb_process(reverbsend, &reverbLout, &reverbRout);

        // Soft Clip the outputs
        sampleL = SoftClip(delayLOut + reverbLout);
        sampleR = SoftClip(delayROut + reverbRout);

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