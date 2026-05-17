/**
 * @file usr_btn.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 29.04.26
 * @note
 */

#include "../Inc/usr_btn.h"

#include "filterbank.h"
// #include "lcd.h"
#include "spectra.h"
#include "help_func.h"
#include "exciter.h"
#include "stm32f769i_discovery.h"


uint8_t preset = 1;

extern filterbank_t filterbank;
extern spectrum_t spectrum;
extern line_t exciterAmp, freq;


static uint8_t CheckForUserButton(void)
{
    if (BSP_PB_GetState(BUTTON_WAKEUP) != RESET)
    {
        HAL_Delay(10);
        while (BSP_PB_GetState(BUTTON_WAKEUP) != RESET);
        return 1 ;
    }
    return 0;
}

// void UserButton(void) // to choose "presets" = partials freq ratios + amplitudes
// {
//     if (CheckForUserButton())
//     {
//         Display_Default();
//         preset ++;
//
//         if (preset > NUMPRESETS)
//         {
//             preset = 1;
//         }
//
//         // clear partials display
//         clearPartialsArea();
//
//         BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
//
//         switch ( preset )
//         {
//         default:
//         case 1:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Bell", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         case 2:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Gong", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         case 3:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Chord", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         case 4:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Sawtooth", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         case 5:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Square", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         case 6:
//             BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"808 Cowbell", RIGHT_MODE);
//             Display_partials(&spectrum);
//             filterbank_spectrum(&filterbank, &spectrum);
//             break;
//         }
//     }
// }





void UserButton(void)
{
    if (CheckForUserButton())
    {
        // evaluate fundamental frequency
        float midiNote = 48;
        float frequency = mtof(midiNote);

        // immediately jump to frequency
        freq.val = frequency;
        freq.dst = frequency;
        filterbank.freq = frequency;
        const float duration = 7.5f;


        filterbank.decay = duration;
        filterbank_update(&filterbank);
        Trigger_Note(&exciterAmp);
    }

}