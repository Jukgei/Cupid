#ifndef LM75_H
#define LM75_H

#include "esp32_drivers.h"

#define Temp_Write_Address 0x90
#define Temp_Read_Address 0x91

esp_err_t Write_Temp(unsigned char command,unsigned char dat);
unsigned int Read_Temp(unsigned char command);
void temp_init();
int Read_Temperature();
void Temp_Sleep();


void tempTask(void *pvParameter);

#endif

