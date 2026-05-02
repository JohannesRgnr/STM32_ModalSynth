/**
 * @file spectra.c
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 27.04.26
 * @note
 */

const float Bell1Partials[16]= {0.5f, 0.5f, 1.0f, 1.0f, 1.183f, 1.183f, 1.506f, 1.506f, 2.0f, 2.514f, 2.662f, 3.011f, 4.166f, 5.433f, 6.796f, 8.215f}; // Bell 1 (Default)
const float GongPartials[16]= {1.0f, 1.01f, 1.59f, 1.582f, 2.14f, 2.143f, 2.3f, 2.308f, 2.65f, 2.92f, 3.16f, 3.5f, 3.6f, 3.65f, 4.06f, 4.15f}; // Gong or Cymbal
const float ChordPartials[16] = {1.0f, 1.2189f, 1.505f, 2.05f, 2.378f, 2.98f, 3.01f, 3.567f, 4.0f, 4.49f, 4.756f, 5.03f, 5.945f, 6.05f, 7.5f}; // Minor Chord
const float SawPartials[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f , 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f}; // Sawtooth (sort of...)
const float SquarePartials[16] = {1.0f, 3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f, 21.0f, 23.0f, 25.0f, 27.0f, 29.0f, 31.0f}; // Square (sort of...)
const float CB808Partials[16] = {0.5f, 0.7198f, 1.5f, 2.1593f, 2.5f, 3.5f, 3.5988f, 4.5f, 5.0383f, 5.5f, 6.4778f, 6.5f, 7.5f, 7.9174f, 9.3569f, 10.7964f}; // 808 Cowbell

const float ConstAmp[16] = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
const float RampAmp[16] = {1.000f, 0.942f, 0.884f, 0.826f, 0.768f, 0.710f, 0.652f, 0.594f, 0.535f, 0.477f, 0.419f, 0.361f, 0.303f, 0.245f, 0.187f, 0.129f};
const float SawAmp[16] = {1.0f, 0.5f, 0.333333f, 0.25f, 0.2f, 0.166667f, 0.142857f, 0.125f, 0.111111f, 0.1f, 0.090909f, 0.083333f, 0.076923f, 0.071429f, 0.066667f, 0.0625f};
const float SquareAmp[16] = {1.f, 0.333333f, 0.2f, 0.142857f, 0.111111f, 0.090909f, 0.076923f, 0.066667f, 0.058824f, 0.052632f, 0.047619f, 0.043478f, 0.04f, 0.037037f, 0.034483f, 0.032258f};
const float ExpAmp[16] = {1.000f, 0.771f, 0.677f, 0.604f, 0.543f, 0.489f, 0.440f, 0.395f, 0.353f, 0.314f, 0.277f, 0.242f, 0.208f, 0.176f, 0.145f, 0.115f};