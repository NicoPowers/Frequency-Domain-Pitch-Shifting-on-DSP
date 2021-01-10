/*
 * SRAM_Driver.h
 *
 *  Created on: Sep 30, 2020
 *      Author: User
 */

#ifndef SRAM_DRIVER_H_
#define SRAM_DRIVER_H_

typedef struct
{
    volatile int16 leftChannel, rightChannel;
} BufferData;



typedef struct
{
    volatile float32 leftChannel, rightChannel;
} BufferDataFloat;

void SRAM_init();
void SRAM_clear();
void SRAM_write_word(uint32_t address, int16_t data);
void SRAM_write_words(uint32_t address, int16_t *data, uint16_t num_data);
int16_t SRAM_read_word(uint32_t address);
int16_t* SRAM_read_words(uint32_t address, uint16_t num_data);
void buffer_write_inc(volatile BufferData* b, volatile uint32_t *bufferIndex);
void buffer_write_noinc(volatile BufferData* b, volatile uint32_t *bufferIndex);
void buffer_read_inc(volatile BufferData *b, volatile uint32_t *bufferIndex);
void buffer_read_noinc(volatile BufferData *b, volatile uint32_t *bufferIndex);

#endif /* SRAM_DRIVER_H_ */
