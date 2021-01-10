MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN            : origin = 0x000000, length = 0x000002
   RAMM0            : origin = 0x000123, length = 0x0002DD
   RAMD0            : origin = 0x00B000, length = 0x000800
   RESET            : origin = 0x3FFFC0, length = 0x000002

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x000121     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x0003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008   /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
   RAMD1           : origin = 0x00B800, length = 0x000800
   RAMLS0           : origin = 0x008000, length = 0x000800
   RAMLS1           : origin = 0x008800, length = 0x000800
   RAMLS2           : origin = 0x009000, length = 0x000800
   RAMLS3           : origin = 0x009800, length = 0x000800
   RAMLS4           : origin = 0x00A000, length = 0x000800
   RAMLS5            : origin = 0x00A800, length = 0x000800


   RAMGS       : origin = 0x00C000, length = 0x00FFF8       /* RAMGS0 through RAMGS15, combined into one block */
//   RAMGS15_RSVD : origin = 0x01BFF8, length = 0x000008    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
                                                            /* Only on F28379D, F28377D, F28375D devices. Remove line on other devices. */

   //EXTRA		   : origin = 0x03F800, length = 0x000800
   EXTRA2		   : origin = 0x049000, length = 0x001000
   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   //CANA_MSG_RAM     : origin = 0x049000, length = 0x000800
   //CANB_MSG_RAM     : origin = 0x04B000, length = 0x000800
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : > RAMD1 | RAMLS0 | RAMLS1 | RAMLS2 | RAMLS3 | RAMLS4 | RAMLS5 | EXTRA2 | RAMGS,     PAGE = 1
   .cinit           : > RAMGS,     PAGE = 1
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
   .stack           : > RAMM1,     PAGE = 1

#if defined(__TI_EABI__)
   .bss             : > RAMLS5 | RAMGS,    PAGE = 1
   .bss:output      : > RAMLS3,    PAGE = 1
   .init_array      : > RAMM0,     PAGE = 0
   .const           : > RAMLS5 | RAMGS,    PAGE = 1
   .data            : > RAMLS5 | RAMGS,    PAGE = 1
   .sysmem          : > RAMLS5 | RAMGS,    PAGE = 1
#else
   .pinit           : > RAMM0,     PAGE = 0
   .ebss            : > RAMLS0 | RAMLS1 | RAMLS2 | RAMLS3 | RAMLS4 | RAMLS5 | RAMGS,    PAGE = 1
   .econst          : > RAMLS5 | RAMGS,    PAGE = 1
   .esysmem         : > RAMLS5,    PAGE = 1
#endif

   //Filter_RegsFile  : > RAMGS,    PAGE = 1


   //ramgs0           : > RAMGS,    PAGE = 1
   //ramgs1           : > RAMGS,    PAGE = 1

#ifdef __TI_COMPILER_VERSION__
   #if __TI_COMPILER_VERSION__ >= 15009000
    .TI.ramfunc : {} > RAMM0,      PAGE = 0
   #else
    ramfuncs    : > RAMM0 PAGE = 0
   #endif
#endif


   /* The following section definitions are required when using the IPC API Drivers */

    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }


    /* The following section definition are for SDFM examples */
   //Filter1_RegsFile : > RAMGS, PAGE = 1, fill=0x1111
   //Filter2_RegsFile : > RAMGS, PAGE = 1, fill=0x2222
   //Filter3_RegsFile : > RAMGS, PAGE = 1, fill=0x3333
   //Filter4_RegsFile : > RAMGS, PAGE = 1, fill=0x4444
   //Difference_RegsFile : >RAMGS,   PAGE = 1, fill=0x3333

	// pragma defines for FFT
   windowedInputBlock : > RAMLS2 | RAMLS3 | RAMLS4 | RAMGS, PAGE = 1, ALIGN(1024) // input into FFT
   RFFToutBuff : > RAMGS, PAGE = 1, ALIGN(1024) // output of FFT
   //RFFTF32Coef : > RAMGS, PAGE = 1, ALIGN(1024) // coef's of FFT

   // pragma defines for IFFT
   CFFTinBuff : > RAMLS2 | RAMLS3 | RAMLS4 | RAMGS, PAGE = 1, ALIGN(2048) // input to IFFT
   outputBlock : > RAMGS, PAGE = 1, ALIGN(2048) // output of IFFT

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
