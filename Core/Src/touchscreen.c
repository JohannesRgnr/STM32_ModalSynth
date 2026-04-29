/**
 * @file touchscreen.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 24.04.26
 * @note
 */

#include "../Inc/touchscreen.h"
#include "exciter.h"
#include "filterbank.h"
#include "help_func.h"
#include "spectra.h"
#include "audio.h"

uint16_t audioLevel = 60;

uint8_t wasTouched = 0;
extern line_t exciterAmp;
extern filterbank_t filterbank;
TS_StateTypeDef  TS_State;

void Touchscreen(void){

    BSP_TS_GetState(&TS_State);
    // if (TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch */
    // {
    //     BSP_TS_GetState(&TS_State);
    //     uint16_t x = TS_State.touchX[0];
    //     uint16_t y = TS_State.touchY[0];
    //
    //     // Volume control in top-left and botton-left corners
    //     if (y < 50 && x < 50)
    //     {
    //         // /* Increase volume by 1% */
    //         if (audioLevel < 99)
    //             audioLevel += 1;
    //         else
    //             audioLevel = 100;
    //         // // sprintf((char*)FreqStr, "VOL: %3lu  ", SoundVolume);
    //         BSP_AUDIO_OUT_SetVolume(audioLevel);
    //         // Display_Default();
    //         // BSP_LCD_DisplayStringAt(230, BSP_LCD_GetYSize() - 16, (uint8_t *)FreqStr, LEFT_MODE);
    //     }
    //     else if (y > (BSP_LCD_GetYSize() - 50) && x < 50 )
    //     {
    //         // // Display_Default();
    //         // /* Decrease volume by 1% */
    //         if (audioLevel > 1)
    //             audioLevel -= 1;
    //         else
    //             audioLevel = 0;
    //         // // sprintf((char*)FreqStr, "VOL: %3lu  ", SoundVolume);
    //         BSP_AUDIO_OUT_SetVolume(audioLevel);
    //         // BSP_LCD_DisplayStringAt(230, BSP_LCD_GetYSize() - 16, (uint8_t *)FreqStr, LEFT_MODE);
    //     }
    //     HAL_Delay(10);
    // }

    // one touch to trigger exciter
    if (wasTouched == 0)
    {
        if (TS_State.touchDetected == 1)
        {
            uint16_t x = TS_State.touchX[0];
            uint16_t y = TS_State.touchY[0];
           BSP_LCD_DrawRect(16, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 32);
            if (x > 16 && y > BSP_LCD_GetYSize()/2 && x < (BSP_LCD_GetXSize() - 16) && (BSP_LCD_GetYSize() - 16))
            {
                const float midiNote = scale(16, 768, 24, 90, x);
                filterbank.freq = mtof(midiNote);
                filterbank_update(&filterbank, Bell1Partials, ExpAmp);
                Trigger_Note(&exciterAmp);

            }
        }
    }
    else
    {
        if (TS_State.touchDetected == 1)
        {
            uint16_t x = TS_State.touchX[0];
            uint16_t y = TS_State.touchY[0];
            const float midiNote = scale(16, 768, 24, 90, x);
            const float duration = scale(BSP_LCD_GetYSize()/2,BSP_LCD_GetYSize() - 32, 4, 10, y);
            filterbank.freq = mtof(midiNote);
            filterbank.decay = duration;
            filterbank_update(&filterbank, Bell1Partials, ExpAmp);
        }
    }
    wasTouched = TS_State.touchDetected;
    // HAL_Delay(5);
}