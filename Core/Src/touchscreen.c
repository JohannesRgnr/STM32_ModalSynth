/**
 * @file touchscreen.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 24.04.26
 * @note
 */

#include "../Inc/touchscreen.h"

#include <stdio.h>

#include "exciter.h"
#include "filterbank.h"
#include "help_func.h"
#include "audio.h"
#include "lcd.h"

uint16_t audioLevel = 60;

uint8_t wasTouched = 0;
extern line_t exciterAmp, freq;
extern filterbank_t filterbank;
extern spectrum_t spectrum;
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

    uint16_t x = TS_State.touchX[0];
    uint16_t y = TS_State.touchY[0];

    // char message[30];
    // sprintf(message, "Touches: %d", TS_State.touchDetected);
    // // BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    // BSP_LCD_DisplayStringAt(30, 30, (uint8_t *)message, LEFT_MODE);

    // if inside trigger area
    if (x > TRIGGERAREA_Left  && x < TRIGGERAREA_Right && y > TRIGGERAREA_Top  && y < TRIGGERAREA_Bottom)
    {
        ts_triggerArea(x, y, wasTouched);
    }
    else if (x > MORPHAREA_Left && x < MORPHAREA_Right && y > MORPHAREA_Top && y < MORPHAREA_Bottom)
    {
       ts_MorphArea(x, y, wasTouched);
    }

    wasTouched = TS_State.touchDetected;


}

/**
 * Trigger new note when touched, or glide frequency and decay if already touched
 * @param x x touch coordinate
 * @param y y touch coordinate
 * @param state currently touched or not
 */
static void ts_triggerArea(uint16_t x, uint16_t y, uint8_t state)
{
    if (state == 0)
    {
        if (TS_State.touchDetected == 1) // new single finger touch
        {
            // evaluate fundamental frequency
            float midiNote = scale(TRIGGERAREA_Left, TRIGGERAREA_Right, 24, 90, x);
            midiNote = clip(midiNote, 24, 90);
            float frequency = mtof(midiNote);

            // immediately jump to frequency
            freq.val = frequency;
            freq.dst = frequency;
            filterbank.freq = frequency;

            filterbank_update(&filterbank);
            Trigger_Note(&exciterAmp);

            // redraw trigger area
            clearTriggerArea();
            // draw trajectory
            BSP_LCD_SetTextColor(BLUE_UI_MAT);
            BSP_LCD_FillCircle(x, y, 8);
        }

    }
    else  // was already touched
    {
        if (TS_State.touchDetected == 1)
        {
            float midiNote = scale(TRIGGERAREA_Left, TRIGGERAREA_Right, 24, 90, x);
            midiNote = clip(midiNote, 24, 90);
            const float duration = scale(TRIGGERAREA_Top,TRIGGERAREA_Bottom, 4, 10, y);

            freq.dst = mtof(midiNote);
            filterbank.decay = duration;
            filterbank_update(&filterbank);

            // draw trajectory
            BSP_LCD_SetTextColor(BLUE_UI_MAT);
            BSP_LCD_FillCircle(x, y, 8);
        }
    }
}


static void ts_MorphArea(uint16_t x, uint16_t y, uint8_t state)
{
    if (TS_State.touchDetected == 1) // new touch
    {
        float xfade = scale(MORPHAREA_Left + 50, MORPHAREA_Right - 50, 0.0f, 1.0f, x);
        xfade = clip(xfade, 0.f, 1.f);
        spectrum_xfade(&spectrum, xfade);
        filterbank_spectrum(&filterbank, &spectrum);
        filterbank_update(&filterbank);
        clearPartialsArea();
        Display_partials(&spectrum);
        HAL_Delay(20);
    }
}