/**
 * @file lcd.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#include "../Inc/lcd.h"
#include "spectra.h"
#include <stdio.h>

#include "oscillators.h"

void Display_Default(void)
{
	/* Default LCD settings */
	  BSP_LCD_SetFont(&Font12);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}


void Display_Init(void)
{
	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);

	// BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);

	/* Display LCD messages */
	char str[32];
	sprintf(str, "partials");
	// BSP_LCD_SetFont(&FontInconsolataNerdFont20);
	BSP_LCD_DisplayStringAt(32, 32, (uint8_t *)str, RIGHT_MODE);

	// Display partials area
	BSP_LCD_DrawRect(16, 16, BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 32);

	// Display freq and magnitude axes
	BSP_LCD_DrawHLine(32, BSP_LCD_GetYSize()/2 - 32, BSP_LCD_GetXSize() - 64);
	BSP_LCD_DrawVLine(32, 32, BSP_LCD_GetYSize()/2 - 64);

	// Display touchscreen area for note triggering
	BSP_LCD_DrawRect(16, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 32);
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	BSP_LCD_FillRect(32, BSP_LCD_GetYSize()/2 + 16, BSP_LCD_GetXSize() - 64, BSP_LCD_GetYSize()/2 - 64);

	// Display partials
	Display_partials(SawPartials, SawAmp, LCD_COLOR_CYAN);
}

void Display_partials(const float *freqRatios, const float *amps, uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	const float hLength = BSP_LCD_GetXSize() - 64;

	for (int i = 0; i < 16; i++)
	{
		const uint16_t partialLength = (uint16_t)(128 * amps[i]);
		BSP_LCD_DrawVLine((uint16_t)(freqRatios[i] * (hLength / 16.f) + 16.0f), 80 + (128 - partialLength), partialLength);
	}
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
}


