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
#include "spectra.h"
#include "stm32f769i_discovery_audio.h"
#include "touchpad.h"
#include "touchscreen.h"

extern spectrum_t spectrum;
extern filterbank_t filterbank;
extern lfo_t lfo;
extern float lfo_speed;
extern float lfo_amp;
extern float lfo_phaseShift;

extern uint32_t delay_time;
extern float delay_feedback;
extern float delay_wet;

extern float reverb_amount;
extern float reverb_feedback;
extern float reverb_time;

static lv_obj_t * partial[BANDS];
static lv_obj_t * morphCursor;

lv_obj_t * scr_main;
lv_obj_t * scr_effects;
lv_obj_t * scr_settings;

lv_obj_t * tabview_main;
lv_obj_t * tabview_lfo;
lv_obj_t * tabview_effects;
lv_obj_t * tabview_settings;


lv_obj_t * spectrum_a_label;
lv_obj_t * spectrum_b_label;
lv_obj_t * spectrum_a;
lv_obj_t * spectrum_b;
lv_obj_t * lfo_speed_arc;
lv_obj_t * lfo_amp_arc;
lv_obj_t * lfo_phase_arc;

lv_obj_t * lfo_speed_label;
lv_obj_t * lfo_amp_label;
lv_obj_t * lfo_phase_label;

uint8_t active_tab;
uint8_t active_presetA, active_presetB;


static lv_style_t partials_style;
static lv_style_t trigArea_style;
static lv_style_t style_arc_bg;
static lv_style_t style_arc_indicator;
static lv_style_t style_arc_knob;


lv_obj_t * trigArea;

TS_StateTypeDef  TS_State;

int32_t maxPartialHeight = BIGPARTIALHEIGHT;
int32_t trigAreaHeight = TRIGGERAREAHEIGHT;
int32_t trigAreaWidth = TRIGGERAREAWIDTH;
int32_t trigAreaX = TRIGGERAREA_X;
int32_t trigAreaTop;

// void GUI_lfoScreen(void);

void GUI_Init()
{
	GUI_mainScreen();
	// GUI_spectrumScreen();
	// GUI_lfoScreen();
	GUI_effectsScreen();
	GUI_settingsScreen();
	lv_screen_load(scr_main);
}


static void GUI_mainScreen()
{
	/************* Create main screen ************/
	scr_main = lv_obj_create(NULL);

	/************* Create timer for partials display refresh ************/
	lv_timer_t * timer_partialDisplay = lv_timer_create(GUI_refreshPartials, 66, NULL);

	/************* Create tabs ************/
	tabview_main = lv_tabview_create(scr_main);
	create_tabview(tabview_main);


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
	lv_obj_set_pos(trigArea, 20, TRIGGERAREA_Y);
	lv_obj_set_size(trigArea, trigAreaWidth - 40, TRIGGERAREAHEIGHT);


	/*************** Draw line indicating morph position   *******************/
	morphCursor = lv_obj_create(scr_main);
	lv_obj_set_size(morphCursor, 20,1);
	lv_obj_set_pos(morphCursor, 0, TABRHEIGHT + 32);
	lv_obj_set_style_bg_color(morphCursor, lv_palette_main(LV_PALETTE_ORANGE), 0);
	lv_obj_set_style_radius(morphCursor, 0, 0);


	/**************************** Init partials  ****************************/
	// create style for all the partials

	lv_style_init(&partials_style);
	lv_style_set_bg_color(&partials_style, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
	lv_style_set_radius(&partials_style, 3);

	for (int i = 0; i < BANDS; i++)
	{
		partial[i] = lv_obj_create(scr_main);
		lv_obj_remove_flag(partial[i], LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_style(partial[i], &partials_style, 0);
		lv_obj_set_size(partial[i], 12, 0);
	}

	/**************************** Dropdown menus ****************************/
	// create labels
	spectrum_a_label = lv_label_create(scr_main);
	lv_label_set_text(spectrum_a_label, "preset A");
	lv_obj_set_pos(spectrum_a_label, 24, 230);
	lv_obj_set_style_text_align(spectrum_a_label, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_text_font(spectrum_a_label, &lv_font_montserrat_20, 0);
	lv_obj_set_style_text_color(spectrum_a_label, lv_palette_darken(LV_PALETTE_GREY, 1),0);

	spectrum_b_label = lv_label_create(scr_main);
	lv_label_set_text(spectrum_b_label, "preset B");
	lv_obj_set_pos(spectrum_b_label, 564, 230);
	lv_obj_set_style_text_align(spectrum_b_label, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_text_font(spectrum_b_label, &lv_font_montserrat_20, 0);
	lv_obj_set_style_text_color(spectrum_b_label, lv_palette_darken(LV_PALETTE_GREY, 1),0);

	// specify dropdown styles
	static lv_style_t dropdown_style;
	lv_style_init(&dropdown_style);

	lv_style_set_bg_color(&dropdown_style, lv_palette_main(LV_PALETTE_NONE));
	lv_style_set_size(&dropdown_style, 100, 32);
	lv_style_set_border_color(&dropdown_style, lv_palette_darken(LV_PALETTE_GREY, 2));
	lv_style_set_border_width(&dropdown_style, 1);
	lv_style_set_radius(&dropdown_style, 5);
	lv_style_set_text_color(&dropdown_style, lv_palette_lighten(LV_PALETTE_GREY, 1));
	lv_style_set_text_font(&dropdown_style, &lv_font_montserrat_20);
	lv_style_set_text_align(&dropdown_style, LV_TEXT_ALIGN_CENTER);

	// create dropdown menus
	spectrum_a = lv_dropdown_create(scr_main);
	lv_obj_add_style(spectrum_a, &dropdown_style, 0);
	lv_obj_set_pos(spectrum_a, 120, 224);
	lv_dropdown_set_dir(spectrum_a, LV_DIR_TOP);
	lv_dropdown_set_options(spectrum_a, "bell 1\nbell 2\ngong\nchord\nsaw\nsquare\n808 CB");
	lv_dropdown_set_symbol(spectrum_a, NULL);
	lv_dropdown_set_selected(spectrum_a, Bell1);

	lv_obj_t * spectrum_a_list=lv_dropdown_get_list(spectrum_a);
	lv_obj_set_style_text_align(spectrum_a_list, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(spectrum_a_list, &lv_font_montserrat_20, 0);
	lv_obj_set_style_pad_all(spectrum_a_list, 8, 0);
	lv_obj_set_style_pad_row(spectrum_a_list, 8, 0);

	spectrum_b = lv_dropdown_create(scr_main);
	lv_obj_add_style(spectrum_b, &dropdown_style, 0);
	lv_obj_set_pos(spectrum_b, 660, 224);
	lv_dropdown_set_dir(spectrum_b, LV_DIR_TOP);
	lv_dropdown_set_options(spectrum_b, "bell 1\nbell 2\ngong\nchord\nsaw\nsquare\n808 CB");
	lv_dropdown_set_symbol(spectrum_b, NULL);
	lv_dropdown_set_selected(spectrum_b, Saw);

	lv_obj_t * spectrum_b_list=lv_dropdown_get_list(spectrum_b);
	lv_obj_set_style_text_align(spectrum_b_list, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(spectrum_b_list, &lv_font_montserrat_20, 0);
	lv_obj_set_style_pad_all(spectrum_b_list, 8, 0);
	lv_obj_set_style_pad_row(spectrum_b_list, 8, 0);

	// add events to dropdowns
	lv_obj_add_event_cb(spectrum_a, spectrum_a_event_cb, LV_EVENT_VALUE_CHANGED, lv_dropdown_get_selected);
	lv_obj_add_event_cb(spectrum_b, spectrum_b_event_cb, LV_EVENT_VALUE_CHANGED, lv_dropdown_get_selected);


	/**************************** LFO slider ****************************/
	// // specify styles
	// static lv_style_t style_slider_track;
	// static lv_style_t style_slider_indicator;
	// static lv_style_t style_slider_knob;
	// static lv_style_t style_slider_knob_pressed;
	//
	//  lv_style_init(&style_slider_track);
	//  // lv_style_set_bg_opa(&style_slider_track, (255 * 100 / 100));
	//  lv_style_set_bg_color(&style_slider_track, lv_palette_lighten(LV_PALETTE_GREY, 2));
	//  lv_style_set_radius(&style_slider_track, 12);
	//  lv_style_set_border_color(&style_slider_track, lv_color_hex(0x0f172a));
	//  lv_style_set_border_width(&style_slider_track, 1);
	//
	//  lv_style_init(&style_slider_indicator);
	//  // lv_style_set_bg_opa(&style_slider_indicator, (255 * 100 / 100));
	//  lv_style_set_bg_color(&style_slider_indicator, lv_palette_main(LV_PALETTE_ORANGE));
	//  // lv_style_set_bg_grad_color(&style_slider_indicator, lv_color_hex(0xec4899));
	//  // lv_style_set_bg_grad_dir(&style_slider_indicator, LV_GRAD_DIR_HOR);
	//  lv_style_set_radius(&style_slider_indicator, 12);
	//
	//  lv_style_init(&style_slider_knob);
	//  // lv_style_set_bg_opa(&style_slider_knob, (255 * 100 / 100));
	//  lv_style_set_bg_color(&style_slider_knob, lv_palette_main(LV_PALETTE_ORANGE));
	//  lv_style_set_radius(&style_slider_knob, 100);
	//  // lv_style_set_border_color(&style_slider_knob, lv_color_hex(0x8b5cf6));
	//  // lv_style_set_border_width(&style_slider_knob, 3);
	//  // lv_style_set_pad_all(&style_slider_knob, 6);
	//
	//
	//  lv_style_init(&style_slider_knob_pressed);
	//  lv_style_set_bg_color(&style_slider_knob_pressed, lv_color_hex(0xfff7ed));
	//  lv_style_set_outline_color(&style_slider_knob_pressed, lv_palette_main(LV_PALETTE_RED));
	//  lv_style_set_outline_width(&style_slider_knob_pressed, 6);
	//  lv_style_set_outline_pad(&style_slider_knob_pressed, 4);
	//  lv_style_set_outline_opa(&style_slider_knob_pressed, 120);
 //
	// // speed slider
	// lv_obj_t * slider_lfo_speed = lv_slider_create(scr_main);
 //
	// lv_obj_set_size(slider_lfo_speed, 16, 200);
	// lv_slider_set_orientation(slider_lfo_speed, LV_SLIDER_ORIENTATION_VERTICAL);
	// lv_obj_set_pos(slider_lfo_speed, 400, 200);
	// lv_slider_set_min_value(slider_lfo_speed, 0);
	// lv_slider_set_max_value(slider_lfo_speed, 127);
	// lv_slider_set_value(slider_lfo_speed, 0, false);
 //
	// lv_obj_add_style(slider_lfo_speed, &style_slider_track, LV_PART_MAIN);
	// lv_obj_add_style(slider_lfo_speed, &style_slider_indicator, LV_PART_INDICATOR);
	// lv_obj_add_style(slider_lfo_speed, &style_slider_knob, LV_PART_KNOB);
	// lv_obj_add_style(slider_lfo_speed, &style_slider_knob_pressed, LV_PART_KNOB | LV_STATE_PRESSED);

	// // amp slider
	// lv_obj_t * slider_lfo_amp = lv_slider_create(scr_main);
	//
	// lv_obj_set_size(slider_lfo_amp, 16, 240);
	// lv_slider_set_orientation(slider_lfo_amp, LV_SLIDER_ORIENTATION_VERTICAL);
	// lv_obj_set_pos(slider_lfo_amp, 500, 200);
	// lv_slider_set_min_value(slider_lfo_amp, 0);
	// lv_slider_set_max_value(slider_lfo_amp, 127);
	// lv_slider_set_value(slider_lfo_amp, 0, false);
	//
	// lv_obj_add_style(slider_lfo_amp, &style_slider_track, LV_PART_MAIN);
	// lv_obj_add_style(slider_lfo_amp, &style_slider_indicator, LV_PART_INDICATOR);
	// lv_obj_add_style(slider_lfo_amp, &style_slider_knob, LV_PART_KNOB);
	// lv_obj_add_style(slider_lfo_amp, &style_slider_knob_pressed, LV_PART_KNOB | LV_STATE_PRESSED);
	//
	// phase shift slider
	// lv_obj_t * lfo_phase_slider = lv_slider_create(scr_main);
	//
	// lv_obj_set_size(lfo_phase_slider, 200, 20);
	// lv_slider_set_orientation(lfo_phase_slider, LV_SLIDER_ORIENTATION_HORIZONTAL);
	// lv_obj_set_pos(lfo_phase_slider, 490, 420);
	// lv_slider_set_min_value(lfo_phase_slider, 0);
	// lv_slider_set_max_value(lfo_phase_slider, 10);
	// lv_slider_set_value(lfo_phase_slider, 0, false);
	//
	// lv_obj_add_style(lfo_phase_slider, &style_slider_track, LV_PART_MAIN);
	// lv_obj_add_style(lfo_phase_slider, &style_slider_indicator, LV_PART_INDICATOR);
	// lv_obj_add_style(lfo_phase_slider, &style_slider_knob, LV_PART_KNOB);
	// lv_obj_add_style(lfo_phase_slider, &style_slider_knob_pressed, LV_PART_KNOB | LV_STATE_PRESSED);
	// lv_obj_t * phase_label = lv_label_create(lfo_phase_slider);
	// lv_obj_set_align(phase_label, LV_ALIGN_CENTER);
	// lv_label_set_text(phase_label, "phase");
	// lv_obj_set_style_text_color(phase_label, lv_palette_darken(LV_PALETTE_GREY, 4),0);

	/**************************** LFO dials ****************************/



	// static lv_subject_t subject_value2;

	lv_style_init(&style_arc_bg);
	lv_style_set_arc_color(&style_arc_bg, lv_palette_lighten(LV_PALETTE_GREY, 2));
	lv_style_set_arc_width(&style_arc_bg, 10);
	lv_style_set_arc_rounded(&style_arc_bg, true);
	lv_style_set_arc_opa(&style_arc_bg, 180);


	lv_style_init(&style_arc_indicator);
	lv_style_set_arc_color(&style_arc_indicator, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 2));
	lv_style_set_arc_width(&style_arc_indicator, 10);
	lv_style_set_arc_rounded(&style_arc_indicator, true);
	lv_style_set_pad_all(&style_arc_indicator, 0);

    lfo_speed_arc = lv_arc_create(scr_main);
    lv_obj_set_size(lfo_speed_arc, 110, 110);
	lv_obj_set_pos(lfo_speed_arc, 60, 240);
    lv_arc_set_min_value(lfo_speed_arc, 0);
    lv_arc_set_max_value(lfo_speed_arc, 127);
    lv_arc_set_value(lfo_speed_arc, 0);
    lv_obj_add_style(lfo_speed_arc, &style_arc_bg, LV_PART_MAIN);
    lv_obj_add_style(lfo_speed_arc, &style_arc_indicator, LV_PART_INDICATOR);
    lv_obj_remove_style(lfo_speed_arc, NULL, LV_PART_KNOB);
    lv_obj_t * speed_label = lv_label_create(lfo_speed_arc);
    lv_obj_set_align(speed_label, LV_ALIGN_CENTER);
    lv_label_set_text(speed_label, "speed");
	lv_obj_set_style_text_color(speed_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	lfo_amp_arc = lv_arc_create(scr_main);
	lv_obj_set_size(lfo_amp_arc, 110, 110);
	lv_obj_set_pos(lfo_amp_arc, 345, 240);
	lv_arc_set_min_value(lfo_amp_arc, 0);
	lv_arc_set_max_value(lfo_amp_arc, 127);
	lv_arc_set_value(lfo_amp_arc, 64);
	lv_obj_add_style(lfo_amp_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(lfo_amp_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(lfo_amp_arc, NULL, LV_PART_KNOB);
	lv_obj_t * amp_label = lv_label_create(lfo_amp_arc);
	lv_obj_set_align(amp_label, LV_ALIGN_CENTER);
	lv_label_set_text(amp_label, "amp");
	lv_obj_set_style_text_color(amp_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	lfo_phase_arc = lv_arc_create(scr_main);
	lv_obj_set_size(lfo_phase_arc, 110, 110);
	lv_obj_set_pos(lfo_phase_arc, 630, 240);
	lv_arc_set_min_value(lfo_phase_arc, 0);
	lv_arc_set_max_value(lfo_phase_arc, 16);
	lv_arc_set_value(lfo_phase_arc, 0);
	lv_obj_add_style(lfo_phase_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(lfo_phase_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(lfo_phase_arc, NULL, LV_PART_KNOB);
	lv_obj_t * phase_label = lv_label_create(lfo_phase_arc);
	lv_obj_set_align(phase_label, LV_ALIGN_CENTER);
	lv_label_set_text(phase_label, "phase");
	lv_obj_set_style_text_color(phase_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	// add events to dials
	lv_obj_add_event_cb(lfo_speed_arc, lfo_speed_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(lfo_amp_arc, lfo_amp_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(lfo_phase_arc, lfo_phase_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);



	// Add labels to display dial values
	lfo_speed_label = lv_label_create(scr_main);
	lv_obj_set_size(lfo_speed_label, 60, 40);
	lv_obj_align_to(lfo_speed_label, lfo_speed_arc, LV_ALIGN_CENTER, 0, 60);
	// lv_obj_set_pos(lfo_speed_label, 60, 350);
	// lv_obj_set_align(lfo_speed_label, LV_ALIGN_CENTER);
	lv_obj_set_style_text_color(lfo_speed_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);
	lv_obj_set_style_text_align(lfo_speed_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lfo_speed_label, "");

	lfo_phase_label = lv_label_create(scr_main);
	lv_obj_set_size(lfo_phase_label, 60, 40);
	lv_obj_align_to(lfo_phase_label, lfo_phase_arc, LV_ALIGN_CENTER, 0, 60);
	// lv_obj_set_pos(lfo_speed_label, 60, 350);
	// lv_obj_set_align(lfo_speed_label, LV_ALIGN_CENTER);
	lv_obj_set_style_text_color(lfo_phase_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);
	lv_obj_set_style_text_align(lfo_phase_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lfo_phase_label, "");

	// keep the LFO dials hidden in the first tab
	lv_obj_add_flag(lfo_speed_arc, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(lfo_amp_arc, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(lfo_phase_arc, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(lfo_speed_label, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(lfo_phase_label, LV_OBJ_FLAG_HIDDEN);
}



// static void GUI_lfoScreen()
// {
// 	scr_lfo = lv_obj_create(NULL);
//
// 	tabview_lfo = lv_tabview_create(scr_lfo);
// 	create_tabview(tabview_lfo);
// }


static void GUI_effectsScreen()
{
	scr_effects = lv_obj_create(NULL);

	tabview_effects = lv_tabview_create(scr_effects);
	create_tabview(tabview_effects);

	lv_obj_t * delay_time_arc;
	lv_obj_t * delay_fbk_arc;
	lv_obj_t * delay_drywet_arc;

	lv_obj_t * rvb_time_arc;
	lv_obj_t * rvb_fbk_arc;
	lv_obj_t * rvb_drywet_arc;

	/*Create trig area*/
	lv_obj_t * trigAreaEffects = lv_obj_create(scr_effects);
	lv_obj_add_style(trigAreaEffects, &trigArea_style, 0);
	lv_obj_remove_flag(trigAreaEffects, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_bg_color(trigAreaEffects, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
	lv_obj_set_pos(trigAreaEffects, 20, 480 - TRIGGERAREAHEIGHT_SMALL);
	lv_obj_set_size(trigAreaEffects, trigAreaWidth - 40, TRIGGERAREAHEIGHT_SMALL);

	/* Draw line separating effects */
	// lv_obj_t * middleLine = lv_line_create(scr_effects);
	// static lv_point_precise_t line_points[] = { {200, 220}, {600, 220} };
	// static lv_style_t style_line;
	//
	// lv_style_init(&style_line);
	// lv_style_set_line_width(&style_line, 2);
	// lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_GREY));
	// lv_style_set_line_rounded(&style_line, true);
	//
	// lv_line_set_points(middleLine, line_points, 2);     /*Set the points*/
	// lv_obj_add_style(middleLine, &style_line, 0);

	lv_obj_t * delayArea = lv_obj_create(scr_effects);
	lv_obj_add_style(delayArea, &trigArea_style, 0);
	lv_obj_remove_flag(delayArea, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_bg_color(delayArea, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
	lv_obj_set_style_opa(delayArea, 160,0);
	lv_obj_set_pos(delayArea, 20, 80);
	lv_obj_set_size(delayArea, trigAreaWidth - 40, 130);
	lv_obj_set_style_radius(delayArea, 16, 0);

	lv_obj_t * reverbArea = lv_obj_create(scr_effects);
	lv_obj_add_style(reverbArea, &trigArea_style, 0);
	lv_obj_remove_flag(reverbArea, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_style_bg_color(reverbArea, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
	lv_obj_set_style_opa(reverbArea, 160,0);
	lv_obj_set_pos(reverbArea, 20, 220);
	lv_obj_set_size(reverbArea, trigAreaWidth - 40, 130);
	lv_obj_set_style_radius(reverbArea, 16, 0);


	/*Create parameter dials*/
	delay_time_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(delay_time_arc, 110, 110);
	lv_obj_set_pos(delay_time_arc, 260, 95);
	lv_arc_set_min_value(delay_time_arc, 0);
	lv_arc_set_max_value(delay_time_arc, 127);
	lv_arc_set_value(delay_time_arc, 63);
	lv_obj_add_style(delay_time_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(delay_time_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(delay_time_arc, NULL, LV_PART_KNOB);
	lv_obj_t * delay_time_label = lv_label_create(delay_time_arc);
	lv_obj_set_align(delay_time_label, LV_ALIGN_CENTER);
	lv_label_set_text(delay_time_label, "time");
	lv_obj_set_style_text_color(delay_time_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	delay_fbk_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(delay_fbk_arc, 110, 110);
	lv_obj_set_pos(delay_fbk_arc, 445, 95);
	lv_arc_set_min_value(delay_fbk_arc, 0);
	lv_arc_set_max_value(delay_fbk_arc, 127);
	lv_arc_set_value(delay_fbk_arc, 63);
	lv_obj_add_style(delay_fbk_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(delay_fbk_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(delay_fbk_arc, NULL, LV_PART_KNOB);
	lv_obj_t * delay_fbk_label = lv_label_create(delay_fbk_arc);
	lv_obj_set_align(delay_fbk_label, LV_ALIGN_CENTER);
	lv_label_set_text(delay_fbk_label, "feedback");
	lv_obj_set_style_text_color(delay_fbk_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	delay_drywet_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(delay_drywet_arc, 110, 110);
	lv_obj_set_pos(delay_drywet_arc, 630, 95);
	lv_arc_set_min_value(delay_drywet_arc, 0);
	lv_arc_set_max_value(delay_drywet_arc, 127);
	lv_arc_set_value(delay_drywet_arc, 63);
	lv_obj_add_style(delay_drywet_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(delay_drywet_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(delay_drywet_arc, NULL, LV_PART_KNOB);
	lv_obj_t * delay_drywet_label = lv_label_create(delay_drywet_arc);
	lv_obj_set_align(delay_drywet_label, LV_ALIGN_CENTER);
	lv_label_set_text(delay_drywet_label, "dry/wet");
	lv_obj_set_style_text_color(delay_drywet_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	rvb_time_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(rvb_time_arc, 110, 110);
	lv_obj_set_pos(rvb_time_arc, 260, 235);
	lv_arc_set_min_value(rvb_time_arc, 0);
	lv_arc_set_max_value(rvb_time_arc, 127);
	lv_arc_set_value(rvb_time_arc, 63);
	lv_obj_add_style(rvb_time_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(rvb_time_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(rvb_time_arc, NULL, LV_PART_KNOB);
	lv_obj_t * rvb_time_label = lv_label_create(rvb_time_arc);
	lv_obj_set_align(rvb_time_label, LV_ALIGN_CENTER);
	lv_label_set_text(rvb_time_label, "time");
	lv_obj_set_style_text_color(rvb_time_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	rvb_fbk_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(rvb_fbk_arc, 110, 110);
	lv_obj_set_pos(rvb_fbk_arc, 445, 235);
	lv_arc_set_min_value(rvb_fbk_arc, 0);
	lv_arc_set_max_value(rvb_fbk_arc, 127);
	lv_arc_set_value(rvb_fbk_arc, 63);
	lv_obj_add_style(rvb_fbk_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(rvb_fbk_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(rvb_fbk_arc, NULL, LV_PART_KNOB);
	lv_obj_t * rvb_fbk_label = lv_label_create(rvb_fbk_arc);
	lv_obj_set_align(rvb_fbk_label, LV_ALIGN_CENTER);
	lv_label_set_text(rvb_fbk_label, "feedback");
	lv_obj_set_style_text_color(rvb_fbk_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	rvb_drywet_arc = lv_arc_create(scr_effects);
	lv_obj_set_size(rvb_drywet_arc, 110, 110);
	lv_obj_set_pos(rvb_drywet_arc, 630, 235);
	lv_arc_set_min_value(rvb_drywet_arc, 0);
	lv_arc_set_max_value(rvb_drywet_arc, 127);
	lv_arc_set_value(rvb_drywet_arc, 63);
	lv_obj_add_style(rvb_drywet_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(rvb_drywet_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(rvb_drywet_arc, NULL, LV_PART_KNOB);
	lv_obj_t * rvb_drywet_label = lv_label_create(rvb_drywet_arc);
	lv_obj_set_align(rvb_drywet_label, LV_ALIGN_CENTER);
	lv_label_set_text(rvb_drywet_label, "amount");
	lv_obj_set_style_text_color(rvb_drywet_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	// add labels
	lv_obj_t * delay_label = lv_label_create(scr_effects);
	lv_obj_align_to(delay_label, delay_time_arc,LV_ALIGN_CENTER, -220, -16);
	lv_obj_set_style_text_font(delay_label, &lv_font_montserrat_32, 0);
	lv_label_set_text(delay_label, "DELAY");
	lv_obj_set_style_text_color(delay_label, lv_palette_darken(LV_PALETTE_GREY, 1),0);

	lv_obj_t * reverb_label = lv_label_create(scr_effects);
	lv_obj_align_to(reverb_label, rvb_time_arc, LV_ALIGN_CENTER, -220, -16);
	lv_obj_set_style_text_font(reverb_label, &lv_font_montserrat_32, 0);
	lv_label_set_text(reverb_label, "REVERB");
	lv_obj_set_style_text_color(reverb_label, lv_palette_darken(LV_PALETTE_GREY, 1),0);


	// add events to dials
	lv_obj_add_event_cb(delay_time_arc, delay_time_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(delay_fbk_arc, delay_fbk_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(delay_drywet_arc, delay_drywet_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(rvb_time_arc, rvb_time_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(rvb_fbk_arc, rvb_fbk_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
	lv_obj_add_event_cb(rvb_drywet_arc, rvb_amount_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
}

static void GUI_settingsScreen()
{
	scr_settings = lv_obj_create(NULL);

	tabview_settings = lv_tabview_create(scr_settings);
	create_tabview(tabview_settings);


	lv_obj_t * volume_arc = lv_arc_create(scr_settings);
	lv_obj_set_size(volume_arc, 160, 160);
	lv_obj_align(volume_arc,LV_ALIGN_CENTER,0, 0);
	lv_arc_set_min_value(volume_arc, 0);
	lv_arc_set_max_value(volume_arc, 100);
	lv_arc_set_value(volume_arc, 80);
	lv_obj_add_style(volume_arc, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(volume_arc, &style_arc_indicator, LV_PART_INDICATOR);
	lv_obj_remove_style(volume_arc, NULL, LV_PART_KNOB);

	lv_obj_t * volume_label = lv_label_create(volume_arc);
	lv_obj_set_style_text_font(volume_label, &lv_font_montserrat_20, 0);
	lv_obj_set_align(volume_label, LV_ALIGN_CENTER);
	lv_label_set_text(volume_label, "volume");
	lv_obj_set_style_text_color(volume_label, lv_palette_lighten(LV_PALETTE_GREY, 2),0);

	lv_obj_add_event_cb(volume_arc, volume_event_cb, LV_EVENT_VALUE_CHANGED, lv_arc_get_value);
}


static void tabview_event_cb(lv_event_t * event)
{
	lv_obj_t * tabview = lv_event_get_target_obj(event);

	active_tab = lv_tabview_get_tab_active(tabview);


	switch ( active_tab )
	{
	default:
	case 0: // load main screen, adjust trigger pad area, display/hide objects, update tabviews
		lv_screen_load(scr_main);
		trigAreaHeight = TRIGGERAREAHEIGHT;
		trigAreaTop = 480 - trigAreaHeight;
		// trigAreaWidth = TRIGGERAREAWIDTH;
		// trigAreaX = TRIGGERAREA_X;
		lv_obj_set_y(trigArea, trigAreaTop);
		lv_obj_set_height(trigArea, trigAreaHeight);

		lv_obj_add_flag(lfo_speed_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(lfo_amp_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(lfo_phase_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(lfo_speed_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(lfo_phase_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(spectrum_a_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(spectrum_b_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(spectrum_a, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(spectrum_b, LV_OBJ_FLAG_HIDDEN);

		lv_tabview_set_active(tabview_main, 0, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 0, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_settings, 0, LV_ANIM_OFF);
		break;
	case 1: // load main screen, adjust trigger pad area, display/hide objects, update tabviews
		lv_screen_load(scr_main);
		trigAreaHeight = TRIGGERAREAHEIGHT_SMALL;
		trigAreaTop = 480 - trigAreaHeight;
		// trigAreaWidth = TRIGGERAREAWIDTH;
		// trigAreaX = TRIGGERAREA_X;
		lv_obj_set_y(trigArea, trigAreaTop);
		lv_obj_set_height(trigArea, trigAreaHeight);

		lv_obj_add_flag(spectrum_a_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(spectrum_b_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(spectrum_a, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(spectrum_b, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lfo_speed_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lfo_amp_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lfo_phase_arc, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lfo_speed_label, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(lfo_phase_label, LV_OBJ_FLAG_HIDDEN);

		lv_tabview_set_active(tabview_main, 1, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 1, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_settings, 1, LV_ANIM_OFF);
		break;
	case 2:
		lv_screen_load(scr_effects);
		trigAreaHeight = TRIGGERAREAHEIGHT_SMALL;
		trigAreaTop = 480 - trigAreaHeight;
		lv_tabview_set_active(tabview_main, 2, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 2, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_settings, 2, LV_ANIM_OFF);
		break;
	case 3:
		lv_screen_load(scr_settings);
		lv_tabview_set_active(tabview_main, 3, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_effects, 3, LV_ANIM_OFF);
		lv_tabview_set_active(tabview_settings, 3, LV_ANIM_OFF);
		break;
	}
}



static void spectrum_a_event_cb(lv_event_t * event)
{
	lv_obj_t * dropdown = lv_event_get_target_obj(event);
	uint8_t selection = lv_dropdown_get_selected(dropdown);

	switch ( selection )
	{
	default:
	case Bell1:
		spectrum_load(&spectrum, Bell1Partials, ExpAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Bell2:
		spectrum_load(&spectrum, Bell2Partials, ExpAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Gong:
		spectrum_load(&spectrum, GongPartials, ExpAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Chord:
		spectrum_load(&spectrum, ChordPartials, RampAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Saw:
		spectrum_load(&spectrum, SawPartials, SawAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Square:
		spectrum_load(&spectrum, SquarePartials, SquareAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case CB808:
		spectrum_load(&spectrum, CB808Partials, RampAmp, LEFT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	}
}

static void spectrum_b_event_cb(lv_event_t * event)
{
	lv_obj_t * dropdown = lv_event_get_target_obj(event);
	uint8_t selection = lv_dropdown_get_selected(dropdown);

	switch ( selection )
	{
	default:
	case Bell1:
		spectrum_load(&spectrum, Bell1Partials, ExpAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Bell2:
		spectrum_load(&spectrum, Bell2Partials, ExpAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Gong:
		spectrum_load(&spectrum, GongPartials, ExpAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Chord:
		spectrum_load(&spectrum, ChordPartials, RampAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Saw:
		spectrum_load(&spectrum, SawPartials, SawAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case Square:
		spectrum_load(&spectrum, SquarePartials, SquareAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	case CB808:
		spectrum_load(&spectrum, CB808Partials, RampAmp, RIGHT_SPECTRUM);
		filterbank_spectrumLoad(&filterbank, &spectrum);
		break;
	}
}


static void lfo_speed_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	lfo_speed = expf(10.0f * value - 2); // range of speed between ~0.1Hz and ~3kHz
	lv_label_set_text_fmt(lfo_speed_label, "%.02f Hz", lfo_speed);
}


static void lfo_amp_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	lfo_amp = value; // range between 0. and 1.
}

static void lfo_phase_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value *= 0.0625f;

	lfo_phaseShift = value; // range between 0. and 1. in 1/16 steps
	lv_label_set_text_fmt(lfo_phase_label, "%d/16", lv_arc_get_value(arc));
}



static void delay_time_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	// value /= 127.0f;
	delay_time = scale(0.0f, 127.0f, 300, 32767, value);

	// delay_time = (uint32_t)(value * 32767.0f);

}


static void delay_fbk_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	delay_feedback = value;
}


static void delay_drywet_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	delay_wet = value;
}


static void rvb_time_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	reverb_time = value;
}

static void rvb_fbk_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);

	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	reverb_feedback = value;
}

static void rvb_amount_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);

	float value = lv_arc_get_value(arc);

	value /= 127.0f;

	reverb_amount = value;
}


static void volume_event_cb(lv_event_t * event)
{
	lv_obj_t * arc = lv_event_get_target_obj(event);
	float value = lv_arc_get_value(arc);

	BSP_AUDIO_OUT_SetVolume(value);

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

		const int32_t height = (int32_t)(maxPartialHeight * amplitude);
		const int32_t xPos = (uint16_t)((spectrum.freqRatios[i] -1) * PARTIALSPACING + PARTIALSAREA_X);
		const int32_t yPos = PARTIALSAREA_Y + (maxPartialHeight - height);
		const int32_t opacity = 75 + 180.0f * amplitude; // opacity proportional to amplitude

		// display only if partial fits within the partials area
		if (xPos < PARTIALSAREA_X + PARTIALSAREAWIDTH)
		{
			lv_obj_set_pos(partial[i], xPos, yPos);
			lv_obj_set_height(partial[i], height);
			lv_obj_set_style_opa(partial[i], opacity, 0);
			lv_obj_remove_flag(partial[i], LV_OBJ_FLAG_HIDDEN);
		} else
		{
			lv_obj_add_flag(partial[i], LV_OBJ_FLAG_HIDDEN);
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
	lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "lfo");
	lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "effects");
	lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "settings");

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

	lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_add_event_cb(tabview, tabview_event_cb, LV_EVENT_VALUE_CHANGED, lv_tabview_get_tab_active);
}
