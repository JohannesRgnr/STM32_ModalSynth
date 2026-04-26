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
#include "help_func.h"
#include "oscillators.h"
#include "exciter.h"



/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t codecBuffer[BUFFER_SIZE]; // x samples X 2 channels (interleaved)

oscillator_t osc1;
line_t exciterAmp;

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
    osc_init(&osc1, 1.0f, 110, 0, 0, 0);
    // cordicAdditiveInit(&cordic1, 55);
    // HAL_Delay(500);
    // osc_init(&osc2, 1.0f, 110, 0, 0, 0);
}

 void audioBlock(int16_t *output, const int32_t samples)
{

    for (int i = 0; i < samples; i++)
    {

        const float sampleL = cordicAdditive(&osc1);
        const int16_t sampleOut = (int16_t)(32767.0f * sampleL);
       // float sampleR = sampleL;  // RIGHT

        output[i << 1] = sampleOut;
        output[(i << 1) + 1]  = sampleOut;
    }
}


void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
    audioBlock((int16_t *)(codecBuffer), BUFFER_SIZE_DIV_4);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
    audioBlock((int16_t *)(codecBuffer + BUFFER_SIZE_DIV_2), BUFFER_SIZE_DIV_4);
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