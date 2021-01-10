/*
 * Codec_Driver.h
 *
 *  Created on: Oct 20, 2020
 *      Author: nicol
 */

#ifndef HEADER_CODEC_DRIVER_H_
#define HEADER_CODEC_DRIVER_H_

void InitMcBSPb();
void InitSPIA();
void InitAIC23();
void SpiTransmit(uint16_t data);
void Codec_driver_init();
void Codec_driver_send_command(uint16_t command);

#endif /* HEADER_CODEC_DRIVER_H_ */
