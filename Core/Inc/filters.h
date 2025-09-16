/**
 * @file filters.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 15.09.25
 * @note
 */

#ifndef MODAL_SYNTH_FILTERS_H
#define MODAL_SYNTH_FILTERS_H

typedef struct
{
    float old_value;

} onepoleLP_t;


float smoothingLP(onepoleLP_t *f, float sample,  float alpha);



#endif //MODAL_SYNTH_FILTERS_H