/*
 * IO_Driver.c
 *
 *  Created on: Oct 1, 2020
 *      Author: User
 */

#include <F28x_Project.h>
#include <IO_Driver.h>

void IO_init()
{
    EALLOW;
    // enable pins 0:3, 4:11, 14:15, 16 on Port A as GPIO pins (DIP SW)

    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO8 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO10 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0b00;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 0b00;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0b00;

    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0b00;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0b00;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0b00;

    // enable internal pull-up for all input pins
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;

    // set DIP switches and Pushbuttons as inputs
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 0;  // DIP0
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 0;  // DIP1
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 0;  // DIP2
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 0;  // DIP3
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 0; // PB0
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 0; // PB1
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 0; // PB2

    // start LEDs off
    IO_LED_all(0x00);

    // set LEDs as outputs
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // LED0
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // LED1
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;  // LED2
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;  // LED3
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;  // LED4
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;  // LED5
    GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;  // LED6
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;  // LED7

}

Uint16 IO_read_PB(uint16_t PB_N)
{
    switch (PB_N)
    {
    case 0:
        return GpioDataRegs.GPADAT.bit.GPIO14;
    case 1:
        return GpioDataRegs.GPADAT.bit.GPIO15;
    case 2:
        return GpioDataRegs.GPADAT.bit.GPIO16;
    default:
        return 0;
    }

}

Uint16 IO_read_DIP(Uint16 DIP_N)
{
    switch (DIP_N)
    {
    case 0:
        return GpioDataRegs.GPADAT.bit.GPIO0;
    case 1:
        return GpioDataRegs.GPADAT.bit.GPIO1;
    case 2:
        return GpioDataRegs.GPADAT.bit.GPIO2;
    case 3:
        return GpioDataRegs.GPADAT.bit.GPIO3;
    default:
        return 0;
    }
}
void IO_LED(Uint16 LED_N, bool state)
{
    switch (LED_N)
    {
    case 0:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO4 = 1;
        }
        return;
    case 1:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO5 = 1;
        }
        return;
    case 2:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO6 = 1;
        }
        return;
    case 3:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO7 = 1;
        }
        return;
    case 4:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO8 = 1;
        }
        return;
    case 5:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO9 = 1;
        }
        return;
    case 6:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO10 = 1;
        }
        return;
    case 7:
        if (state)
        {
            GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
        }
        else
        {
            GpioDataRegs.GPASET.bit.GPIO11 = 1;
        }
        return;
    }
}

void IO_LED_all(char pattern)
{
    if (pattern & 0x01)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO4 = 1;
    }

    if ((pattern & 0x02) >> 1)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO5 = 1;
    }

    if ((pattern & 0x04) >> 2)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO6 = 1;
    }

    if ((pattern & 0x08) >> 3)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO7 = 1;
    }

    if ((pattern & 0x10) >> 4)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO8 = 1;
    }

    if ((pattern & 0x20) >> 5)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO9 = 1;
    }

    if ((pattern & 0x40) >> 6)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO10 = 1;
    }

    if ((pattern & 0x80) >> 7)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
    }
    else
    {
        GpioDataRegs.GPASET.bit.GPIO11 = 1;
    }

}

/*
 bool IO_read_PB0()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO14;
 }

 bool IO_read_PB1()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO15;
 }

 bool IO_read_PB2()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO16;
 }

 bool IO_read_DIP0()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO0;
 }

 bool IO_read_DIP1()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO1;
 }

 bool IO_read_DIP2()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO2;
 }

 bool IO_read_DIP3()
 {
 return (bool) GpioDataRegs.GPADAT.bit.GPIO3;
 }

 void IO_LED0(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO4 = 1;
 }
 }

 void IO_LED1(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO5 = 1;
 }
 }

 void IO_LED2(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO6 = 1;
 }
 }

 void IO_LED3(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO7 = 1;
 }
 }

 void IO_LED4(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO8 = 1;
 }
 }

 void IO_LED5(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO9 = 1;
 }
 }

 void IO_LED6(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO10 = 1;
 }
 }

 void IO_LED7(bool state)
 {
 if (state)
 {
 GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
 }
 else
 {
 GpioDataRegs.GPASET.bit.GPIO4 = 1;
 }
 }
 */
