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

typedef struct {
    uint32_t addr[2];
    uint32_t width;
    uint32_t height;
    uint32_t front;
} Screen;

Screen* ct_screen_init();
void ct_screen_flip_buffers(Screen *screen);
uint32_t* ct_screen_backbuffer_ptr(Screen *screen);

uint32_t ct_screen_backbuffer_id(Screen *screen);

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
#define PARTIALSAREAWIDTH           (800 - 2 * PARTIALSAREA_X)
#define PARTIALSAREAHEIGHT          192
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





void Display_Default(void);
void Display_Init(void);
void Display_partials(spectrum_t *s);
void Display_morphBar(uint16_t x);
void clearTriggerArea(void);
void clearPartialsArea(void);

void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void drawMenu(void);

void swapBuffers(void);
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);

// void clearMorphArea(void);


#endif //MODAL_SYNTH_LCD_H