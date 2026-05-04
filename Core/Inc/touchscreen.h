/**
 * @file touchscreen.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 24.04.26
 * @note
 */

#ifndef MODAL_SYNTH_TOUCHSCREEN_H
#define MODAL_SYNTH_TOUCHSCREEN_H

#include "stm32f769i_discovery_ts.h"
#include "stm32f769i_discovery.h"




void Touchscreen(void);
static void ts_triggerArea(uint16_t x, uint16_t y, uint8_t state);
static void ts_MorphArea(uint16_t x, uint16_t y, uint8_t state);
static void ts_MenuArea(uint16_t x, uint8_t state);

static void inc_Preset(uint8_t leftOrRight);

#endif //MODAL_SYNTH_TOUCHSCREEN_H
