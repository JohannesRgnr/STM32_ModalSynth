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
	BSP_LCD_SetBrightness(100);

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
	// BSP_LCD_FillRect(PARTIALAREA_X, PARTIALAREA_Y, PARTIALSAREAWIDTH, PARTIALSAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(PARTIALAREA_X + PADDING, PARTIALAREA_Y + PADDING, PARTIALSAREAWIDTH - 2 * PADDING, PARTIALSAREAHEIGHT - 2 * PADDING);


	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(COLOR_TEXT);

	/* Display LCD messages */
	// BSP_LCD_SetFont(&FontInconsolataNerdFont32);
	// BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Bell", RIGHT_MODE);

	// Display freq and magnitude axes
	// BSP_LCD_SetTextColor(GREY_UI);
	//BSP_LCD_DrawHLine(2 * PADDING, PARTIALSAREAHEIGHT, PARTIALSAREAWIDTH - 2 * PADDING);
	//BSP_LCD_DrawVLine(2 * PADDING, 2 * PADDING, PARTIALSAREAHEIGHT - 2 * PADDING);

	// Display morphing area
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP);
	BSP_LCD_SetBackColor(COLOR_PAD_TRANSP);
	BSP_LCD_FillRect(MORPHAREA_X, MORPHAREA_Y, MORPHAREAWIDTH, MORPHAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_TEXT);
	BSP_LCD_DisplayStringAt(2* PADDING, MORPHAREA_Y + MORPHAREAHEIGHT - 24, (uint8_t *)"morph pad", RIGHT_MODE);


	// Display touchscreen area for note triggering
	BSP_LCD_SetBackColor(COLOR_PAD);
	BSP_LCD_SetTextColor(GREY_UI);
	// BSP_LCD_DrawRect(TRIGGERAREA_X, TRIGGERAREA_Y, TRIGGERAREAWIDTH, TRIGGERAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_FillRect(TRIGGERAREA_X, TRIGGERAREA_Y, TRIGGERAREAWIDTH, TRIGGERAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_TEXT);
	BSP_LCD_DisplayStringAt(2 * PADDING, TRIGGERAREA_Y + TRIGGERAREAHEIGHT - 24, (uint8_t *)"trigger pad", RIGHT_MODE);

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
	// BSP_LCD_SelectLayer(1);
	const float hLength = PARTIALSAREAWIDTH - PADDING;

	for (int i = 0; i < BANDS; i++)
	{
		const uint16_t partialXpos = (uint16_t)(s->freqRatios[i] * (hLength / BANDS) + PARTIALAREA_X);
		const uint16_t partialHeight = (uint16_t)(MAXPARTIALHEIGHT * s->amps[i]);

		// Color transparency as function of the partial amplitude
		uint32_t partialColor = (uint32_t)(scale(0.f, 1.f, 0.5f, 1.f, s->amps[i]) * 0xFF) * 0x1000000 + BLUE_PARTIALS;

		// display only if partial fits within the partials area
		if (partialXpos < PARTIALAREA_X + PARTIALSAREAWIDTH - PADDING)
		{
			BSP_LCD_SetTextColor(partialColor);
			BSP_LCD_FillRect(partialXpos, 3 * PADDING+ (MAXPARTIALHEIGHT - partialHeight), 6, partialHeight);
		}
	}

}


void clearTriggerArea(void)
{
	BSP_LCD_SetTextColor(COLOR_PAD);
	BSP_LCD_SetBackColor(COLOR_PAD);
	BSP_LCD_FillRect(TRIGGERAREA_X, TRIGGERAREA_Y, TRIGGERAREAWIDTH, TRIGGERAREAHEIGHT);

	BSP_LCD_SetTextColor(COLOR_TEXT);
	BSP_LCD_DisplayStringAt(2 * PADDING, TRIGGERAREA_Y + TRIGGERAREAHEIGHT - 24, (uint8_t *)"trigger pad", RIGHT_MODE);
}


void clearPartialsArea(void)
{
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(PARTIALAREA_X, PARTIALAREA_Y, PARTIALSAREAWIDTH, PARTIALSAREAHEIGHT );
}

void clearMorphArea(void)
{
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP);
	BSP_LCD_SetBackColor(COLOR_PAD_TRANSP);
	BSP_LCD_FillRect(MORPHAREA_X, MORPHAREA_Y, MORPHAREAWIDTH, MORPHAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_TEXT);
	BSP_LCD_DisplayStringAt(2* PADDING, MORPHAREA_Y + MORPHAREAHEIGHT - 24, (uint8_t *)"morph pad", RIGHT_MODE);
}