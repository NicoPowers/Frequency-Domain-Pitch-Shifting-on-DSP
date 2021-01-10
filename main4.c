/**
 * Author: Nicolas Montoya
 * Implements Pitch Shifting in Frequency Domain (implements Ocean's Algorithm - 2010 paper by Nicolas Juillerat)
 */

#include <F28x_Project.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "LCD_Driver.h"
#include "device.h"
#include "gpio.c"
#include "interrupt.c"
#include "AIC23.h"
#include "fpu_rfft.h"
#include "fpu_cfft.h"
#include "Codec_Driver.h"

typedef struct
{
    float real;
    float imaginary;
} Complex;

/*
 * ***********************************
 * Defines for Reverb Schroeder Effect
 * ***********************************
 */

//Schroeder delays from 25k->96k interpolated
//*2 delay extension -> not possible without external ram
#define l_CB0 3460
#define l_CB1 2988
#define l_CB2 3882
#define l_AP0 480
#define l_AP1 161

//define wet 0.0 <-> 1.0
float wet = 1.0;
//define time delay 0.0 <-> 1.0 (max)
float time = 1.0;

//define pointer limits = delay time
int cf0_lim, cf1_lim, cf2_lim, ap0_lim, ap1_lim;

//define buffer for comb- and allpassfilters
float cfbuf0[l_CB0], cfbuf1[l_CB1], cfbuf2[l_CB2];
float apbuf0[l_AP0], apbuf1[l_AP1];
//feedback defines as of Schroeder
float cf0_g = 0.805, cf1_g = 0.827, cf2_g = 0.783, cf3_g = 0.764;
float ap0_g = 0.7, ap1_g = 0.7, ap2_g = 0.7;
//buffer-pointer
int cf0_p = 0, cf1_p = 0, cf2_p = 0, cf3_p = 0, ap0_p = 0, ap1_p = 0, ap2_p = 0;

/*
 * ******************************************
 * Defines for Rotary Encoder and LCD Display
 * ******************************************
 */

#define RESOLUTION 50
#define MODES 2
volatile bool pitchShiftMode = true;

volatile int rotaryPosition = RESOLUTION / 2;
volatile bool changed = true, pressed = false, reverb = false;

/*
 * *********************************************************
 * Defines for IIR Band Pass Filter (allows 250 HZ to 4 KHz)
 * *********************************************************
 */

#define STAGES 6
#define BUFFER_SIZE 2 // only need to store 2 delays per stage

float xBuffer_IN[BUFFER_SIZE * STAGES] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float yBuffer_IN[BUFFER_SIZE * STAGES] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float xBuffer_OUT[BUFFER_SIZE * STAGES] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float yBuffer_OUT[BUFFER_SIZE * STAGES] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

const float coeffs[6][6] = { { 0.0692, 0, -0.0692, 1, -1.7080, 0.8616 }, { 0.1501, 0, -0.1501, 1, -1.7972, 0.8427 }, { 0.1898, 0, -0.1898, 1, -1.9953, 0.9964 }, { 0.0186, 0, -0.0186, 1, -1.9825,
                                                                                                                                                                   0.9843 },
                             { 1.2086, 0, -1.2086, 1, -1.6879, 0.9464 }, { 0.2486, 0, -0.2486, 1, -1.9353, 0.9412 } };

/*
 * ******************************************
 * Defines for Ocean Pitch Shifting Algorithm
 * ******************************************
 */

#define TWO_PI 6.28318530718
#define blockSize 1024
#define overlap 4
#define zeroPad 2
#define hopSize blockSize / overlap
#define nbInputBins (blockSize / 2) + 1
#define nbOutputBins ((blockSize * zeroPad) / 2) + 1
#define cycleLength overlap * zeroPad

int16 processHopIndex = -(int16) (overlap - 1);
int16 outputHopIndex;
float pitchShiftRatio = 1.0;
float gain = 3.0;
float inputBlock[blockSize];

#pragma DATA_SECTION(windowedInputBlock,"windowedInputBlock") // input to FFT
float windowedInputBlock[blockSize];

float analysisWindow[blockSize];
float synthesisWindow[blockSize];
Complex unityRoots[cycleLength];
Complex inputSpectrum[nbInputBins];
Complex outputSpectrum[nbOutputBins];
#pragma DATA_SECTION(outputBlock,"outputBlock")
float outputBlock[blockSize * zeroPad];
float overlapAddBuffer[blockSize - hopSize];

/*
 * ******************************************
 * Defines for TI RFFT Structures
 * ******************************************
 */
#define RFFT_STAGES 10
#define RFFT_SIZE  (1 << RFFT_STAGES)

#pragma DATA_SECTION(RFFToutBuff,"RFFToutBuff")
float RFFToutBuff[RFFT_SIZE];
//#pragma DATA_SECTION(RFFTF32Coef,"RFFTF32Coef")
float RFFTF32Coef[RFFT_SIZE];

RFFT_F32_STRUCT rfft;

RFFT_F32_STRUCT_Handle hnd_rfft = &rfft;

/*
 * ******************************************
 * Defines for TI CFFT Structures
 * ******************************************
 */

#define CFFT_STAGES RFFT_STAGES
#define CFFT_SIZE  (1 << CFFT_STAGES)

#pragma DATA_SECTION(CFFTinBuff,"CFFTinBuff")
float CFFTinBuff[CFFT_SIZE * 2];
float CFFTF32Coef[CFFT_SIZE];

CFFT_F32_STRUCT icfft;

CFFT_F32_STRUCT_Handle hnd_icfft = &icfft;

volatile int16 pitchShiftedAudio[hopSize];
volatile Uint16 audioOutIndex = 0, audioInIndex = 0;

volatile int16 audioIn[hopSize];
int16 audioOut[hopSize];

float soundData[hopSize];

volatile bool fillingBuffer = true, dataReadyToPlay = false, dataReadyToProcess = false;

const float HannCoefs[2] = { 0.5, -0.25 };

float IIRFilter(float input, const float coeffs[][STAGES], float *xBuffer, float *yBuffer);
float getDemodulationWindow(int16 index, long hopIndex);
float getModifiedAnalysisWindowFloat(int16 index, long hopIndex, float pitchShiftRatio);
float getModifiedAnalysisWindowInteger(int16 index, long hopIndex, int16 pitchShiftRatioZP);
void fixInputSpectrum(float fftOutput[], Complex inputSpectrum[]);
void fixOutputSpectrum(Complex outputSpectrum[], float ifftInput[]);
void generateUnityRoots();
void fillHannWindow(float *window);
void fillArray(float src[], int16 srcPos, float dest[], int16 destPos, int16 length);
void processData(float data[]);
void processSpectrum(Complex inputSpectrum[], Complex outputSpectrum[]);
float Do_Comb0(float inSample);
float Do_Comb1(float inSample);
float Do_Comb2(float inSample);
float Do_Allpass0(float inSample);
float Do_Allpass1(float inSample);
float Do_Reverb(float inSample);

interrupt void MCBSP_RX_INT(void);
interrupt void A_ISR(void);
interrupt void B_ISR(void);
interrupt void SW_ISR(void);

Uint16 i = 0; // generic index used for debugging

int main(void)

{
    InitSysCtrl();
    // Init PIE
    InitPieCtrl();  // Initialize PIE -> disable PIE and clear all PIE registers
    InitPieVectTable(); // Initialize PIE vector table to known state

    outputHopIndex = processHopIndex;
    cf0_lim = (int) (time * l_CB0);
    cf1_lim = (int) (time * l_CB1);
    cf2_lim = (int) (time * l_CB2);
    ap0_lim = (int) (time * l_AP0);
    ap1_lim = (int) (time * l_AP1);

    LCD_init();

    fillHannWindow(analysisWindow);
    fillArray(analysisWindow, 0, synthesisWindow, 0, blockSize);
    generateUnityRoots();

    // setup TI RFFT
    RFFT_f32_setInputPtr(hnd_rfft, windowedInputBlock);
    RFFT_f32_setOutputPtr(hnd_rfft, RFFToutBuff);
    RFFT_f32_setStages(hnd_rfft, RFFT_STAGES);
    RFFT_f32_setFFTSize(hnd_rfft, RFFT_SIZE);
    RFFT_f32_setTwiddlesPtr(hnd_rfft, RFFTF32Coef);
    RFFT_f32_sincostable(hnd_rfft);

    // setup TI CFFT
    CFFT_f32_setInputPtr(hnd_icfft, CFFTinBuff);
    CFFT_f32_setOutputPtr(hnd_icfft, outputBlock);
    CFFT_f32_setStages(hnd_icfft, CFFT_STAGES);
    CFFT_f32_setFFTSize(hnd_icfft, CFFT_SIZE);
    CFFT_f32_setTwiddlesPtr(hnd_icfft, CFFTF32Coef);
    CFFT_f32_sincostable(hnd_icfft);

    Codec_driver_init();    // init Codec board
    Codec_driver_send_command(aaudpath()); // setup Codec to output Mic data to DSP
    Codec_driver_send_command(CLKsampleratecontrol(SR32)); // set sampling rate to 32 kHz

    // attach ISR to interrupts
    Interrupt_register(INT_MCBSPB_RX, &MCBSP_RX_INT);
    Interrupt_register(INT_XINT1, &A_ISR);
    Interrupt_register(INT_XINT2, &B_ISR);
    Interrupt_register(INT_XINT3, &SW_ISR);

    // enable XINT1,2,3 in the PIE
    Interrupt_enable(INT_XINT1);
    Interrupt_enable(INT_XINT2);
    Interrupt_enable(INT_XINT3);
    Interrupt_enable(INT_MCBSPB_RX);
    EINT;

    // P40 -> A, P41 -> B, P52 -> SW
    // make above pins input with pullup enabled
    GPIO_SetupPinOptions(40, 0, GPIO_PULLUP);
    GPIO_SetupPinOptions(41, 0, GPIO_PULLUP);
    GPIO_SetupPinOptions(52, 0, GPIO_PULLUP);

    // setup external interrupts for input pins
    GPIO_setInterruptPin(40, GPIO_INT_XINT1);
    GPIO_setInterruptPin(41, GPIO_INT_XINT2);
    GPIO_setInterruptPin(52, GPIO_INT_XINT3);

    // configure each interrupt as falling edge
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT3, GPIO_INT_TYPE_FALLING_EDGE);

    // enable XINT1,2,3 in GPIO
    GPIO_enableInterrupt(GPIO_INT_XINT1);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
    GPIO_enableInterrupt(GPIO_INT_XINT3);

    while ( 1 )
    {
        if ( dataReadyToProcess )
        {

            for (i = 0; i < hopSize; i++)
            {
                soundData[i] = IIRFilter((float) audioIn[i], coeffs, xBuffer_IN, yBuffer_IN);
            }

            processData(soundData); // pitch shift

            for (i = 0; i < hopSize; i++)
            {
                if ( !pitchShiftMode )
                    audioOut[i] = (int16) Do_Reverb(gain * IIRFilter((float) soundData[i], coeffs, xBuffer_OUT, yBuffer_OUT));
                else audioOut[i] = (int16) (gain * IIRFilter((float) soundData[i], coeffs, xBuffer_OUT, yBuffer_OUT));
            }

            dataReadyToPlay = true;
            dataReadyToProcess = false;
        }
        if ( changed || pressed )
        {

            if ( pitchShiftMode )
            {
                pitchShiftRatio = (float) ((1.5 / RESOLUTION) * rotaryPosition) + 0.50;
                if ( pitchShiftRatio > 2.0 )
                {
                    pitchShiftRatio = 2.0;
                    rotaryPosition = RESOLUTION;
                }

                LCD_clear();
                LCD_write_string("PS Ratio:");
                LCD_newline();
                LCD_write_float(pitchShiftRatio);

            }
            else
            {
                LCD_clear();
                LCD_write_string("Reverb Effect");
            }

            changed = false;
            // if button is currently pressed, wait for it to be released
            if ( pressed )
            {
                while ( !GPIO_ReadPin(52) )
                {
                };
                DELAY_US(100000); // debounce
                pressed = false;
            }

        }

    }

}

float IIRFilter(float input, const float coeffs[][STAGES], float *xBuffer, float *yBuffer)
{
    float filterOutput, filterInput;

    Uint16 nMinus1, nMinus2;

    filterInput = input;
    filterOutput = 0.0;
    nMinus1 = 0, nMinus2 = 0;

    for (int16_t stage = 0; stage < STAGES; ++stage)
    {
        // set up delay indexes
        nMinus1 = stage * 2;
        nMinus2 = nMinus1 + 1;

        // y[n] = b0*x[n] ...
        filterOutput = filterInput * coeffs[stage][0];

        // y[n] = ... + b1*x[n-1] ...
        filterOutput += xBuffer[nMinus1] * coeffs[stage][1];

        // y[n] = ... + b2*x[n-2] ...
        filterOutput += xBuffer[nMinus2] * coeffs[stage][2];

        // y[n] = ... - a1*y[n-1] ...
        filterOutput -= yBuffer[nMinus1] * coeffs[stage][4];

        // y[n] = ... - a2*y[n-2]
        filterOutput -= yBuffer[nMinus2] * coeffs[stage][5];

        // update prev input buffer for next time
        // x[n - 2] gets thrown away
        // x[n - 2] now equals x[n - 1]
        // x[n - 1] equals current x[n]

        xBuffer[nMinus2] = xBuffer[nMinus1];
        xBuffer[nMinus1] = filterInput;

        // update prev output buffer for next time
        // y[n - 2] gets thrown away
        // y[n - 2] now equals y[n - 1]
        // y[n - 1] equals current y[n]

        yBuffer[nMinus2] = yBuffer[nMinus1];
        yBuffer[nMinus1] = filterOutput;

        // x[n] = y[n]_n-1, previous stage's output sample, is this stage's current input sample
        // prev stage y[n] == current stage x[n]
        filterInput = filterOutput;
    }
    return filterOutput;

}

interrupt void A_ISR(void)

{
    if ( !changed )
    {
        bool a = GPIO_ReadPin(40);
        bool b = GPIO_ReadPin(41);
        if ( !a && b )
            rotaryPosition++;
        if ( rotaryPosition == RESOLUTION )
            rotaryPosition = RESOLUTION;

        changed = true;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void B_ISR(void)
{
    if ( !changed )
    {
        bool a = GPIO_ReadPin(40);
        bool b = GPIO_ReadPin(41);
        if ( a && !b )
            rotaryPosition--;
        if ( rotaryPosition < 0 )
            rotaryPosition = 0;

        changed = true;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void SW_ISR(void)
{
    if ( !pressed )
    {
        pitchShiftMode = !pitchShiftMode;
        pressed = true;
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void MCBSP_RX_INT(void)
{

    // fill up samples, then output them, there will be a small delay
    int16 currentSample = (int16) McbspbRegs.DRR2.all;
    int16 garbage = (int16) McbspbRegs.DRR1.all;

    audioIn[audioInIndex++] = currentSample;
    if ( audioInIndex == hopSize )
    {
        // we are done filling the audio in buffer
        audioInIndex = 0;
        dataReadyToProcess = true;
    }

    if ( dataReadyToPlay )
    {
        McbspbRegs.DXR2.all = audioOut[audioOutIndex];
        McbspbRegs.DXR1.all = audioOut[audioOutIndex];
        audioOutIndex++;
        if ( audioOutIndex == hopSize )
        {
            audioOutIndex = 0;
            dataReadyToPlay = false;
        }

    }

    PieCtrlRegs.PIEACK.bit.ACK6 = 1;

}

// this function is called everytime there is new data to process
void processData(float data[])
{

    // Shift previous block
    fillArray(inputBlock, hopSize, inputBlock, 0, blockSize - hopSize);

    // Append new hop
    fillArray(data, 0, inputBlock, blockSize - hopSize, hopSize);

    // apply analysis window
    for (Uint16 i = 0; i < blockSize; i++)
        windowedInputBlock[i] = inputBlock[i] * analysisWindow[i];

    // compute forward FFT on windowed input block, place result in input spectrum
    RFFT_f32(hnd_rfft);

    // transform TI fft output into the correct input spectrum for the algorithm
    fixInputSpectrum(RFFToutBuff, inputSpectrum);

    // Actual Ocean Pitch Shifting implementation
    processSpectrum(inputSpectrum, outputSpectrum);

    // transform output spectrum to TI cifft expected input buffer
    fixOutputSpectrum(outputSpectrum, CFFTinBuff);

    // compute backward FFT on fixed output spectrum
    ICFFT_f32(hnd_icfft);

    // apply synthesis window
    for (Uint16 i = 0; i < blockSize; i++)
        outputBlock[i] *= synthesisWindow[i];

    // Overlap and add
    {
        for (Uint16 i = 0; i < hopSize; i++)
            data[i] = (outputBlock[i] + overlapAddBuffer[i]) * getDemodulationWindow(i, outputHopIndex);

        // Shift overlap-added history
        fillArray(overlapAddBuffer, hopSize, overlapAddBuffer, 0, blockSize - hopSize * 2);

        // same as Arrays.fill(overlapAddBuffer, blockSize - hopSize * 2, blockSize - hopSize, 0.0f) in Java
        for (Uint16 i = blockSize - hopSize * 2; i <= blockSize - hopSize; i++)
            overlapAddBuffer[i] = 0.0;

        // Mix remaining hops of new block into overlap-added history
        for (Uint16 i = 0; i < blockSize - hopSize; i++)
            overlapAddBuffer[i] += outputBlock[i + hopSize];
    }

    // Increment counters
    processHopIndex++;
    outputHopIndex++;

    if ( processHopIndex == 32767 || outputHopIndex == 32767 )
    {
        processHopIndex = -(int16) (overlap - 1);
        outputHopIndex = processHopIndex;
    }
}

void processSpectrum(Complex inputSpectrum[], Complex outputSpectrum[])
{
    outputSpectrum[0] = inputSpectrum[0]; // copy DC signal
    // clear output bins
    for (Uint16 i = 1; i < nbOutputBins; i++)
    {
        outputSpectrum[i].real = 0.0;
        outputSpectrum[i].imaginary = 0.0;
    }
    Complex work;
    int16 cycleIndex = (int16) (processHopIndex + cycleLength * 2) % cycleLength;
    for (Uint16 srcBinIndex = 1; srcBinIndex < nbInputBins; srcBinIndex++)
    {
        int16 paddedSrcBinIndex = srcBinIndex * zeroPad;
        int16 dstBinIndex = (int16) ((float) paddedSrcBinIndex * pitchShiftRatio + 0.5); // b := floor(mka + 0.5) (Equation 4)
        if ( dstBinIndex > 0 && dstBinIndex < nbOutputBins )
        {
            // Copy source bin index
            work.real = inputSpectrum[srcBinIndex].real;
            work.imaginary = inputSpectrum[srcBinIndex].imaginary;

            // Compute phase shift
            int16 cycleShift;
            // Here we compute '(paddedSrcBinIndex - dstBinIndex) % cycleLength' with Euclidian modulo
            // In paper's notation: (b-ma) % mO
            // The modulo is not strictly necessary and is absent from the paper, but it helps the next step by keeping 0 <= cycleShift < cycleLength
            if ( dstBinIndex >= paddedSrcBinIndex )
                cycleShift = (dstBinIndex - paddedSrcBinIndex) % cycleLength;
            else cycleShift = cycleLength - (paddedSrcBinIndex - dstBinIndex) % cycleLength;

            int16 phaseShift = (cycleIndex * cycleShift) % cycleLength; // In paper's notation: (b-ma)p % mO
            // The modulo above is not strictly necessary and is absent from the paper, but it helps the next step by keeping 0 <= cycleShift < cycleLength
            if ( phaseShift != 0 )
            { // Apply phase shift
              // In paper's notation (equation 5, top of page 3): e^(-i2π( (b-ma)p % mO) / mO)
              // The paper has an extra 'N' denominator. Seems to be an error
                Complex unityRoot = unityRoots[(cycleLength - phaseShift) % cycleLength];
                float re = work.real * unityRoot.real - work.imaginary * unityRoot.imaginary;
                float im = work.real * unityRoot.imaginary + work.imaginary * unityRoot.real;
                work.real = re;
                work.imaginary = im;
            }

            // Accumulate into output spectrum
            outputSpectrum[dstBinIndex].real += work.real;
            outputSpectrum[dstBinIndex].imaginary += work.imaginary;
        }
    }

}

float getModifiedAnalysisWindowInteger(int16 index, long hopIndex, int16 pitchShiftRatioZP)
{
    int16 cycleIndex = (int16) (hopIndex + cycleLength * 2) % cycleLength;

    // Calculate 'pitchShiftRatio - zeroPad', modulo 'cycleLength', Euclidian modulo
    // With no zero padding (zeroPad == 1), this is 'pitchShiftRatio - 1', modulo 'overlap'
    int16 psrMinusPad = (pitchShiftRatioZP + (cycleLength - zeroPad)) % cycleLength;

    int16 shift = (int16) (((long) blockSize * cycleIndex * psrMinusPad) / cycleLength); // s * N
    int16 offset = (index * pitchShiftRatioZP / zeroPad + shift) % blockSize; // (k*t+s*N) % N
    return analysisWindow[offset];
}

float getModifiedAnalysisWindowFloat(int16 index, long hopIndex, float pitchShiftRatio)
{
    float paddedPitchShiftRatio = pitchShiftRatio * zeroPad;
    int16 floorPitchShiftRatio = (int16) paddedPitchShiftRatio;
    int16 ceilPitchShiftRatio = floorPitchShiftRatio + 1;
    float ceilAmount = (float) paddedPitchShiftRatio - floorPitchShiftRatio;
    float floorAmount = 1.0 - ceilAmount;

    float floorPsrWindow = getModifiedAnalysisWindowInteger(index, hopIndex, floorPitchShiftRatio);
    float ceilPsrWindow = getModifiedAnalysisWindowInteger(index, hopIndex, ceilPitchShiftRatio);
    // Linear interpolation:
    return floorPsrWindow * floorAmount + ceilPsrWindow * ceilAmount;
}

float getDemodulationWindow(int16 index, long hopIndex)
{
    float result = 0.0;

    // 'modified analysis window' * 'synthesis window', with overlap-add
    for (Uint16 k = 0; k < overlap; k++)
    {
        // Get the mixing offset of the 'hopIndex - k'-th output block:
        int16 offset = (k * hopSize) + index;

        // Accumulate values for all overlapped output blocks
        result += getModifiedAnalysisWindowFloat(offset, hopIndex - k, pitchShiftRatio) * synthesisWindow[offset];
    }
    return 1.0 / result;
}

void fixInputSpectrum(float fftOutput[], Complex inputSpectrum[])
{
    // this algorithm assumes FFT is taken over [0..N-1] where N = blockSize
    // so we need to take TI's output:
    //   OutBuf[0] = real[0]
    //   OutBuf[1] = real[1]
    //   OutBuf[2] = real[2]
    //   ………
    //   OutBuf[N/2] = real[N/2]
    //   OutBuf[N/2+1] = imag[N/2-1]
    //   ………
    //   OutBuf[N-3] = imag[3]
    //   OutBuf[N-2] = imag[2]
    //   OutBuf[N-1] = imag[1]
    // and convert this to a spectrum of complex numbers of nbInputBin size
    for (Uint16 i = 0; i < nbInputBins; i++)
    {
        inputSpectrum[i].real = fftOutput[i];
        inputSpectrum[i].imaginary = fftOutput[blockSize - i];
    }
    inputSpectrum[0].imaginary = 0.0; // DC has no phase
    inputSpectrum[nbInputBins - 1].imaginary = 0.0;
    inputSpectrum[nbInputBins - 2].imaginary = fftOutput[nbInputBins]; // middle edge case
}

void fixOutputSpectrum(Complex outputSpectrum[], float ifftInput[])
{
    // our output spectrum should look like
    // outputSpectrum[0] = (real, imaginary)
    // we need to convert to:
    //     CFFTinBuff[0] = real[0]
    //     CFFTinBuff[1] = imag[0]
    //     CFFTinBuff[2] = real[1]
    //     ………
    //     CFFTinBuff[N] = real[N/2]
    //     CFFTinBuff[N+1] = imag[N/2]
    //     ………
    //     CFFTinBuff[2N-3] = imag[N-2]
    //     CFFTinBuff[2N-2] = real[N-1]
    //     CFFTinBuff[2N-1] = imag[N-1]
    for (Uint16 i = 0; i < nbOutputBins; i++)
    {
        ifftInput[2 * i] = outputSpectrum[i].real;
        ifftInput[(2 * i) + 1] = outputSpectrum[i].imaginary;
    }
}

void generateUnityRoots()
{
    float cosInc = cos(TWO_PI / cycleLength);
    float sinInc = sin(TWO_PI / cycleLength);
    unityRoots[0].real = 1.0;
    unityRoots[0].imaginary = 0.0;
    float lre = 1.0;
    float lim = 0.0;
    for (Uint16 i = 1; i < cycleLength; i++)
    {
        float re = cosInc * lre - sinInc * lim;
        float im = sinInc * lre + cosInc * lim;
        lre = re;
        lim = im;
        unityRoots[i].real = re;
        unityRoots[i].imaginary = im;
    }
}

void fillArray(float src[], int16 srcPos, float dest[], int16 destPos, int16 length)
{
    Uint16 j = 0;
    for (Uint16 i = srcPos; i < srcPos + length; i++)
    {
        dest[destPos + j++] = src[i];
    }
}

void fillHannWindow(float *window)
{
    for (Uint16 i = 0; i < blockSize; i++)
    {
        float x = TWO_PI * (float) i / (float) blockSize;
        float val = HannCoefs[0];
        for (Uint16 c = 1; c < 2; c++)
            val += (HannCoefs[c] * 2.0 * cos(x * (float) c));
        window[i] = (float) val;
    }
}

float Do_Comb0(float inSample)
{
    float readback = cfbuf0[cf0_p];
    float newS = readback * cf0_g + inSample;
    cfbuf0[cf0_p] = newS;
    cf0_p++;
    if ( cf0_p == cf0_lim )
        cf0_p = 0;
    return readback;
}

float Do_Comb1(float inSample)
{
    float readback = cfbuf1[cf1_p];
    float newS = readback * cf1_g + inSample;
    cfbuf1[cf1_p] = newS;
    cf1_p++;
    if ( cf1_p == cf1_lim )
        cf1_p = 0;
    return readback;
}
float Do_Comb2(float inSample)
{
    float readback = cfbuf2[cf2_p];
    float newS = readback * cf2_g + inSample;
    cfbuf2[cf2_p] = newS;
    cf2_p++;
    if ( cf2_p == cf2_lim )
        cf2_p = 0;
    return readback;
}

float Do_Allpass0(float inSample)
{
    float readback = apbuf0[ap0_p];
    readback += (-ap0_g) * inSample;
    float newS = readback * ap0_g + inSample;
    apbuf0[ap0_p] = newS;
    ap0_p++;
    if ( ap0_p == ap0_lim )
        ap0_p = 0;
    return readback;
}

float Do_Allpass1(float inSample)
{
    float readback = apbuf1[ap1_p];
    readback += (-ap1_g) * inSample;
    float newS = readback * ap1_g + inSample;
    apbuf1[ap1_p] = newS;
    ap1_p++;
    if ( ap1_p == ap1_lim )
        ap1_p = 0;
    return readback;
}

float Do_Reverb(float inSample)
{
    float newsample = (Do_Comb0(inSample) + Do_Comb1(inSample) + Do_Comb2(inSample)) / 3.0;
    newsample = Do_Allpass0(newsample);
    newsample = Do_Allpass1(newsample);

    return newsample;
}
