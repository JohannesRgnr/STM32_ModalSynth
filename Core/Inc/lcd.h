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

void Display_Default(void);
void Display_Init(void);
void Display_partials(const float *freqRatios, const float *amps, uint32_t color);



#endif //MODAL_SYNTH_LCD_H