/**
 * @file lcd.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#include "../Inc/lcd.h"

#include <stdlib.h>

#include "filterbank.h"
#include "help_func.h"
#include "multiLFO.h"

extern spectrum_t spectrum;
extern lfo_t lfo;
Screen *screen;


Screen* ct_screen_init() {
	BSP_LCD_Init();
	Screen *screen = (Screen*) malloc(sizeof(Screen));
	screen->width = BSP_LCD_GetXSize();
	screen->height = BSP_LCD_GetYSize();
	screen->addr[0] = LCD_FB_START_ADDRESS;
	screen->addr[1] = LCD_FB_START_ADDRESS + screen->width * screen->height * 4;
	screen->front = 1;
	BSP_LCD_LayerDefaultInit(0, screen->addr[0]);
	BSP_LCD_LayerDefaultInit(1, screen->addr[1]);
	BSP_LCD_SetLayerVisible(0, DISABLE);
	BSP_LCD_SetLayerVisible(1, ENABLE);
	BSP_LCD_SelectLayer(0);
	return screen;
}

void ct_screen_flip_buffers(Screen *screen) {
	// wait for VSYNC
	while (!(LTDC->CDSR & LTDC_CDSR_VSYNCS));
	BSP_LCD_SetLayerVisible(screen->front, DISABLE);
	screen->front ^= 1;
	BSP_LCD_SetLayerVisible(screen->front, ENABLE);
	BSP_LCD_SelectLayer(ct_screen_backbuffer_id(screen));
}

uint32_t* ct_screen_backbuffer_ptr(Screen *screen) {
	return (uint32_t*)(screen->addr[ct_screen_backbuffer_id(screen)]);
}


uint32_t ct_screen_backbuffer_id(Screen *screen) {
	return 1 - screen->front;
}

void Display_Default(void)
{
	/* Default LCD settings */
	// BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
}


void Display_Init(void)
{
	BSP_LCD_SetBrightness(100);
	// ct_screen_flip_buffers(screen);

	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);

	// BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_SetFont(&FontChicagoFLF16);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
	BSP_LCD_Clear(COLOR_BACKGROUND);

	/* Set the LCD Text Color */
	// BSP_LCD_SetTextColor(COLOR_TEXT);

	// char str[32];
	// sprintf(str, "partials");
	// BSP_LCD_DisplayStringAt(32, 32, (uint8_t *)str, RIGHT_MODE);

	// Display partials area
	//BSP_LCD_SetTextColor(COLOR_PAD);
	// BSP_LCD_FillRect(PARTIALAREA_X, PARTIALAREA_Y, PARTIALSAREAWIDTH, PARTIALSAREAHEIGHT);
	// BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	// BSP_LCD_FillRect(PARTIALSAREA_X + PADDING, PARTIALSAREA_Y + PADDING, PARTIALSAREAWIDTH - 2 * PADDING, PARTIALSAREAHEIGHT - 2 * PADDING);


	/* Display LCD messages */
	// BSP_LCD_SetFont(&FontInconsolataNerdFont32);
	// BSP_LCD_DisplayStringAt(PARTIALSAREAWIDTH, 3 * PADDING, (uint8_t *)"Bell", RIGHT_MODE);


	// Display menu bar
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
	BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), MENUBARHEIGHT);
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP2);
	BSP_LCD_FillRect(0, MENUBARHEIGHT, BSP_LCD_GetXSize(), 5);

	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP3);
	BSP_LCD_DrawVLine(ITEM_WIDTH, 0, 48);
	BSP_LCD_DrawVLine(ITEM_WIDTH * 2, 0, 48);
	BSP_LCD_DrawVLine(ITEM_WIDTH * 3, 0, 48);
	BSP_LCD_DrawVLine(ITEM_WIDTH * 4, 0, 48);

	// fill menu items
	BSP_LCD_SetBackColor(COLOR_PAD_TRANSP1);
	BSP_LCD_SetFont(&FontChicagoFLF16);
	BSP_LCD_SetTextColor(ORANGE_TEXT);
	BSP_LCD_DisplayStringAt((ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Bell 1", CENTER_MODE);
	BSP_LCD_DisplayStringAt(3 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2 , 20, (uint8_t *)"Saw", CENTER_MODE);
	BSP_LCD_SetTextColor(COLOR_TEXT_ACTIVE);
	BSP_LCD_DisplayStringAt(5 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"LFO", CENTER_MODE);
	BSP_LCD_DisplayStringAt(7 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Delay", CENTER_MODE);
	BSP_LCD_DisplayStringAt(9 * (ITEM_WIDTH/2) - BSP_LCD_GetXSize()/2, 20, (uint8_t *)"Reverb", CENTER_MODE);


	// Display touchscreen area for note triggering
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
	BSP_LCD_FillRect(TRIGGERAREA_X, TRIGGERAREA_Y, TRIGGERAREAWIDTH, TRIGGERAREAHEIGHT);
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP2);
	BSP_LCD_FillRect(TRIGGERAREA_X, TRIGGERAREA_Y-5, TRIGGERAREAWIDTH, 5);

	// Display partials
	// Display_partials(&spectrum);

	// Display morphing bar
	Display_morphBar(PARTIALSAREA_Left + 20);
}


/**
 * Display spectrum components as vertical lines within the partials area
 * @param freqRatios partials frequency ratios
 * @param amps partials amplitudes
 */
void Display_partials(spectrum_t *s)
{

	clearPartialsArea();

	// BSP_LCD_SelectLayer(1);
	const float hLength = PARTIALSAREAWIDTH - PADDING;

	for (int i = 0; i < BANDS; i++)
	{
		const uint16_t partialXpos = (uint16_t)(s->freqRatios[i] * (hLength / (BANDS - 4)) + PARTIALSAREA_X - 2 * PADDING);
		const uint16_t partialHeight = (uint16_t)(MAXPARTIALHEIGHT * s->amps[i] * lfo.output[i]);

		// Color transparency as function of the partial amplitude
		uint32_t partialColor = (uint32_t)(scale(0.f, 1.f, 0.5f, 1.f, s->amps[i]) * 0xFF) * 0x1000000 + BLUE_PARTIALS;

		// display only if partial fits within the partials area
		if (partialXpos < PARTIALSAREA_X + PARTIALSAREAWIDTH - PADDING)
		{
			BSP_LCD_SetTextColor(partialColor);
			BSP_LCD_FillRect(partialXpos, PARTIALSAREA_Y + 2 * PADDING + (MAXPARTIALHEIGHT - partialHeight), 6, partialHeight);
		}
	}

}

void Display_morphBar(uint16_t x)
{
	BSP_LCD_SetTextColor(ORANGE_UI);
	BSP_LCD_FillRect(x, PARTIALSAREA_Y, 16, 7);
	BSP_LCD_FillCircle(x, PARTIALSAREA_Y + 3, 3);
	BSP_LCD_FillCircle(x + 16, PARTIALSAREA_Y + 3, 3);
}

void clearTriggerArea(void)
{
	BSP_LCD_SetTextColor(COLOR_PAD_TRANSP1);
	BSP_LCD_FillRect(TRIGGERAREA_X, TRIGGERAREA_Y, TRIGGERAREAWIDTH, TRIGGERAREAHEIGHT);
}


void clearPartialsArea(void)
{
	BSP_LCD_SetTextColor(COLOR_BACKGROUND);
	BSP_LCD_FillRect(PARTIALSAREA_X, PARTIALSAREA_Y, PARTIALSAREAWIDTH, PARTIALSAREAHEIGHT );
}






// void clearMorphArea(void)
// {
// 	// BSP_LCD_SetTextColor(COLOR_PAD_TRANSP);
// 	BSP_LCD_SetBackColor(COLOR_BACKGROUND);
// 	BSP_LCD_FillRect(PARTIALSAREA_X, TRIGGERAREAHEIGHT - 8, PARTIALSAREAWIDTH, MORPHAREAHEIGHT);
// 	BSP_LCD_SetTextColor(COLOR_TEXT);
// 	// BSP_LCD_DisplayStringAt(MORPHAREA_X+ PADDING, MORPHAREA_Y + MORPHAREAHEIGHT - 24, (uint8_t *)"morph pad", RIGHT_MODE);
// }