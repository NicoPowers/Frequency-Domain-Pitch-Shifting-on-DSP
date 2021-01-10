/*
 * OneToOneI2CDriver.c
 *
 *  Created on: Sep 24, 2016
 *      Author: Raz Aloni
 */

#include <F28x_Project.h>
#include <F2837xD_Device.h>
#include "OneToOneI2CDriver.h"

/* Ideal module clock frequency for I2C */
static const Uint16 IdealModClockFrqMHz = 12;

/*
 * <summary>
 * 	Initializes the GPIO for the I2C
 * </summary>
 */
static void InitI2CGpio();

/*
 * <summary>
 * 	Calculates and sets the ClockDivides for the I2C Module
 * </summary>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
static inline void SetClockDivides(float32 sysClkMHz, float32 I2CClkKHz);

/*
 * <summary>
 * 	Initializes the I2C to run in Master Mode for a One-To-One connection
 * </summary>
 * <param="slaveAddress">Address of the slave device to write to</param>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
void I2C_O2O_Master_Init(Uint16 slaveAddress, float32 sysClkMhz, float32 I2CClkKHz)
{
	// Init GPIO
	InitI2CGpio();

	EALLOW;

	// Enable Clock for I2C
	CpuSysRegs.PCLKCR9.bit.I2C_A = 1;

	// Put I2C into Reset Mode
	I2caRegs.I2CMDR.bit.IRS = 0;

	// Set Slave Address
	I2caRegs.I2CSAR.bit.SAR = slaveAddress;

	// Set Clocks
	SetClockDivides(sysClkMhz, I2CClkKHz);

	// Release from Reset Mode
	I2caRegs.I2CMDR.bit.IRS = 1;

	EDIS;
}


/*
 * <summary>
 * 	Sends bytes via I2C
 * </summary>
 * <param="values">Pointer to array of bytes to send</param>
 * <param-"length">Length of array</param>
 */
void I2C_O2O_SendBytes(Uint16 * const values, Uint16 length)
{
	// Set to Master, Repeat Mode, TRX, FREE, Start
	I2caRegs.I2CMDR.all = 0x66A0;

	while(I2caRegs.I2CMDR.bit.STT){}; //wait for start condition to be cleared

	// Write values to I2C
	for (Uint16 i = 0; i < length; i++)
	{
		// Wait if Transmit is not ready
		while(!I2caRegs.I2CSTR.bit.XRDY);
		I2caRegs.I2CDXR.bit.DATA = values[i];


	}

	// Stop Bit
	I2caRegs.I2CMDR.bit.STP = 1;

}

/*
 * <summary>
 * 	Calculates and sets the ClockDivides for the I2C Module
 * </summary>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
static inline void SetClockDivides(float32 sysClkMHz, float32 I2CClkKHz)
{
	/* Calculate Module Clock Frequency - Must be between 7-12 MHz
	 * Module Clock Frequency = sysClkMhz/(IPSC + 1)
	 */
	Uint16 IPSC = (Uint16)(sysClkMHz/IdealModClockFrqMHz);

	/* Calculate Divide Downs for SCL
	 * FreqMClk = sysClkMHz/((IPSC + 1)[(ICCL + d) + (ICCH + d)])
	 *
	 * Assume an even clock size -> ICCH == ICCL
	 * ICCL = ICCH = sysclkMHz/(2000 * I2CClkKHz * (IPSC + 1)) - d
	 */

	// Find value for d
	Uint16 d = 5;

	if (IPSC < 2)
	{
		d++;
		if (IPSC < 1)
		{
			d++;
		}
	}

	Uint16 ICCLH = (Uint16)(1000 * sysClkMHz/(2 * I2CClkKHz * (IPSC + 1)) - d);

	// Set values
	I2caRegs.I2CPSC.all = IPSC;
	I2caRegs.I2CCLKL = ICCLH;
	I2caRegs.I2CCLKH = ICCLH;
}


/*
 * <summary>
 * 	Initializes the GPIO for the I2C
 * </summary>
 */
static void InitI2CGpio()
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPDPUD.bit.GPIO104 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPDPUD.bit.GPIO105 = 0;	   // Enable pull-up for GPIO33 (SCLA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPDQSEL1.bit.GPIO104 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPDQSEL1.bit.GPIO105 = 3;  // Asynch input GPIO33 (SCLA)

/* Configure SCI pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be I2C functional pins.
// Comment out other unwanted lines.

	GpioCtrlRegs.GPDGMUX1.bit.GPIO104 = 0;   // Configure GPIO104 for SDAA operation
	GpioCtrlRegs.GPDGMUX1.bit.GPIO105 = 0;   // Configure GPIO105 for SCLA operation
	GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 1;
	GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 1;

    //EDIS;
}
