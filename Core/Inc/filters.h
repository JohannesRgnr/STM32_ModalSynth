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


typedef struct
{
    float s1;
    float s2;
    float r;
    float cutoff;
    float g;
} ZDFLP_t;

float smoothingLP(onepoleLP_t *f, float sample, float alpha);
float SVF_BP_compute(reson_t *f, float sample);

void  SVF_LP_init(ZDFLP_t *filter);
float freq_to_g(float freq_hz); // calculate g coefficient for SVF
float SVF_LP_compute(ZDFLP_t *f, float sample);

#endif //MODAL_SYNTH_FILTERS_H