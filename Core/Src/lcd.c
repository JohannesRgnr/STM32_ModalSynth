/**
 * @file lcd.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#include "../Inc/lcd.h"

#include <stdio.h>

#include "oscillators.h"

#include "spectra.h"




void Display_Default(void)
{

	/* Default LCD settings */
	// BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(COLOR_TEXT);
	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
}


void Display_Init(void)
{

	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);

	// BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
	BSP_LCD_Clear(COLOR_BACKGROUND);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(COLOR_TEXT);

	/* Display LCD messages */
	BSP_LCD_SetFont(&FontInconsolataNerdFont20);
	BSP_LCD_DisplayStringAt(24, 32, (uint8_t *)"Bell   ", RIGHT_MODE);
	// char str[32];
	// sprintf(str, "partials");

	// BSP_LCD_DisplayStringAt(32, 32, (uint8_t *)str, RIGHT_MODE);

	// Display partials area
	BSP_LCD_SetTextColor(COLOR_ELEMENTS);
	BSP_LCD_DrawRect(16, 16, BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 32);

	// Display freq and magnitude axes
	BSP_LCD_DrawHLine(32, BSP_LCD_GetYSize()/2 - 32, BSP_LCD_GetXSize() - 64);
	BSP_LCD_DrawVLine(32, 32, BSP_LCD_GetYSize()/2 - 64);

	// Display touchscreen area for note triggering
	BSP_LCD_SetTextColor(COLOR_ELEMENTS);
	BSP_LCD_DrawRect(16, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 16);
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(32, BSP_LCD_GetYSize()/2 + 16, BSP_LCD_GetXSize() - 64, BSP_LCD_GetYSize()/2 - 48);

	// Display partials
	Display_partials(Bell1Partials, ExpAmp, COLOR_PARTIALS);
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
	BSP_LCD_SetTextColor(COLOR_TEXT);
}


