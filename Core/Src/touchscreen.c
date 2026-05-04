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
#include "lcd.h"
#include "spectra.h"

uint16_t audioLevel = 60;

uint8_t wasTouched = 0;
uint8_t LFO_btn = 1;

uint8_t delay_btn = 1;
uint8_t reverb_btn = 1;

uint8_t presetLeft = 1;
uint8_t presetRight = 5;

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
    // if inside partials area
    else if (x > PARTIALSAREA_Left && x < PARTIALSAREA_Right && y > PARTIALSAREA_Top && y < PARTIALSAREA_Bottom)
    {
       ts_MorphArea(x, y, wasTouched);
    }
    // if inside menu bar area
    else if (y < MENUBARHEIGHT + 8)
    {
        ts_MenuArea(x, wasTouched);
    }

    // Display_partials(&spectrum);
    wasTouched = TS_State.touchDetected;
    // HAL_Delay(12);

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
        float xfade = scale(PARTIALSAREA_Left + 20, PARTIALSAREA_Right - 20, 0.0f, 1.0f, x);
        xfade = clip(xfade, 0.f, 1.f);
        spectrum_xfade(&spectrum, xfade);
        filterbank_spectrum(&filterbank, &spectrum);
        filterbank_update(&filterbank);


        if ( x > PARTIALSAREA_Left + 20 && x < PARTIALSAREA_Right - 20 )
        {
            // clearPartialsArea();
           // Display_partials(&spectrum);

            BSP_LCD_SetTextColor(ORANGE_UI);
            BSP_LCD_FillRect(x, PARTIALSAREA_Y, 16, 7);
            BSP_LCD_FillCircle(x, PARTIALSAREA_Y + 3, 3);
            BSP_LCD_FillCircle(x + 16, PARTIALSAREA_Y + 3, 3);
        }

    }
}


static void ts_MenuArea(uint16_t x, uint8_t state)
{
    if (state == 0)
    {
        if (TS_State.touchDetected == 1)
        {
            if ( x > 0 && x < ITEM_WIDTH)                           // 1st menu item - Left spectrum
            {
                inc_Preset(LEFT_SPECTRUM);
            }
            else if (x > ITEM_WIDTH && x < 2 * ITEM_WIDTH)          // 2nd menu item - Right spectrum
            {
                inc_Preset(RIGHT_SPECTRUM);
            }
            else if (x > 2 * ITEM_WIDTH && x < 3 * ITEM_WIDTH)      // 3rd menu item - LFO on/off
            {
                LFO_btn = 1 - LFO_btn; // toggle button state
                if (LFO_btn == 0)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_INACTIVE);
                    BSP_LCD_DisplayStringAt(5 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"LFO", CENTER_MODE);

                } else if (LFO_btn == 1)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
                    BSP_LCD_DisplayStringAt(5 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"LFO", CENTER_MODE);

                }
            }
            else if (x > 3 * ITEM_WIDTH && x < 4 * ITEM_WIDTH)      // 4th menu item - Delay on/off
            {
                delay_btn = 1 - delay_btn;
                if (delay_btn == 0)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_INACTIVE);
                    BSP_LCD_DisplayStringAt(7 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Delay", CENTER_MODE);
                } else if (delay_btn == 1)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
                    BSP_LCD_DisplayStringAt(7 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Delay", CENTER_MODE);
                }
            }
            else if (x > 4 * ITEM_WIDTH && x < BSP_LCD_GetXSize())      // 5th menu item - Reverb on/off
            {
                reverb_btn = 1 - reverb_btn;
                if (reverb_btn == 0)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_INACTIVE);
                    BSP_LCD_DisplayStringAt(9 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Reverb", CENTER_MODE);
                } else if (reverb_btn == 1)
                {
                    BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
                    BSP_LCD_DisplayStringAt(9 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Reverb", CENTER_MODE);
                }
            }
        }
    }
    HAL_Delay(20);
}



static void inc_Preset(uint8_t leftOrRight)
{
    if (leftOrRight == LEFT_SPECTRUM)
    {
        presetLeft ++;

        if (presetLeft > NUMPRESETS)
        {
            presetLeft = 1;
        }

        BSP_LCD_SetBackColor(COLOR_PAD_TRANSP1);
        BSP_LCD_SetFont(&FontChicagoFLF16);
        BSP_LCD_SetTextColor(ORANGE_TEXT);

        switch ( presetLeft )
        {
        default:
        case 1:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Bell 1", CENTER_MODE);
            spectrum_load(&spectrum, Bell1Partials, ExpAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 2:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Bell 2", CENTER_MODE);
            spectrum_load(&spectrum, Bell2Partials, ExpAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 3:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Gong", CENTER_MODE);
            spectrum_load(&spectrum, GongPartials, ExpAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 4:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Chord", CENTER_MODE);
            spectrum_load(&spectrum, ChordPartials, ExpAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 5:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Saw", CENTER_MODE);
            spectrum_load(&spectrum, SawPartials, SawAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 6:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Square", CENTER_MODE);
            spectrum_load(&spectrum, SquarePartials, SquareAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 7:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(0, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"808 CB", CENTER_MODE);
            spectrum_load(&spectrum, CB808Partials, ConstAmp, LEFT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        }
    }
    else if (leftOrRight == RIGHT_SPECTRUM)
    {
        presetRight ++;

        if (presetRight > NUMPRESETS)
        {
            presetRight = 1;
        }

        BSP_LCD_SetBackColor(COLOR_PAD_TRANSP1);
        BSP_LCD_SetFont(&FontChicagoFLF16);
        BSP_LCD_SetTextColor(ORANGE_TEXT);

        switch ( presetRight )
        {
        default:
        case 1:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Bell 1", CENTER_MODE);
            spectrum_load(&spectrum, Bell1Partials, ExpAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 2:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Bell 2", CENTER_MODE);
            spectrum_load(&spectrum, Bell2Partials, ExpAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 3:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Gong", CENTER_MODE);
            spectrum_load(&spectrum, GongPartials, ExpAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 4:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Chord", CENTER_MODE);
            spectrum_load(&spectrum, ChordPartials, ExpAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 5:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Saw", CENTER_MODE);
            spectrum_load(&spectrum, SawPartials, SawAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 6:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Square", CENTER_MODE);
            spectrum_load(&spectrum, SquarePartials, SquareAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        case 7:
            BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
            BSP_LCD_FillRect(ITEM_WIDTH+1, 0, ITEM_WIDTH, MENUBARHEIGHT);
            BSP_LCD_SetTextColor(ORANGE_TEXT);
            BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"808 CB", CENTER_MODE);
            spectrum_load(&spectrum, CB808Partials, ConstAmp, RIGHT_SPECTRUM);
            Display_partials(&spectrum);
            filterbank_spectrum(&filterbank, &spectrum);
            break;
        }
    }

}