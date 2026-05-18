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
static lv_obj_t * partial[BANDS];



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



void GUI_Init()
{
	// change background color
	// lv_obj_set_style_bg_color(lv_screen_active(),lv_palette_main(LV_PALETTE_NONE),LV_PART_MAIN);


	/**************************** Draw tabs  ****************************/
	/*Create a Tab view object*/
	lv_obj_t * tabview;
	uint32_t tab_count = 0;
	uint32_t i = 0;

	tabview = lv_tabview_create(lv_screen_active());
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
	lv_tabview_set_tab_bar_size(tabview, TABRHEIGHT);


	lv_obj_t * tab_buttons = lv_tabview_get_tab_bar(tabview);
	// lv_obj_set_style_bg_color(tab_buttons, lv_palette_main(LV_PALETTE_NONE), 0);
	lv_obj_set_style_text_color(tab_buttons, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);


	// Add 4 tabs
	lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "main");
	lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "spectrum");
	lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "lfo");
	lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "effects");

	tab_count = lv_tabview_get_tab_count(tabview);

	for(i = 0; i < tab_count; i++) {
		lv_obj_t * button = lv_obj_get_child(tab_buttons, i);

		lv_obj_set_style_bg_color(button, lv_palette_main(LV_PALETTE_NONE), 0);
		lv_obj_set_style_bg_color(button, lv_palette_darken(LV_PALETTE_GREY, 4), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_text_color(button, lv_palette_lighten(LV_PALETTE_GREY, 5), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_side(button, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_color(button, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_width(button, 4, LV_PART_MAIN | LV_STATE_CHECKED);
	}

	lv_obj_set_style_bg_color(tab1, lv_palette_main(LV_PALETTE_NONE), 0);
	lv_obj_set_style_bg_color(tab2, lv_palette_main(LV_PALETTE_NONE), 0);
	lv_obj_set_style_bg_color(tab3, lv_palette_main(LV_PALETTE_NONE), 0);
	lv_obj_set_style_bg_color(tab4, lv_palette_main(LV_PALETTE_NONE), 0);


	/*Add content to the tabs*/
	lv_obj_t * label = lv_label_create(tab1);
	lv_label_set_text(label, "First tab");

	label = lv_label_create(tab2);
	lv_label_set_text(label, "Second tab");

	label = lv_label_create(tab3);
	lv_label_set_text(label, "Third tab");

	lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);



	/*************** Draw line separating partials from trigger area   *******************/
	lv_obj_t * middleLine = lv_line_create(lv_screen_active());
	static lv_point_precise_t line_points[] = { {0, 0}, {780, 0} };
	static lv_style_t style_line;

	lv_style_init(&style_line);
	lv_style_set_line_width(&style_line, 4);
	lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_line_rounded(&style_line, true);

	lv_line_set_points(middleLine, line_points, 2);     /*Set the points*/
	lv_obj_add_style(middleLine, &style_line, 0);
	lv_obj_center(middleLine);


	/**************************** Draw partials  ****************************/

	// create style for all the partials
	static lv_style_t style_partials;
	lv_style_init(&style_partials);
	lv_style_set_bg_color(&style_partials, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
	lv_style_set_radius(&style_partials, 0);

	for (int i = 0; i < BANDS; i++)
	{
		partial[i] = lv_obj_create(lv_screen_active());
		lv_obj_remove_flag(partial[i], LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_style(partial[i], &style_partials, 0);

		lv_obj_set_x(partial[i], 64 + i * 32);
		lv_obj_set_y(partial[i], 25);
		lv_obj_set_size(partial[i], 8, 0);
	}
}


void GUI_LCDProcess()
{
	for (int i = 0; i < BANDS; i++)
	{
		float amplitude = spectrum.amps[i] * lfo.output[i];

		int32_t height = (int32_t)(MAXPARTIALHEIGHT * amplitude);
		const int32_t xPos = (uint16_t)(spectrum.freqRatios[i] * (PARTIALSAREAWIDTH / (BANDS - 4)) + PARTIALSAREA_X);
		const int32_t yPos = PARTIALSAREA_Y + (MAXPARTIALHEIGHT - height);
		// display only if partial fits within the partials area
		if (xPos < PARTIALSAREA_X + PARTIALSAREAWIDTH - PADDING)
		{
			lv_obj_set_pos(partial[i], xPos, yPos);
			lv_obj_set_height(partial[i], height);
			lv_obj_set_style_opa(partial[i], 55 + 200 * amplitude, 0);
		}
	}
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