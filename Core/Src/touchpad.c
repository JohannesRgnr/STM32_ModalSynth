/**
 * @file indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "touchpad.h"
#include "stm32f769i_discovery_ts.h"

#include "exciter.h"
#include "help_func.h"
#include "lcd.h"
#include "tft.h"
#include "filterbank.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void GUI_TSProcess(lv_indev_t * indev, lv_indev_data_t *data);
static void GUI_triggerArea(uint16_t x, uint16_t y, uint8_t state, lv_indev_data_t *data);
static void GUI_MorphArea(uint16_t x);
/**********************
 *  VARIABLES
 **********************/
static TS_StateTypeDef  TS_State;
static uint8_t wasTouched = 0;
extern line_t exciterAmp, freq;
extern filterbank_t filterbank;
extern spectrum_t spectrum;

extern int32_t trigAreaWidth;
extern int32_t trigAreaX;

// extern lv_timer_t * timer_partialDisplay;

extern uint8_t active_tab;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{
	BSP_TS_Init(TFT_HOR_RES, TFT_VER_RES);

	lv_indev_t * indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, GUI_TSProcess);
}






/**********************
 *   STATIC FUNCTIONS
 **********************/

static void GUI_TSProcess(lv_indev_t * indev, lv_indev_data_t *data)
{

	static int16_t last_x = 0;
	static int16_t last_y = 0;
	BSP_TS_GetState(&TS_State);

	if(TS_State.touchDetected != 0)
		{
			data->point.x = TS_State.touchX[0];
			data->point.y = TS_State.touchY[0];
			last_x = data->point.x;
			last_y = data->point.y;
			data->state = LV_INDEV_STATE_PR;

			// if inside trigger area
			if (active_tab < 2 && data->point.x > trigAreaX && data->point.x < (trigAreaWidth + trigAreaX) && data->point.y > TRIGGERAREA_Top  && data->point.y < TRIGGERAREA_Bottom)
			{
				data->continue_reading = true;
				GUI_triggerArea(data->point.x, data->point.y, wasTouched, data);
			}
			// if inside partials area
			else if (active_tab < 2 && data->point.y > 80 && data->point.y < 235)
			{
				GUI_MorphArea(data->point.x);
			}
		}
	else
		{
			data->point.x = last_x;
			data->point.y = last_y;
			data->state = LV_INDEV_STATE_REL;
		}

	wasTouched = TS_State.touchDetected;
}



/**
 * Trigger new note when touched, or glide frequency if already touched
 * @param x x touch coordinate
 * @param y y touch coordinate
 * @param state currently touched or not
 */
static void GUI_triggerArea(uint16_t x, uint16_t y, uint8_t state, lv_indev_data_t *data)
{
	if (state == 0)
	{
		if (LV_INDEV_STATE_PR == 1) // new single finger touch
		{
			// evaluate fundamental frequency
			float midiNote = scale(trigAreaX, trigAreaWidth + trigAreaX, 24, 90, x );
			midiNote = clip(midiNote, 24, 90);
			float frequency = mtof(midiNote);

			const float duration = scale(TRIGGERAREA_Bottom,TRIGGERAREA_Top, 5, 10, y);

			// immediately jump to frequency
			freq.val = frequency;
			freq.dst = frequency;
			filterbank.freq = frequency;
			filterbank.decay = duration;
			filterbank_update(&filterbank);
			Trigger_Note(&exciterAmp);
		}
	}
	else  // was already touched
	{
		if (LV_INDEV_STATE_PR == 1)
		{


			float midiNote = scale(trigAreaX, trigAreaWidth + trigAreaX, 24, 90, x);
			midiNote = clip(midiNote, 24, 90);
			const float duration = scale(TRIGGERAREA_Bottom,TRIGGERAREA_Top, 5, 10, y);
			freq.dst = mtof(midiNote);

			filterbank.decay = duration;
			filterbank_update(&filterbank);
		}
	}
}

/**
 * Do linear crossfade between 2 spectra, and display x position with cursor
 * @param x x coordinate of finger touch
 */
static void GUI_MorphArea(uint16_t x)
{
	x = clip(x, PARTIALSAREA_Left, PARTIALSAREA_Right);
	float xfade = scale( PARTIALSAREA_Left, PARTIALSAREA_Right, 0.0f, 1.0f, x);
	xfade = clip(xfade, 0.f, 1.f);

	spectrum_xfade(&spectrum, xfade);
	filterbank_spectrumLoad(&filterbank, &spectrum);
	filterbank_update(&filterbank);

	GUI_refreshMorphCursor(x);
}