/**
 * @file spectra.h
 * @author Johannes R.
 * @brief 
 * @version 
 * @date 13.09.25
 * @note
 */

#ifndef MODAL_SYNTH_SPECTRA_H
#define MODAL_SYNTH_SPECTRA_H

#define NUMPRESETS  7

typedef enum presets
{
    Bell1,
    Bell2,
    Gong,
    Chord,
    Saw,
    Square,
    CB808
} presets_t;

extern const float Bell1Partials[];     // Bell 1 (Default Left)
extern const float Bell2Partials[];     // Bell 2
extern const float GongPartials[];      // Gong or Cymbal
extern const float ChordPartials[];     // Minor Chord
extern const float SawPartials[];       // Sawtooth (sort of..... Default Right)
extern const float SquarePartials[];    // Square (sort of...)
extern const float CB808Partials[];     // 808 Cowbell

extern const float ConstAmp[];
extern const float RampAmp[];
extern const float SawAmp[];
extern const float SquareAmp[];
extern const float ExpAmp[];

#endif //MODAL_SYNTH_SPECTRA_H