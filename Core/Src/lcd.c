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
	BSP_LCD_DisplayStringAt(16, BSP_LCD_GetYSize()/2, (uint8_t *)str, CENTER_MODE);


	BSP_LCD_DrawRect(16, 16, BSP_LCD_GetXSize() - 32, BSP_LCD_GetYSize()/2 - 48);


	BSP_LCD_DrawHLine(32, BSP_LCD_GetYSize()/2 - 48, BSP_LCD_GetXSize() - 64);
	BSP_LCD_DrawVLine(32, 32, BSP_LCD_GetYSize()/2 - 80);

	// Display partials
	Display_partials();
}

void Display_partials(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
	const uint32_t hLength = BSP_LCD_GetXSize() - 64;
	for (int i = 0; i < 16; i++)
	{
		BSP_LCD_DrawVLine(CB808Partials[i] * (hLength / 16.f) + 32, 64, 128);
	}
}

void Display_Init2(void)
{
	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);

	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_Clear(LCD_COLOR_BLACK);


	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);

	/* Display LCD messages */
	char str[128];
	sprintf(str, "SAI DEMO - %d voices additive synthesis", harmonics);
	BSP_LCD_SetFont(&FontInconsolataNerdFont20);
	BSP_LCD_DisplayStringAt(16, BSP_LCD_GetYSize()/2, (uint8_t *)str, CENTER_MODE);
}
