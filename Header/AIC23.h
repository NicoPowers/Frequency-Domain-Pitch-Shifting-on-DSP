// TI File $Revision:  $
// Checkin $Date:  $
//###########################################################################
//
// FILE:    AIC23.h
//
// TITLE:   TLV320AIC23B Driver Register and Bit Definitions
//
//###########################################################################
// $TI Release:   $
// $Release Date:   $
//###########################################################################

#ifndef _AIC_23_H
#define _AIC_23_H

#include <F2837xD_device.h>

#define SmallDelay() for(volatile long  i = 0; i < 500000; i++);// -> MOVED TO AIC23.h

/* TLV320AIC23B Register Map */
#define LLINVCTL    0x0000 // Left line input channel volume control
#define RLINVCTL    0x0200 // Right line input channel volume control
#define LHVCTL      0x0400 // Left channel headphone volume control
#define RHVCTL      0x0600 // Right channel headphone volume control
#define AAUDCTL     0x0800 // Analog audio path control
#define DAUDCTL     0x0A00 // Digital audio path control
#define POWDCTL     0x0C00 // Power down control
#define DAUDINTF    0x0E00 // Digital audio interface format
#define SMPLRCTL    0x1000 // Sample rate control
#define DIGINTFACT  0x1200 // Digital interface activation
#define RESETREG    0x1E00 // Reset register

/* TLV320AIC23B Register Bits */


/* Left line input channel volume control */
#define LRS         0x0100 // Left/right simultaneous volume/mute update
#define LIM         0x0080 // Left line input mute

/* Right line input channel volume control */
#define RLS         0x0100 // Right/left simultaneous volume/mute update
#define RIM         0x0080 // Right line input mute

/* Left channel headphone volume control */
#define LRS         0x0100 // Left/right simultaneous volume/mute update
#define LZC         0x0080 // Right channel Zero-cross detect

/* Right channel headphone volume control */
#define RLS         0x0100 // Right/left simultaneous volume/mute update
#define RZC         0x0080 // Left channel Zero-cross detect

/* Analog audio path control - default 0x0010 */
#define DB_0_ST      0x0120 //  0  dB Added Sidetone
#define DB_N6_ST     0x0020 // -6  dB Added Sidetone
#define DB_N9_ST     0x0060 // -9  dB Added Sidetone
#define DB_N12_ST    0x00A0 // -12 dB Added Sidetone
#define DB_N18_ST    0x00E0 // -18 dB Added Sidetone
#define STDIS        0x0000 // Added Sidetone Disabled
#define DAC          0x0010 // DAC select
#define BYPASS       0x0008 // Bypass Mode
#define MICINSEL     0x0004 // MIC input select for ADC
#define MICMUTE      0x0002 // Microphone Mute
#define MICBOOST     0x0001 // Microphone Boost

/* Digital audio path control - default 0x0008 */
#define DACM         0x0008 // DAC soft mute enabled
#define DEEMP48      0x0006 // 48 kHz de-emphasis control
#define DEEMP44_1    0x0004 // 44.1 kHz de-emphasis control
#define DEEMP32      0x0002 // 32 kHz de-emphasis control
#define ADCHP        0x0001 // Disables ADC high-pass filter


/* Power down control - default 0x0007 */
#define AIC23OFF     0x0080 // Device Power Off
#define AIC23CLK     0x0040 // Clock Off
#define AIC23OSC     0x0020 // Oscillator Off
#define AIC23OUT     0x0010 // Outputs Off
#define AIC23DAC     0x0008 // DAC Off
#define AIC23ADC     0x0004 // ADC Off
#define AIC23MIC     0x0002 // Microphone input Off
#define AIC23LINE    0x0001 // Line input Off

/* Digital audio interface format */
#define AIC23MS      0x0040 // Master Mode
#define LRSWAP       0x0020 // DAC Left/Right Swap
#define LRP          0x0010 // DAC Left/Right Phase = R-channel LRCIN high (DSP mode: MSB on 2nd BCLK rising edge)
#define LRPZRO       0x0000 // DAC Left/Right Phase = R-channel LRCIN low  (DSP mode: MSB on 1st BCLK rising edge)
#define FORMSBFRA    0x0000 // Format MSB first, Right-aligned
#define FORMSBFLA    0x0001 // Format MSB first, Left-aligned
#define FORI2S       0x0002 // I2S Format
#define FORDSP       0x0003 // DSP format, frame sync followed by 2 data words
#define IWL16B       0x0000 // Input bit length - 16 bits
#define IWL20B       0x0004 // Input bit length - 20 bits
#define IWL24B       0x0008 // Input bit length - 24 bits
#define IWL32B       0x000C // Input bit length - 32 bits


/* Sample rate control */
#define AIC23CLKIND  0x0040 // Clock Input Divider (MCLK/2)
#define AIC23CLKOUTD 0x0080 // Clock Output Divider (MCLK/2)
#define BOSR         0x0002 // Base Oversampling Rate (USB 272 fs/ Normal 384 fs)
#define USB          0x0001 // USB clock

/* Digital interface activation */
#define ACT          0x0001 // Activate interface

/* Reset register */
#define RES          0x0000 // Reset AIC23



/* Useful Definitions */
// Sample Rates:
#define SRUSB44_1    0x0022 // 44.1 kHz USB, BOSR set
#define SR48         0x0000 // 48 kHz USB/normal, BOSR clear
#define SR44_1       0x0020 // 44.1 kHz normal - 11.2896 MHz MCLK, BOSR clear
#define SR32         0x0018 // 32 kHz normal, BOSR clear
#define SR8          0x000C // 8 kHz


// Volume Control:
#define LIV         0x0017 // Left line input volume control
                           // 11111=12 dB down to 00000= -34.5 dB
                           // at 1.5 dB per step
                           // Default = 10111 = 0dB
#define RIV         0x0017 // Right line input volume control
                           // 11111=12 dB down to 00000= -34.5 dB
                           // at 1.5 dB per step
                           // Default = 10111 = 0dB
#define LHV         0x0069 // Left Channel Headphone Volume Control
                           // 1111111 = 6 dB, 79 steps between +6 and -73 dB (mute)
                           // 0110000 = -73 dB (mute)
                           // Currently set at 1101001 = -16 dB
#define RHV         0x0069 // Right Channel Headphone Volume Control
                           // 1111111 = 6 dB, 79 steps between +6 and -73 dB (mute)
                           // 0110000 = -73 dB (mute)
                           // Currently set at 1101001 = -16 dB





// AIC23.c External Function Prototypes
extern Uint16 softpowerdown (void);
extern Uint16 fullpowerup (void);
extern Uint16 nomicpowerup (void);
extern Uint16 linput_volctl (Uint16 volume);
extern Uint16 rinput_volctl (Uint16 volume);
extern Uint16 lhp_volctl (Uint16 volume);
extern Uint16 rhp_volctl (Uint16 volume);
extern Uint16 nomicaaudpath (void);
extern Uint16 aaudpath (void);
extern Uint16 digaudiopath (void);
extern Uint16 DSPdigaudinterface (void);
extern Uint16 I2Sdigaudinterface (void);
extern Uint16 USBsampleratecontrol (Uint16 ratecontrol);
extern Uint16 CLKsampleratecontrol (Uint16 ratecontrol);
extern Uint16 digact (void);
extern Uint16 reset (void);


#endif

//===========================================================================
// End of file.
//===========================================================================
