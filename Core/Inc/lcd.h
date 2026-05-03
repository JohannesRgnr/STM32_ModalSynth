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


#define BORDER                      8
#define PARTIALSAREAWIDTH           (BSP_LCD_GetXSize() / 2)
#define PARTIALSAREAHEIGHT          (BSP_LCD_GetYSize() / 2.5)
#define MAXPARTIALLENGTH            (PARTIALSAREAHEIGHT - 3 * BORDER)

void Display_Default(void);
void Display_Init(void);
void Display_partials(spectrum_t *s);
void clearTriggerArea(void);
void clearPartialsArea(void);


#endif //MODAL_SYNTH_LCD_H