/*
 * SPIB_Driver.c
 *
 *  Created on: Sep 30, 2020
 *      Author: Nicolas Montoya
 *      This code serves as a SPI Driver for port B.
 *      This allows for the initialization of SPIB as well as basic usage functionality.
 */

#include <F28x_Project.h>
#include <SPIB_Driver.h>
#include <stdlib.h>


// ******************** Read a byte from SPIB  *************************
char SPIB_read_byte()
{

    // write dummy data to start SPI transaction
    SpibRegs.SPIDAT = 0x0000;

    while (!SpibRegs.SPISTS.bit.INT_FLAG)
    {
        // wait until transmit is done
    }

    // reset INT_FLAG by reading SPIRXBUF
    SpibRegs.SPIRXBUF;

    return (char) SpibRegs.SPIDAT;
}


// ******************** Write a byte to SPIB *************************
void SPIB_write_byte(char data)
{

    // left justify data since data register is 16 bits
    SpibRegs.SPIDAT = (0x0000 | data) << 8;

    while (!SpibRegs.SPISTS.bit.INT_FLAG)
    {
        // wait until transmit is done
    }

    // reset INT_FLAG by reading SPIRXBUF
    unsigned int dummyInputData = SpibRegs.SPIRXBUF;
}


// ******************** Initialize SPI Peripheral on Port B *************************
void SPIB_init()
{

    EALLOW;

    // disable SPI for the time being

    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0;    // fastest speed I could get to allow SRAM to work (25 MHz)
    SpibRegs.SPICCR.bit.SPISWRESET = 0;     // force SPI to reset state
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;   // set SPI to master mode
    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;    // set CLK to latch on rising edge
    SpibRegs.SPICTL.bit.CLK_PHASE = 1;      // enable clock phase for delay
    SpibRegs.SPIBRR.bit.SPI_BIT_RATE = 0x0;   // set to highest speed
    SpibRegs.SPICCR.bit.HS_MODE = 1;        // enable high speed mode
    SpibRegs.SPICCR.bit.SPICHAR = 0x7;      // set character length to 8 bits
    SpibRegs.SPICTL.bit.TALK = 1;           // enable transmit
    SpibRegs.SPIPRI.bit.FREE = 1; // enable Free bit so debugging does not stop SPI communication
    SpibRegs.SPICCR.bit.SPISWRESET = 1;     // release SPI from reset state

    // initialize all relevant GPIO pins connected for SPIB
    SPIB_gpio_init();
}

// ******************** Initialize GPIO for SPI Peripheral on Port B *************************
void SPIB_gpio_init()
{
    // multiplex pins 63, 64, 65
    EALLOW;

    // SPISIMOB
    GpioCtrlRegs.GPBGMUX2.bit.GPIO63 = 0b11;
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 0b11;
    GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 0b11;

    // SPISOMIB
    GpioCtrlRegs.GPCGMUX1.bit.GPIO64 = 0b11;
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0b11;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 0b11;

    // SPICLKB
    GpioCtrlRegs.GPCGMUX1.bit.GPIO65 = 0b11;
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0b11;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 0b11;

    // set pin 66 and 67 as GPIO, and set as output, default high
    GpioCtrlRegs.GPCGMUX1.bit.GPIO66 = 0b00;
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0b00;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO67 = 0b00;
    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0b00;

    // set direction of pins 63, 65, 66, 67 as outputs
    GpioCtrlRegs.GPBDIR.bit.GPIO63 = 1;
    GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;
    GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;
    GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;

    // set CE for SRAM0 (66) high (off state)
    GpioDataRegs.GPCSET.bit.GPIO66 = 1;

    // set CE for SRAM1 (67) high (off state)
    GpioDataRegs.GPCSET.bit.GPIO67 = 1;

}
