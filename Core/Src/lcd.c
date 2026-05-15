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
#include "multiLFO.h"

extern spectrum_t spectrum;
extern lfo_t lfo;


static lv_obj_t * label;
lv_obj_t * obj;

static void slider_event_cb(lv_event_t * e)
{
	lv_obj_t * slider = lv_event_get_target_obj(e);

	/*Refresh the text*/
	lv_label_set_text_fmt(label, "%" LV_PRId32, lv_slider_get_value(slider));
	lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}

/**
 * @title Slider with live value label
 * @brief Mirror a slider's value into a label anchored above it.
 *
 * A 200 px wide slider is centered on the active screen with a label placed
 * 15 px above it via `lv_obj_align_to` and `LV_ALIGN_OUT_TOP_MID`. An
 * `LV_EVENT_VALUE_CHANGED` callback reads `lv_slider_get_value` and rewrites
 * the label text, re-aligning it after each update.
 */
void lv_oneSlider(void)
{
	/*Create a slider in the center of the display*/
	lv_obj_t * slider = lv_slider_create(lv_screen_active());
	lv_obj_set_width(slider, 200);                          /*Set the width*/
	lv_obj_center(slider);                                  /*Align to the center of the parent (screen)*/
	lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /*Assign an event function*/


	/*Create a label above the slider*/
	label = lv_label_create(lv_screen_active());
	lv_label_set_text(label, "0");
	lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}


static void anim_x_cb(void * var, int32_t v)
{
	lv_obj_set_x((lv_obj_t *) var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
	lv_obj_set_size((lv_obj_t *) var, v, v);
}

/**
 * @title Infinite playback animation
 * @brief Grow a red circle while sliding it right, then reverse and repeat.
 *
 * A red circular object sits on the left edge of the active screen. One
 * `lv_anim_t` drives `lv_obj_set_size` from 10 to 50 over 1000 ms; the same
 * configured animation is then reused with `lv_obj_set_x` running from 10
 * to 240. Both run with `lv_anim_path_ease_in_out`, a 300 ms reverse stage
 * after a 100 ms reverse delay, a 500 ms gap between cycles, and
 * `LV_ANIM_REPEAT_INFINITE`.
 */
void lv_circle_anim()
{
	static uint32_t oldRadius;
	obj = lv_obj_create(lv_screen_active());
	lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
	lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

	lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);
	uint32_t radius = (uint32_t)(spectrum.amps[0] * 500.0f);
	// lv_obj_set_size(obj, radius, radius);
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_values(&a, oldRadius, radius);
	lv_anim_set_duration(&a, 10);
	lv_anim_set_reverse_delay(&a, 1);
	lv_anim_set_reverse_duration(&a, 10);
	//lv_anim_set_repeat_delay(&a, 500);
	//lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
	//lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

	lv_anim_set_exec_cb(&a, anim_size_cb);
	// lv_anim_start(&a);
	//lv_anim_set_exec_cb(&a, anim_x_cb);
	//lv_anim_set_values(&a, 10, 240);
	//lv_anim_start(&a);
	oldRadius = radius;
}

void GUI_LCDProcess(spectrum_t *s)
{
	static float oldRadius;
	float x = (lfo.output[0] * 240.0f);
	lv_obj_set_x(obj, x);
	lv_obj_set_size(obj, 64, 64);
	// lv_anim_t a;
	// lv_anim_init(&a);
	// lv_anim_set_var(&a, obj);
	// lv_anim_set_exec_cb(&a, anim_x_cb);
	// lv_anim_set_values(&a, 10, x);
	//
	//
	// lv_anim_start(&a);

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