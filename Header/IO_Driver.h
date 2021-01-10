/*
 * SW_PB_Driver.h
 *
 *  Created on: Oct 1, 2020
 *      Author: Nicolas Montoya
 *      This code serves as an IO driver.
 *      This code acts as an easy to use API to read from the pushbuttons, DIP switches, and output to the onboard LEDs.
 */

#ifndef HEADER_IO_DRIVER_H_
#define HEADER_IO_DRIVER_H_

void IO_init();

void IO_LED_all(char pattern);
Uint16 IO_read_PB(uint16_t PB_N);
Uint16 IO_read_DIP(Uint16 DIP_N);
void IO_LED(Uint16 LED_N, bool state);


/*
 bool IO_read_PB0 ();

 bool IO_read_PB1 ();

 bool IO_read_PB2 ();

 bool IO_read_DIP0 ();

 bool IO_read_DIP1 ();

 bool IO_read_DIP2 ();

 bool IO_read_DIP3 ();

 void IO_LED0(bool state);

 void IO_LED1(bool state);

 void IO_LED2(bool state);

 void IO_LED3(bool state);

 void IO_LED4(bool state);

 void IO_LED5(bool state);

 void IO_LED6(bool state);

 void IO_LED7(bool state);
 */

#endif /* HEADER_IO_DRIVER_H_ */
