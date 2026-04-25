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

typedef struct
{
    float s1;
    float s2;
    float r;
    float g;
}	reson_t;


float smoothingLP(onepoleLP_t *f, float sample,  float alpha);
float resonBP(reson_t *f, float sample);


#endif //MODAL_SYNTH_FILTERS_H