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
#include "filters.h"
#include "help_func.h"
#include "multiLFO.h"
#include "touchpad.h"
#include "touchscreen.h"

extern spectrum_t spectrum;
extern lfo_t lfo;

// static lv_obj_t * label;
static lv_obj_t * partial[BANDS];
static lv_obj_t * morphCursor;

lv_obj_t * scr_main;
lv_obj_t * scr_spectrum;
lv_obj_t * scr_lfo;
lv_obj_t * scr_effects;

lv_obj_t * tabview_main;
lv_obj_t * tabview_spectrum;
lv_obj_t * tabview_lfo;
lv_obj_t * tabview_effects;
uint8_t active_tab;

static lv_style_t partials_style;
static lv_style_t trigArea_style;
static lv_style_t dropdown_style;

lv_obj_t * trigArea;

TS_StateTypeDef  TS_State;

int32_t maxPartialHeight = BIGPARTIALHEIGHT;


void GUI_Init()
{
	GUI_mainScreen();
	GUI_spectrumScreen();
	GUI_lfoScreen();
	GUI_effectsScreen();
	lv_screen_load(scr_main);
}


static void GUI_mainScreen()
{
	scr_main = lv_obj_create(NULL);
	/************* Create timer for partials display refresh ************/
	lv_timer_t * timer_partialDisplay = lv_timer_create(GUI_refreshPartials, 60, NULL);


	tabview_main = lv_tabview_create(scr_main);

	create_tabview(tabview_main);


	/*************** Draw line separating partials from trigger area   *******************/
	// lv_obj_t * middleLine = lv_line_create(scr_main);
	// static lv_point_precise_t line_points[] = { {0, 0}, {780, 0} };
	// static lv_style_t style_line;
	//
	// lv_style_init(&style_line);
	// lv_style_set_line_width(&style_line, 4);
	// lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_GREY));
	// lv_style_set_line_rounded(&style_line, true);
	//
	// lv_line_set_points(middleLine, line_points, 2);     /*Set the points*/
	// lv_obj_add_style(middleLine, &style_line, 0);
	// lv_obj_center(middleLine);


	/************************ Draw trigger area   ***************************/
	// static lv_style_t trigArea_style;
	lv_style_init(&trigArea_style);
	lv_style_set_radius(&trigArea_style, 5);

	/*Create an object with the new style*/
	trigArea = lv_obj_create(scr_main);
	lv_obj_add_style(trigArea, &trigArea_style, 0);
	lv_obj_remove_flag(trigArea, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_bg_color(trigArea, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
	// lv_obj_set_style_radius(trigArea, 4, 0);
	lv_obj_set_pos(trigArea, 20, TRIGGERAREAHEIGHT);
	lv_obj_set_size(trigArea, 760, 230);


	/*************** Draw cursor indicating morph position   *******************/
	morphCursor = lv_obj_create(scr_main);
	lv_obj_set_size(morphCursor, 20,1);
	lv_obj_set_pos(morphCursor, 0, TABRHEIGHT + 32);
	lv_obj_set_style_bg_color(morphCursor, lv_palette_main(LV_PALETTE_ORANGE), 0);
	lv_obj_set_style_radius(morphCursor, 0, 0);


	/**************************** Init partials  ****************************/
	// create style for all the partials

	lv_style_init(&partials_style);
	lv_style_set_bg_color(&partials_style, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
	lv_style_set_radius(&partials_style, 0);

	for (int i = 0; i < BANDS; i++)
	{
		partial[i] = lv_obj_create(scr_main);
		lv_obj_remove_flag(partial[i], LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_style(partial[i], &partials_style, 0);
		lv_obj_set_size(partial[i], 8, 0);
	}
}


static void GUI_spectrumScreen()
{
	scr_spectrum = lv_obj_create(NULL);

	tabview_spectrum = lv_tabview_create(scr_spectrum);

	create_tabview(tabview_spectrum);
}

static void GUI_spectrumTab()
{
	lv_obj_set_size(trigArea, 760, 130);
	lv_obj_set_pos(trigArea, 20, TRIGGERAREAHEIGHT - 40);
	maxPartialHeight = SMALLPARTIALHEIGHT;


	/**** dropdown menus ****/
	lv_style_init(&dropdown_style);

	lv_style_set_bg_color(&dropdown_style, lv_palette_darken(LV_PALETTE_GREY, 4));
	lv_style_set_size(&dropdown_style, 200, 38);
	lv_style_set_border_color(&dropdown_style, lv_palette_darken(LV_PALETTE_GREY, 2));
	lv_style_set_border_width(&dropdown_style, 3);
	lv_style_set_radius(&dropdown_style, 4);
	lv_style_set_text_color(&dropdown_style, lv_palette_lighten(LV_PALETTE_GREY, 5));
	lv_style_set_text_font(&dropdown_style, &lv_font_montserrat_24);
	lv_style_set_text_align(&dropdown_style, LV_TEXT_ALIGN_CENTER);

	lv_obj_t * spectrum_a = lv_dropdown_create(scr_main);

	lv_obj_add_style(spectrum_a, &dropdown_style, 0);
	lv_obj_set_pos(spectrum_a, 450, 380);
	lv_dropdown_set_dir(spectrum_a, LV_DIR_TOP);
	lv_dropdown_set_options(spectrum_a, "Bell 1\nBell 2\nGong\nChord\nSaw\nSquare\n808 CB");
	lv_dropdown_set_symbol(spectrum_a, NULL);

	lv_obj_t * dropdown_a=lv_dropdown_get_list(spectrum_a);
	lv_obj_set_style_text_align(dropdown_a, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(dropdown_a, &lv_font_montserrat_24, 0);
	lv_obj_set_style_pad_all(dropdown_a, 8, 0);
	lv_obj_set_style_pad_row(dropdown_a, 8, 0);

	lv_obj_t * spectrum_b = lv_dropdown_create(scr_main);

	lv_obj_add_style(spectrum_b, &dropdown_style, 0);
	lv_obj_set_pos(spectrum_b, 150, 380);
	lv_dropdown_set_dir(spectrum_b, LV_DIR_TOP);
	lv_dropdown_set_options(spectrum_b, "Bell 1\nBell 2\nGong\nChord\nSaw\nSquare\n808 CB");
	lv_dropdown_set_symbol(spectrum_b, NULL);

	lv_obj_t * dropdown_b=lv_dropdown_get_list(spectrum_b);
	lv_obj_set_style_text_align(dropdown_b, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(dropdown_b, &lv_font_montserrat_24, 0);
	lv_obj_set_style_pad_all(dropdown_b, 8, 0);
	lv_obj_set_style_pad_row(dropdown_b, 8, 0);

}


static void GUI_lfoScreen()
{
	scr_lfo = lv_obj_create(NULL);

	tabview_lfo = lv_tabview_create(scr_lfo);

	create_tabview(tabview_lfo);
}


static void GUI_effectsScreen()
{
	scr_effects = lv_obj_create(NULL);

	tabview_effects = lv_tabview_create(scr_effects);

	create_tabview(tabview_effects);
}


static void tabview_event_cb(lv_event_t * event)
{
	//lv_obj_t * label = lv_label_create(lv_screen_active());
	lv_obj_t * tabview = lv_event_get_target_obj(event);

	active_tab = lv_tabview_get_tab_active(tabview);

	switch ( active_tab )
	{
	default:
	case 0:
		lv_screen_load(scr_main);
		lv_obj_set_size(trigArea, 760, 230);
		lv_obj_set_pos(trigArea, 20, TRIGGERAREAHEIGHT);
		maxPartialHeight = BIGPARTIALHEIGHT;
		lv_tabview_set_active(tabview_main, 0, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_spectrum, 0, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_lfo, 0, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 0, LV_ANIM_OFF);
		break;
	case 1:
		// lv_screen_load(scr_spectrum);
		GUI_spectrumTab();
		lv_tabview_set_active(tabview_main, 1, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_spectrum, 1, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_lfo, 1, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 1, LV_ANIM_OFF);
		break;
	case 2:
		lv_screen_load(scr_lfo);
		lv_tabview_set_active(tabview_main, 2, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_spectrum, 2, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_lfo, 2, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 2, LV_ANIM_OFF);
		break;
	case 3:
		lv_screen_load(scr_effects);
		lv_tabview_set_active(tabview_main, 3, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_spectrum, 3, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_lfo, 3, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 3, LV_ANIM_OFF);
		break;
	}

	/*Refresh the text*/
	//lv_label_set_text_fmt(label, "%" LV_PRId32, active_tab);
	//lv_obj_align_to(label, tabview, LV_ALIGN_OUT_TOP_MID, 0, 32);    /*Align top of the slider*/

}

void GUI_refreshMorphCursor(float x)
{
	float width = x;
	width = clip(width, 20, 780);
	lv_obj_set_width(morphCursor, width);
}



void GUI_refreshPartials(lv_timer_t * timer)
{
	for (int i = 0; i < BANDS; i++)
	{
		float amplitude = spectrum.amps[i] * lfo.output[i];

		int32_t height = (int32_t)(maxPartialHeight * amplitude);
		const int32_t xPos = (uint16_t)((spectrum.freqRatios[i] -1) * PARTIALSPACING + PARTIALSAREA_X);
		const int32_t yPos = PARTIALSAREA_Y + (maxPartialHeight - height);
		// display only if partial fits within the partials area
		if (xPos < PARTIALSAREA_X + PARTIALSAREAWIDTH)
		{
			lv_obj_set_pos(partial[i], xPos, yPos);
			lv_obj_set_height(partial[i], height);
			lv_obj_set_style_opa(partial[i], 75 + 180 * amplitude, 0);
		} else
		{
			lv_obj_set_style_opa(partial[i], 0, 0);
		}
	}
}



static void create_tabview(lv_obj_t * tv)
{
/**************************** Draw tabs  ****************************/
	/*Create a Tab view object*/
	// lv_obj_t * tabview;
	uint32_t tab_count = 0;
	uint32_t i = 0;

	lv_obj_t * tabview = tv;

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
		lv_obj_set_style_border_side(button, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_color(button, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_width(button, 8, LV_PART_MAIN | LV_STATE_CHECKED);
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

	lv_obj_add_event_cb(tabview, tabview_event_cb, LV_EVENT_VALUE_CHANGED, lv_tabview_get_tab_active);
}
