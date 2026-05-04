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

#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR

#define COLOR_BACKGROUND            0xFF000000
#define GREY_UI                     0xFFC0C0C0
#define COLOR_PAD                   0xFF707070
#define COLOR_TEXT                  0xFFCCCCCC
#define BLUE_PARTIALS               0x5983FC
#define BLUE_UI                     0xFF3E60C1
#define BLUE_UI_MAT                 0xFF2E4583
#define BLUE_UI_MATTER              0xFF293556
#define COLOR_PAD_TRANSP1           0x22707070
#define COLOR_PAD_TRANSP2           0x66707070
#define COLOR_PAD_TRANSP3           0xAA707070
#define ORANGE_UI                   0xCCEF8354


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
#define TRIGGERAREA_Bottom          (TRIGGERAREA_Y + TRIGGERAREAHEIGHT - 2 * PADDING)

void Display_Default(void);
void Display_Init(void);
void Display_partials(spectrum_t *s);
void clearTriggerArea(void);
void clearPartialsArea(void);
// void clearMorphArea(void);


#endif //MODAL_SYNTH_LCD_H