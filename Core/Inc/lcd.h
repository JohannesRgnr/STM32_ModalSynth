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
#define COLOR_TEXT                  0xFFFFFFFF
#define BLUE_PARTIALS               0x5983FC
#define BLUE_UI                     0xFF3E60C1
#define BLUE_UI_MAT                 0xFF2E4583
#define BLUE_UI_MATTER              0xFF293556
#define COLOR_PAD_TRANSP            0xCC707070



#define PADDING                     8

#define PARTIALAREA_X               8
#define PARTIALAREA_Y               8
#define PARTIALSAREAWIDTH           (BSP_LCD_GetXSize() / 2)
#define PARTIALSAREAHEIGHT          (BSP_LCD_GetYSize() / 2.5)
#define MAXPARTIALHEIGHT            (PARTIALSAREAHEIGHT - PARTIALAREA_Y  - 2 * PADDING)

#define MORPHAREA_X                 8
#define MORPHAREA_Y                 (PARTIALSAREAHEIGHT + 2 * PADDING)
#define MORPHAREAWIDTH              PARTIALSAREAWIDTH
#define MORPHAREAHEIGHT             (TRIGGERAREA_Y - PARTIALSAREAHEIGHT - 3 * PADDING)
#define MORPHAREA_Left              MORPHAREA_X
#define MORPHAREA_Right             (MORPHAREA_X + MORPHAREAWIDTH)
#define MORPHAREA_Top               MORPHAREA_Y
#define MORPHAREA_Bottom            (MORPHAREA_Y + MORPHAREAHEIGHT)

#define TRIGGERAREA_X               8
#define TRIGGERAREA_Y               300
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


#endif //MODAL_SYNTH_LCD_H