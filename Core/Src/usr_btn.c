/**
 * @file usr_btn.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 29.04.26
 * @note
 */

#include "../Inc/usr_btn.h"

#include "lcd.h"
#include "spectra.h"


uint8_t preset = 0;

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

void UserButton(void) // to choose "presets" = partials
{
    int i;
    if (CheckForUserButton())
    {
        Display_Default();
        preset ++;

        if (preset > 5)
        {
            preset = 1;
        }

        switch ( preset )
        {
        case 1:

            BSP_LCD_DisplayStringAt(318, BSP_LCD_GetYSize() - 16, (uint8_t *)"Current preset: Gong   ", LEFT_MODE);
            break;
        case 2:

            BSP_LCD_DisplayStringAt(318, BSP_LCD_GetYSize() - 16, (uint8_t *)"Current preset: Bell   ", LEFT_MODE);
            break;
        case 3:

            BSP_LCD_DisplayStringAt(318, BSP_LCD_GetYSize() - 16, (uint8_t *)"Current preset: Chord  ", LEFT_MODE);
            break;
        case 4:

            BSP_LCD_DisplayStringAt(318, BSP_LCD_GetYSize() - 16, (uint8_t *)"Current preset: Saw    ", LEFT_MODE);
            break;
        case 5:

            BSP_LCD_DisplayStringAt(318, BSP_LCD_GetYSize() - 16, (uint8_t *)"Current preset: Cowbell", LEFT_MODE);
            break;
        }
    }
}