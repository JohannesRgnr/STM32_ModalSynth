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
#include "lcd.h"
#include "spectra.h"


uint8_t preset = 1;

extern filterbank_t filterbank;

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

void UserButton(void) // to choose "presets" = partials freq ratios + amplitudes
{
    int i;
    if (CheckForUserButton())
    {
        Display_Default();
        BSP_LCD_SetFont(&FontInconsolataNerdFont20);
        preset ++;

        if (preset > NUMPRESETS)
        {
            preset = 1;
        }

        // clear partials display
        clearPartialsArea();

        BSP_LCD_SetTextColor(COLOR_TEXT);

        switch ( preset )
        {
        case 1:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"Bell", RIGHT_MODE);
            Display_partials(Bell1Partials, ExpAmp);
            filterbank_spectrum(&filterbank, Bell1Partials, RampAmp);
            break;
        case 2:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"Gong", RIGHT_MODE);
            Display_partials(GongPartials, ExpAmp);
            filterbank_spectrum(&filterbank, GongPartials, ExpAmp);
            break;
        case 3:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"Chord", RIGHT_MODE);
            Display_partials(ChordPartials, ExpAmp);
            filterbank_spectrum(&filterbank, ChordPartials, ExpAmp);
            break;
        case 4:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"Sawtooth", RIGHT_MODE);
            Display_partials(SawPartials, SawAmp);
            filterbank_spectrum(&filterbank, SawPartials, SawAmp);
            break;
        case 5:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"Square", RIGHT_MODE);
            Display_partials(SquarePartials, SquareAmp);
            filterbank_spectrum(&filterbank, SquarePartials, SquareAmp);
            break;
        case 6:
            BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 2 * BORDER, (uint8_t *)"CB808", RIGHT_MODE);
            Display_partials(CB808Partials, ConstAmp);
            filterbank_spectrum(&filterbank, CB808Partials, ConstAmp);
            break;
        }
    }
}

