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
#include "spectra.h"

/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t codecBuffer[BUFFER_SIZE]; // x samples X 2 channels (interleaved)

noise_t noise;
line_t exciterAmp, freq;
filterbank_t filterbank;

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
    filterbank_init(&filterbank, Bell1Partials, ExpAmp);
    freq.val = freq.dst = 0.f;
}

 void audioBlock(int16_t *output, const int32_t samples)
{
    freq.inc = (freq.dst - freq.val) * samples * TS;
    for (int i = 0; i < samples; i++)
    {

        float samp = whiteNoise(&noise);

        /* Exciter */
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

        // float to int16 conversion
        const int16_t sampleOut = (int16_t)(32767.0f * samp);

        // output to circular buffer
        output[i << 1] = sampleOut;
        output[(i << 1) + 1]  = sampleOut;

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