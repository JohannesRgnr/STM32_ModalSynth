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

#include "anim/lv_example_anim.h"
#include "event/lv_example_event.h"
#include "get_started/lv_example_get_started.h"
#include "layouts/lv_example_layout.h"
#include "libs/lv_example_libs.h"
#include "others/lv_example_others.h"
#include "porting/osal/lv_example_osal.h"
#include "scroll/lv_example_scroll.h"
#include "styles/lv_example_style.h"
#include "widgets/lv_example_widgets.h"
#include "grad/lv_example_grad.h"

#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR

#define COLOR_BACKGROUND            0xFF000000
#define GREY_UI                     0xFFC0C0C0
#define COLOR_PAD                   0xFF707070
#define COLOR_TEXT_ACTIVE           0xFFCCCCCC
#define COLOR_TEXT_INACTIVE         0xFF707070
#define BLUE_PARTIALS               0x5983FC
#define BLUE_UI                     0xFF3E60C1
#define BLUE_UI_MAT                 0xFF2E4583
#define BLUE_UI_MATTER              0xFF293556
#define COLOR_PAD_TRANSP1           0x22707070
#define COLOR_PAD_TRANSP2           0x66707070
#define COLOR_PAD_TRANSP3           0xAA707070
#define ORANGE_UI                   0xCCEF8354
#define ORANGE_TEXT                 0xFFEF8354


#define PADDING                     8

#define PARTIALSAREA_X              32
#define PARTIALSAREA_Y              72
#define PARTIALSAREAWIDTH           BSP_LCD_GetXSize() - 2 * PARTIALSAREA_X
#define PARTIALSAREAHEIGHT          (BSP_LCD_GetYSize() / 2.5)
#define MAXPARTIALHEIGHT            (PARTIALSAREAHEIGHT - 3 * PADDING)

#define PARTIALSAREA_Left           PARTIALSAREA_X
#define PARTIALSAREA_Right          (PARTIALSAREA_X + PARTIALSAREAWIDTH)
#define PARTIALSAREA_Top            PARTIALSAREA_Y
#define PARTIALSAREA_Bottom         (PARTIALSAREA_Y + PARTIALSAREAHEIGHT + 64)

#define TRIGGERAREA_X               8
#define TRIGGERAREA_Y               280
#define TRIGGERAREAWIDTH            (BSP_LCD_GetXSize() - TRIGGERAREA_X - PADDING)
#define TRIGGERAREAHEIGHT           (BSP_LCD_GetYSize() - TRIGGERAREA_Y - PADDING)

#define TRIGGERAREA_Left            (TRIGGERAREA_X + 2 * PADDING)
#define TRIGGERAREA_Right           (TRIGGERAREAWIDTH - PADDING)
#define TRIGGERAREA_Top             (TRIGGERAREA_Y + 2 * PADDING)
#define TRIGGERAREA_Bottom          (TRIGGERAREA_Y + TRIGGERAREAHEIGHT - PADDING)

#define MENUBARHEIGHT               48
#define MENUBAR_ITEMS               5
#define ITEM_WIDTH                  (BSP_LCD_GetXSize() / MENUBAR_ITEMS)



void lv_oneSlider(void);

void lv_circle_anim();
void lv_partials_anim();
void GUI_displayPartials(spectrum_t *s);
void GUI_LCDProcess();

void Display_Default(void);
void Display_Init(void);
void Display_partials(spectrum_t *s);
void Display_morphBar(uint16_t x);
void clearTriggerArea(void);
void clearPartialsArea(void);
// void clearMorphArea(void);


#endif //MODAL_SYNTH_LCD_H