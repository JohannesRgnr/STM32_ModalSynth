/**
 * @file exciter.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 26.04.26
 * @note
 */

#include "../Inc/exciter.h"

#include "consts.h"




void Trigger_Note(line_t *line)
{
    line->val = 0.8f;
    line->dst = 0.0f;

    line->inc = (line->dst - line->val)/(0.02f * FS);
}


