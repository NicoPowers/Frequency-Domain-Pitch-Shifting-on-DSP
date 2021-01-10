/*
 * SRAM_Driver.c
 *
 *  Created on: Sep 30, 2020
 *      Author: Nicolas Montoya
 *      This code serves as an SRAM driver that allows for initialization
 *      and basic usage of SRAM. It currently uses both SRAM chips in parallel.
 */

#include <F28x_Project.h>
#include <SRAM_Driver.h>
#include <SPIB_Driver.h>
#include <stdlib.h>

// **************** Initialize SRAM attached to SPIB *******************
void SRAM_init()
{
    SPIB_init();
}

void SRAM_clear()
{
    for (uint32_t i = 0; i < 0x40000; i++)
    {
        SRAM_write_word(i, 0);
    }
}

// **************** Write a single word to SRAM *******************
// user can write from 0x00000 to 0x3FFFF

void SRAM_write_word(uint32_t address, int16_t data)
{
    if (address < 0x20000)
    {
        // pull CE low to enable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO66 = 0;
    }
    else
    {
        // pull CE low to enable SRAM1
        GpioDataRegs.GPCDAT.bit.GPIO67 = 0;
    }

    uint32_t realAddress = (address << 1) & 0x3FFFF;

    // write the instruction byte. since data register is 16 bits, we need to left justify the instruction byte
    SPIB_write_byte(0x02);

    // send the address bytes. we can only send a byte at a time, so first send a blank byte (0's), and then the upper and lower byte
    SPIB_write_byte(realAddress >> 16);
    SPIB_write_byte(realAddress >> 8); // get the upper byte in the lower byte position
    SPIB_write_byte(realAddress); // send lower byte

    // write lower byte to SRAM0
    SPIB_write_byte((char) data);

    // write upper byte to SRAM0
    SPIB_write_byte(0x00 | (char) (data >> 8));

    if (address < 0x20000)
    {
        // pull CE high to disable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO66 = 1;
    }
    else
    {
        // pull CE high to disable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO67 = 1;
    }

}

// **************** Read a single word from SRAM *******************
int16_t SRAM_read_word(uint32_t address)
{
    if (address < 0x20000)
    {
        // pull CE low to enable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO66 = 0;
    }
    else
    {
        // pull CE low to enable SRAM1
        GpioDataRegs.GPCDAT.bit.GPIO67 = 0;
    }

    uint32_t realAddress = (address << 1) & 0x3FFFF;

    // write the instruction byte. since data register is 16 bits, we need to left justify the instruction byte
    SPIB_write_byte(0x03);

    // send the address bytes. we can only send a byte at a time, so first send a blank byte (0's), and then the upper and lower byte
    SPIB_write_byte(realAddress >> 16);
    SPIB_write_byte(realAddress >> 8); // get the upper byte in the lower byte position
    SPIB_write_byte(realAddress); // send lower byte

    SPIB_read_byte();
    char lowerByte = SPIB_read_byte();
    char higherByte = SPIB_read_byte();

    if (address < 0x20000)
    {
        // pull CE high to disable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO66 = 1;
    }
    else
    {
        // pull CE high to disable SRAM0
        GpioDataRegs.GPCDAT.bit.GPIO67 = 1;
    }

    return (int16_t) lowerByte | higherByte << 8;
}

// **************** Write an array of words to SRAM *******************
void SRAM_write_words(uint32_t address, int16_t *data, uint16_t num_data)
{

    for (int i = 0; i < num_data; i++)
    {
        // write higher byte to SRAM1
        SRAM_write_word(address + i, data[i]);
    }
}

// **************** Read an array of words from SRAM *******************
int16_t* SRAM_read_words(uint32_t address, uint16_t num_data)
{
    int16_t *data = malloc(sizeof(int16_t) * num_data);

    for (int i = 0; i < num_data; i++)
    {
        data[i] = SRAM_read_word(address + i);
    }

    return data;
}

void buffer_write_inc(volatile BufferData* b, volatile uint32_t *bufferIndex)
{

    SRAM_write_word(*bufferIndex, b->leftChannel);
    (*bufferIndex)++;

// store low word at odd address
    SRAM_write_word(*bufferIndex, b->rightChannel);
    (*bufferIndex)++;

// if we have finished filling the buffer, reset index back to 0
    if (*bufferIndex == 0x40000)
    {
        *bufferIndex = 0;
    }

}

void buffer_write_noinc(volatile BufferData* b, volatile uint32_t *bufferIndex)
{

    SRAM_write_word(*bufferIndex, b->leftChannel);


// store low word at odd address
    SRAM_write_word(*bufferIndex, b->rightChannel);


// if we have finished filling the buffer, reset index back to 0
    if (*bufferIndex == 0x40000)
    {
        *bufferIndex = 0;
    }

}

void buffer_read_inc(volatile BufferData* b, volatile uint32_t *bufferIndex)
{

    b->leftChannel = SRAM_read_word(*bufferIndex);
    (*bufferIndex)++;
    b->rightChannel = SRAM_read_word(*bufferIndex);
    (*bufferIndex)++;

// if we have reached the end of the buffer, reset the buffer index back to 0
    if (*bufferIndex == 0x40000)
    {
        *bufferIndex = 0;
    }

}

void buffer_read_noinc(volatile BufferData *b, volatile uint32_t* bufferIndex)
{

    b->leftChannel = SRAM_read_word(*bufferIndex);

    b->rightChannel = SRAM_read_word((*bufferIndex) + 1);

}
