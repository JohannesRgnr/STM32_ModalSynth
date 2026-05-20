/**
 * @file reverb.c
 * @author Johannes R, code originally from Tom Erbe
 * @brief 
 * @version 
 * @date 01.05.26
 * @note
 */

#include "../Inc/reverb.h"

#include "filters.h"



float float_buffer[AUDIO_BUF];
int16_t delay_buffer[DELAY_BUF];

float* float_ptr;
int16_t* short_ptr;

int32_t allPassWP0, allPassWP1, allPassWP2, allPassWP3;
int32_t delWP0, delWP1, delWP2, delWP3;

// knobs
float reverb_time;
// float lowPass0, lowPass1;
float del2Out, del4Out;
float reverb_feedback = 0.8;
float sixteenbitout;

// filters
smoothingLP_t smoothingLP1, smoothingLP2;


void reverb_Init(void) // Tom's code
{
    uint32_t i;

    /* clear the buffer */
    float_ptr = float_buffer;
    i = AUDIO_BUF;
    while (i-- > 0)
        *float_ptr++ = 0.0f;
    /* clear the buffer */
    short_ptr = delay_buffer;
    i = DELAY_BUF;
    while (i-- > 0)
        *short_ptr++ = 0;

    /* init the pointer */
    allPassWP0 = 0;
    allPassWP1 = 0;
    allPassWP2 = 0;
    allPassWP3 = 0;
    delWP0 = delWP1 = delWP2 = delWP3 = 0;
    reverb_time = 0.5f;
    reverb_feedback = 0.4f;
    del2Out = del4Out = 0.0f;
    sixteenbitout = 1.0f / 32767.0f;
}


/* adapted from Tom Erbe's Dattorro reverb implementation */
void reverb_process(float in, float* reverbLout, float* reverbRout)
{
    float sampData;
    float outL, outR;

    // delay left channel
    sampData = in + del4Out * reverb_feedback;
    if (sampData > 1.0f)
        sampData = 1.0f;
    if (sampData < -1.0f)
        sampData = -1.0f;
    sampData = allpass(0, &allPassWP0, sampData, 1345, 0.7f);
    delay_write(0, 6249, &delWP0, sampData);
    sampData = delay_read_linear(0, 6249, delWP0, reverb_time);


    sampData = smoothingLP(&smoothingLP1, sampData, 0.2f); // ~ 8kHz cutoff

    sampData = allpass(1345, &allPassWP1, sampData, 3936, 0.5f);
    sampData *= reverb_feedback;
    delay_write(6249, 4687, &delWP1, sampData);
    del2Out = delay_read_linear(6249, 4687, delWP1, reverb_time);


    sampData = in + del2Out * reverb_feedback;
    if (sampData > 1.0f)
        sampData = 1.0f;
    if (sampData < -1.0f)
        sampData = -1.0f;


    sampData = allpass(5281, &allPassWP2, sampData, 996, 0.7f);
    delay_write(10937, 6598, &delWP2, sampData);
    sampData = delay_read_linear(10937, 6598, delWP2, reverb_time);

    sampData = smoothingLP(&smoothingLP2, sampData, 0.32f); // ~ 8kHz cutoff

    sampData = allpass(6310, &allPassWP3, sampData, 2667, 0.5f);
    sampData *= reverb_feedback;
    delay_write(17535, 5512, &delWP3, sampData);
    del4Out = delay_read_linear(17535, 5512, delWP3, reverb_time);

    outL = delay_read_linear(0, 6249, delWP0, reverb_time * 0.0628f);
    outL += delay_read_linear(0, 6249, delWP0, reverb_time * 0.7043f);
    outL += delay_read_linear(6249, 4687, delWP1, reverb_time * 0.6304f);
    outL -= delay_read_linear(10937, 6598, delWP2, reverb_time * 0.4464f);
    outL -= delay_read_linear(17535, 5512, delWP3, reverb_time * 0.00596f);

    outR = delay_read_linear(10937, 6598, delWP2, reverb_time * 0.0788f);
    outR += delay_read_linear(10937, 6598, delWP2, reverb_time * 0.8134f);
    outR += delay_read_linear(17535, 5512, delWP3, reverb_time * 0.7176f);
    outR -= delay_read_linear(0, 6249, delWP0, reverb_time * 0.4996f);
    outR -= delay_read_linear(6249, 4687, delWP1, reverb_time * 0.0386f);

    *reverbLout = outL;
    *reverbRout = outR;
}


// allpass filter, non-interpolating
static float allpass(int32_t location, int32_t* writePos, float input, int32_t delaySamps, float gain) // Tom's code
{
    float readOut, writeIn;
    int32_t readPos;

    readPos = *(writePos);
    /* read from delay line */
    readOut = *(float_buffer + location + readPos);

    /* mix input and delayed with saturation */
    /* put data in */
    writeIn = input + readOut * gain;
    *(float_buffer + location + readPos) = writeIn;

    readPos++;
    readPos = (readPos == delaySamps) ? 0 : readPos;

    *writePos = readPos;

    readOut -= writeIn * gain;

    return readOut;
}


static void delay_write(int32_t location, int32_t delSize, int32_t* wPos, float input) // Tom's code
{
    int32_t writePos;

    writePos = *wPos;

    writePos--;
    if (writePos < 0)
        writePos += delSize;
    *wPos = writePos;
    *(delay_buffer + location + writePos) = (int16_t)(32767.0f * input);
}


static float delay_read_linear(int32_t location, int32_t delSize, int32_t wPos, float time) // Tom's code
{
    float timeSamps, rPosF, rFractF, rOut;
    int32_t rPos, rPosP1;
    float samp, sampP1;

    // if delaySize is n, n-1 puts the read pointer as far from the write as possible

    timeSamps = time * (float)(delSize - 2) + 1.0f;
    rPosF = (float)wPos + timeSamps;
    rPos = (int32_t)rPosF;
    rFractF = rPosF - (float)rPos;

    rPosP1 = rPos + 1;
    if (rPos < 0) rPos += delSize;
    if (rPosP1 < 0) rPosP1 += delSize;
    if (rPos >= delSize) rPos -= delSize;
    if (rPosP1 >= delSize) rPosP1 -= delSize;

    samp = (float)*(delay_buffer + location + rPos);
    sampP1 = (float)*(delay_buffer + location + rPosP1);

    rOut = samp + rFractF * (sampP1 - samp);
    rOut *= sixteenbitout;
    return rOut;
}
