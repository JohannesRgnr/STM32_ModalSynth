/**
 * @file spectra.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#ifndef MINIMAL_SAI_SPECTRA_H
#define MINIMAL_SAI_SPECTRA_H

float Bell1Partials[16]= {0.5f, 0.5f, 1.0f, 1.0f, 1.183f, 1.183f, 1.506f, 1.506f, 2.0f, 2.514f, 2.662f, 3.011f, 4.166f, 5.433f, 6.796f, 8.215f}; // Bell 1 (Default)
float GongPartials[16]= {1.0f, 1.0f, 1.59f, 1.59f, 2.14f, 2.14f, 2.3f, 2.3f, 2.65f, 2.92f, 3.16f, 3.5f, 3.6f, 3.65f, 4.06f, 4.15f}; // Gong or Cymbal
float ChordPartials[16] = {1.0f, 1.2189f, 1.505f, 2.05f, 2.378f, 2.98f, 3.01f, 3.567f, 4.0f, 4.49f, 4.756f, 5.03f, 5.945f, 6.05f, 7.5f}; // Minor Chord
float SawPartials[16] = {0.99f, 1.0f, 1.01f, 1.98f, 2.0f, 2.02f, 3.0f, 3.03f, 3.97f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f , 9.0f, 10.0f}; // Sawtooth (sort of...)
float CB808Partials[16] = {0.5f, 0.7198f, 1.5f, 2.1593f, 2.5f, 3.5f, 3.5988f, 4.5f, 5.0383f, 5.5f, 6.4778f, 6.5f, 7.5f, 7.9174f, 9.3569f, 10.7964f}; // 808 Cowbell


#endif //MINIMAL_SAI_SPECTRA_H