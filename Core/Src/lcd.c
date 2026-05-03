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

extern spectrum_t spectrum;

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
	BSP_LCD_SetFont(&FontChicagoFLF16);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
	BSP_LCD_Clear(COLOR_BACKGROUND);

	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(COLOR_TEXT);

	// char str[32];
	// sprintf(str, "partials");

	// BSP_LCD_DisplayStringAt(32, 32, (uint8_t *)str, RIGHT_MODE);

	// Display partials area
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(BORDER, BORDER, PARTIALSAREAWIDTH, PARTIALSAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(2 * BORDER, 2 * BORDER, PARTIALSAREAWIDTH - 2 * BORDER, PARTIALSAREAHEIGHT - 2 * BORDER);


	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(COLOR_TEXT);

	/* Display LCD messages */
	// BSP_LCD_SetFont(&FontInconsolataNerdFont32);
	BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * BORDER, (uint8_t *)"Bell", RIGHT_MODE);

	// Display freq and magnitude axes
	// BSP_LCD_SetTextColor(GREY_UI);
	//BSP_LCD_DrawHLine(2 * BORDER, PARTIALSAREAHEIGHT, PARTIALSAREAWIDTH - 2 * BORDER);
	//BSP_LCD_DrawVLine(2 * BORDER, 2 * BORDER, PARTIALSAREAHEIGHT - 2 * BORDER);

	// Display touchscreen area for note triggering
	BSP_LCD_SetTextColor(GREY_UI);
	BSP_LCD_DrawRect(BORDER, BSP_LCD_GetYSize()/2 , BSP_LCD_GetXSize() - 2 * BORDER, BSP_LCD_GetYSize()/2 - BORDER);
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(2 * BORDER, BSP_LCD_GetYSize()/2 + BORDER, BSP_LCD_GetXSize() - 4 * BORDER, BSP_LCD_GetYSize()/2 - 3 * BORDER);

	// Display partials
	Display_partials(&spectrum);
}


/**
 * Display spectrum components as vertical lines within the partials area
 * @param freqRatios partials frequency ratios
 * @param amps partials amplitudes
 */
void Display_partials(spectrum_t *s)
{
	const float hLength = PARTIALSAREAWIDTH - 4 * BORDER;

	for (int i = 0; i < BANDS; i++)
	{
		const uint16_t partialXpos = (uint16_t)(s->freqRatios[i] * (hLength / BANDS) + 2 * BORDER);
		const uint16_t partialLength = (uint16_t)(MAXPARTIALLENGTH * s->amps[i]);
		uint32_t partialColor;
		partialColor =  (uint32_t)(scale(0.f, 1.f, 0.6f, 1.f, s->amps[i]) * 0xFF) * 0x1000000 + BLUE_PARTIALS;

		// display only if partial fits within the partials area
		if (partialXpos < PARTIALSAREAWIDTH - 2 * BORDER)
		{
			BSP_LCD_SetTextColor(partialColor);
			BSP_LCD_DrawVLine(partialXpos,   3 * BORDER+ (MAXPARTIALLENGTH - partialLength), partialLength);
		}
	}
}


void clearTriggerArea(void)
{
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(2 * BORDER, BSP_LCD_GetYSize()/2 + BORDER, BSP_LCD_GetXSize() - 4 * BORDER, BSP_LCD_GetYSize()/2 - 3 * BORDER);
}


void clearPartialsArea(void)
{
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(2 * BORDER + 1, 2 * BORDER, PARTIALSAREAWIDTH - 2 * BORDER, PARTIALSAREAHEIGHT - 2 * BORDER);
}

