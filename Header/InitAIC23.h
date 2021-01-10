#ifndef _INIT_AIC_23_H
#define _INIT_AIC_23_H

#include <F2837xD_device.h>

/***************** User Functions *****************/
void SpiTransmit(uint16_t data);
void InitMcBSPb();
void InitSPIA();
void InitAIC23();



#endif

