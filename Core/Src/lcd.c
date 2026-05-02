/**
 * @file lcd.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#include "../Inc/lcd.h"

#include "filterbank.h"
#include "help_func.h"
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
	BSP_LCD_DisplayStringAt(2 * BORDER, 2 * BORDER, (uint8_t *)"Bell", RIGHT_MODE);
	// char str[32];
	// sprintf(str, "partials");

	// BSP_LCD_DisplayStringAt(32, 32, (uint8_t *)str, RIGHT_MODE);

	// Display partials area
	BSP_LCD_SetTextColor(COLOR_ELEMENTS);
	BSP_LCD_DrawRect(BORDER, BORDER, BSP_LCD_GetXSize() - 2 * BORDER, BSP_LCD_GetYSize()/2 - 2 * BORDER);

	// Display freq and magnitude axes
	BSP_LCD_DrawHLine(2 * BORDER, BSP_LCD_GetYSize()/2 - 2 * BORDER, BSP_LCD_GetXSize() - 4 * BORDER);
	BSP_LCD_DrawVLine(2 * BORDER, 2 * BORDER, BSP_LCD_GetYSize()/2 - 4 * BORDER);

	// Display touchscreen area for note triggering
	BSP_LCD_SetTextColor(COLOR_ELEMENTS);
	BSP_LCD_DrawRect(BORDER, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 2 * BORDER, BSP_LCD_GetYSize()/2 - 16);
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(2 * BORDER, BSP_LCD_GetYSize()/2 + BORDER, BSP_LCD_GetXSize() - 4 * BORDER, BSP_LCD_GetYSize()/2 - 3 * BORDER);

	// Display partials
	Display_partials(Bell1Partials, RampAmp);
}


/**
 * Display spectrum components as vertical lines within the partials area
 * @param freqRatios partials frequency ratios
 * @param amps partials amplitudes
 */
void Display_partials(const float *freqRatios, const float *amps)
{
	const float hLength = BSP_LCD_GetXSize() - 4 * BORDER;

	for (int i = 0; i < BANDS; i++)
	{
		const uint16_t partialXpos = (uint16_t)(freqRatios[i] * (hLength / BANDS) + BORDER);
		const uint16_t partialLength = (uint16_t)(MAXPARTIALLENGTH * amps[i]);
		uint32_t partialColor;
		partialColor =  (uint32_t)(scale(0.f, 1.f, 0.4f, 1.f, amps[i]) * 0xFF) * 0x1000000 + 0xFFFFFF;

		// display only if partial fits within the partials area
		if (partialXpos < BSP_LCD_GetXSize() - 2 * BORDER)
		{
			BSP_LCD_SetTextColor(partialColor);
			BSP_LCD_DrawVLine(partialXpos, 5 * BORDER + (MAXPARTIALLENGTH - partialLength), partialLength);
		}
	}
}


void clearTriggerArea(void)
{
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(32, BSP_LCD_GetYSize()/2 + 16, BSP_LCD_GetXSize() - 64, BSP_LCD_GetYSize()/2 - 48);
}


void clearPartialsArea(void)
{
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(33, 32, BSP_LCD_GetXSize() - 64, BSP_LCD_GetYSize()/2 - 64);
}

