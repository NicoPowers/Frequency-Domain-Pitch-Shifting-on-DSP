/*
 * LCD_Driver.h
 *
 *  Created on: Sep 30, 2020
 *      Author: User
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

void LCD_init();
void LCD_clear();
void LCD_newline();
void LCD_write_string(char* string);
void LCD_write_data(char *data, Uint16 num_elements);
void LCD_write_command(char* commands, Uint16 num_elements);
void LCD_write_float(float data);


#endif /* LCD_DRIVER_H_ */
