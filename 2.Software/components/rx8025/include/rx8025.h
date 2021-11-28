#ifndef RX8025_H
#define RX8025_H

#include "esp32_drivers.h"

#define RTC_Write_Address 0x64
#define RTC_Read_Address 0x65

#define Sec_Add 0x00
#define Min_Add 0x01
#define Hour_Add 0x02
#define Week_Add 0x03
#define Day_Add 0x04
#define Month_Add 0x05
#define Year_Add 0x06

void rtcTask(void *pvParameter);

#endif

