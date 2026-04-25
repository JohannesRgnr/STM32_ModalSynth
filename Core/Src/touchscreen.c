/**
 * @file touchscreen.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 24.04.26
 * @note
 */

#include "../Inc/touchscreen.h"



uint16_t audioLevel = INITIAL_VOLUME;
TS_StateTypeDef  TS_State;

void Touchscreen(void){

    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch */
    {
        uint16_t x = TS_State.touchX[0];
        uint16_t y = TS_State.touchY[0];


        if (y < 50 && x < 50)
        {
            // /* Increase volume by 1% */
            if (audioLevel < 99)
                audioLevel += 1;
            else
                audioLevel = 100;
            // // sprintf((char*)FreqStr, "VOL: %3lu  ", SoundVolume);
            BSP_AUDIO_OUT_SetVolume(audioLevel);
            // Display_Default();
            // BSP_LCD_DisplayStringAt(230, BSP_LCD_GetYSize() - 16, (uint8_t *)FreqStr, LEFT_MODE);
        }
        else if (y > (BSP_LCD_GetYSize() - 50) && x < 50 )
        {
            // // Display_Default();
            // /* Decrease volume by 1% */
            if (audioLevel > 1)
                audioLevel -= 1;
            else
                audioLevel = 0;
            // // sprintf((char*)FreqStr, "VOL: %3lu  ", SoundVolume);
            BSP_AUDIO_OUT_SetVolume(audioLevel);
            // BSP_LCD_DisplayStringAt(230, BSP_LCD_GetYSize() - 16, (uint8_t *)FreqStr, LEFT_MODE);
        }
    }
}