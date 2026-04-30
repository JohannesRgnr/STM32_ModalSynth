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
#include "lcd.h"

uint16_t audioLevel = 60;

uint8_t wasTouched = 0;
extern line_t exciterAmp, freq;
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


    if (wasTouched == 0) // 1st touch since untouched = new note is triggered
    {
        if (TS_State.touchDetected == 1)
        {
            uint16_t x = TS_State.touchX[0];
            uint16_t y = TS_State.touchY[0];

            if (x > 48 && y > BSP_LCD_GetYSize()/2 + 32 && x < (BSP_LCD_GetXSize() - 48) && y <  (BSP_LCD_GetYSize() - 48))
            {
                // evaluate fundamental frequency
                const float midiNote = scale(32, 768, 24, 90, x);
                float frequency = mtof(midiNote);

                // immediately jump to frequency
                freq.val = frequency;
                freq.dst = frequency;
                filterbank.freq = frequency;

                filterbank_update(&filterbank);
                Trigger_Note(&exciterAmp);

                // redraw trigger area
                // BSP_LCD_SetTextColor(COLOR_BACKGROUND);
                // BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize(), BSP_LCD_GetYSize()/2);
                BSP_LCD_SetTextColor(COLOR_ELEMENTS);
                BSP_LCD_DrawRect(16, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 16);
                BSP_LCD_SetTextColor(COLOR_PAD);
                BSP_LCD_FillRect(32, BSP_LCD_GetYSize()/2 + 16, BSP_LCD_GetXSize() - 64, BSP_LCD_GetYSize()/2 - 48);
                // draw trajectory
                BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
                BSP_LCD_FillCircle(x, y, 12);
            }
        }
    }
    else // it was touched already
    {
        if (TS_State.touchDetected == 1)
        {
            uint16_t x = TS_State.touchX[0];
            uint16_t y = TS_State.touchY[0];

            if (x > 48 && y > BSP_LCD_GetYSize()/2 + 32 && x < (BSP_LCD_GetXSize() - 48) && y <  (BSP_LCD_GetYSize() - 48))
            {
                const float midiNote = scale(32, 768, 24, 90, x);
                const float duration = scale(BSP_LCD_GetYSize()/2,BSP_LCD_GetYSize() - 32, 4, 10, y);

                freq.dst = mtof(midiNote);
                filterbank.decay = duration;
                filterbank_update(&filterbank);

                // draw trajectory
                BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
                BSP_LCD_FillCircle(x, y, 12);
            }
        }
    }
    wasTouched = TS_State.touchDetected;

    HAL_Delay(5);
}