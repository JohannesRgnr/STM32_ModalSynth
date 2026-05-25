/**
 * @file lcd.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#ifndef MODAL_SYNTH_LCD_H
#define MODAL_SYNTH_LCD_H

#include "filterbank.h"
#include "stm32f769i_discovery_lcd.h"
/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

// #include "anim/lv_example_anim.h"
// #include "event/lv_example_event.h"
// #include "get_started/lv_example_get_started.h"
// #include "layouts/lv_example_layout.h"
// #include "libs/lv_example_libs.h"
// #include "others/lv_example_others.h"
// #include "porting/osal/lv_example_osal.h"
// #include "scroll/lv_example_scroll.h"
// #include "styles/lv_example_style.h"
// #include "widgets/lv_example_widgets.h"
// #include "grad/lv_example_grad.h"


#define PADDING                     8

#define TABRHEIGHT                  48

#define PARTIALSAREA_X              80
#define PARTIALSAREA_Y              100
#define PARTIALSAREAWIDTH           640.0f
#define PARTIALSAREAHWIDTH_small    320.0f
#define PARTIALSPACING              (PARTIALSAREAWIDTH / 12.0f)
#define PARTIALSPACING_small        (PARTIALSAREAHWIDTH_small / 12.0f)
#define PARTIALSAREAHEIGHT          160.0f
#define BIGPARTIALHEIGHT            100.0f
#define SMALLPARTIALHEIGHT          60.0f

#define PARTIALSAREA_Left           PARTIALSAREA_X
#define PARTIALSAREA_Right          (PARTIALSAREA_X + PARTIALSAREAWIDTH)
#define PARTIALSAREA_Right_small    (PARTIALSAREA_X + PARTIALSAREAHWIDTH_small)
#define PARTIALSAREA_Top            PARTIALSAREA_Y
#define PARTIALSAREA_Bottom         (PARTIALSAREA_Y + PARTIALSAREAHEIGHT)



#define TRIGGERAREA_X               0
#define TRIGGERAREA_Y               280
#define TRIGGERAREAWIDTH            800
#define TRIGGERAREAHEIGHT           200

#define TRIGGERAREA_Left            TRIGGERAREA_X
#define TRIGGERAREA_Right           TRIGGERAREAWIDTH
#define TRIGGERAREA_Top             TRIGGERAREA_Y
#define TRIGGERAREA_Bottom          (TRIGGERAREA_Y + TRIGGERAREAHEIGHT)

#define MENUBARHEIGHT               48
#define MENUBAR_ITEMS               5
#define ITEM_WIDTH                  (BSP_LCD_GetXSize() / MENUBAR_ITEMS)



void GUI_Init();
static void GUI_mainScreen();
// static void GUI_lfoScreen();
static void GUI_effectsScreen();
static void GUI_settingsScreen();

static void tabview_event_cb(lv_event_t * event);
static void spectrum_a_event_cb(lv_event_t * event);
static void spectrum_b_event_cb(lv_event_t * event);
static void lfo_speed_event_cb(lv_event_t * event);
static void lfo_amp_event_cb(lv_event_t * event);
static void lfo_phase_event_cb(lv_event_t * event);

void GUI_refreshPartials(lv_timer_t * timer);
void GUI_refreshMorphCursor(float x);
static void create_tabview(lv_obj_t * tabview);




#endif //MODAL_SYNTH_LCD_H
