/*
 * LCD_Driver.c
 *
 *  Created on: Sep 30, 2020
 *      Author: Nicolas Montoya
 */

#include "Custom_Utils.h"
#include <F28x_Project.h>
#include <OneToOneI2CDriver.h>
#include <LCD_Driver.h>
#include <stdlib.h>
#include <string.h>

char init_commands[] = { 0x33, 0x32, 0x28, 0x0F, 0x01 };
char newline[] = { 0xC0 };
char clear[] = { 0x01, 0x02 };

void LCD_init() {
    I2C_O2O_Master_Init(0x27, 200, 100);
    LCD_write_command(init_commands, sizeof(init_commands)/sizeof(char));
}

void LCD_newline() {
    LCD_write_command(newline, sizeof(newline)/sizeof(char));
}

void LCD_clear() {
    LCD_write_command(clear, sizeof(clear)/sizeof(char));
}

void LCD_write_float(float data) {
    char dataString[16];
    ftoa(data, dataString, 2);
    LCD_write_string(dataString);
}

void LCD_write_string(char* string) {
    LCD_write_data(string, strlen(string));
}

void LCD_write_data(char *data, Uint16 num_elements)
{
    Uint16 *dataToWrite = (Uint16*) malloc(sizeof(Uint16) * num_elements * 6); // allocate new memory to store data we need to write LCD

        // we need to breakdown each byte into two nibbles, and send each nibble 3 times, so its 6 cycles per byte
        for (int i = 0; i < num_elements; i++)
        {
            // prepare to write upper nibble
            dataToWrite[(i*6)] = (data[i] & 0xF0) | 0x09; // write the upper nibble with E disabled
            dataToWrite[(i*6) + 1] = (data[i] & 0xF0) | 0x0D; // write the upper nibble with E enabled
            dataToWrite[(i*6) + 2] = (data[i] & 0xF0) | 0x09; // write the upper nibble with E disabled
            // prepare to write lower nibble
            dataToWrite[(i*6) + 3] = ((data[i] << 4) & 0xF0) | 0x09; // write the lower nibble with E disabled
            dataToWrite[(i*6) + 4] = ((data[i] << 4) & 0xF0) | 0x0D; // write the lower nibble with E enabled
            dataToWrite[(i*6) + 5] = ((data[i] << 4) & 0xF0) | 0x09; // write the lower nibble with E disabled
        }

        I2C_O2O_SendBytes(dataToWrite, num_elements * 6);
        DELAY_US(10000);
        free(dataToWrite);
        DELAY_US(10000);
}

void LCD_write_command(char *commands, Uint16 num_elements)
{
    Uint16 *dataToWrite = (Uint16*) malloc(sizeof(Uint16) * num_elements * 6); // allocate new memory to store data we need to write LCD

    // we need to breakdown each byte into two nibbles, and send each nibble 3 times, so its 6 cycles per byte
    for (int i = 0; i < num_elements; i++)
    {
        // prepare to write upper nibble
        dataToWrite[(i*6)] = (commands[i] & 0xF0) | 0x08; // write the upper nibble with E disabled
        dataToWrite[(i*6) + 1] = (commands[i] & 0xF0) | 0x04; // write the upper nibble with E enabled
        dataToWrite[(i*6) + 2] = (commands[i] & 0xF0) | 0x08; // write the upper nibble with E disabled
        // prepare to write lower nibble
        dataToWrite[(i*6) + 3] = ((commands[i] << 4) & 0xF0) | 0x08; // write the lower nibble with E disabled
        dataToWrite[(i*6) + 4] = ((commands[i] << 4) & 0xF0) | 0x04; // write the lower nibble with E enabled
        dataToWrite[(i*6) + 5] = ((commands[i] << 4) & 0xF0) | 0x08; // write the lower nibble with E disabled
    }

    I2C_O2O_SendBytes(dataToWrite, num_elements * 6);
    DELAY_US(10000);
    free(dataToWrite);
    DELAY_US(10000);
}

