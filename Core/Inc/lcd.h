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

#include "stm32f769i_discovery_lcd.h"

#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR

#define COLOR_BACKGROUND            0xFF000000
#define COLOR_ELEMENTS              0xFFC0C0C0
#define COLOR_PAD                   0xFF808080
#define COLOR_TEXT                  0xFFFFFFFF
#define COLOR_PARTIALS              0xFFC0C0C0

void Display_Default(void);
void Display_Init(void);
void Display_partials(const float *freqRatios, const float *amps, uint32_t color);



#endif //MODAL_SYNTH_LCD_H