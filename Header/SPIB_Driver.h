/*
 * SPIB_Driver.h
 *
 *  Created on: Sep 30, 2020
 *      Author: User
 */

#ifndef SPIB_DRIVER_H_
#define SPIB_DRIVER_H_

void SPIB_init();
void SPIB_gpio_init();
char SPIB_read_byte();
void SPIB_write_byte(char data);


#endif /* SPIB_DRIVER_H_ */
