#include "rx8025.h"


//0:sec 1:min 2:hour 3:week 4:day 5:month 6:year 
esp_err_t Write_RTC(unsigned char command,unsigned char dat)
{
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, RTC_Write_Address, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, dat, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(MINI_WATCH_I2C, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    return ret;
}

uint8_t Read_RTC(unsigned char command)
{
    uint8_t ret_dat = 0;

    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_ack_type_t nack = I2C_MASTER_NACK;
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, RTC_Write_Address, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, RTC_Read_Address, ACK_CHECK_EN);
    ret = i2c_master_read_byte(cmd, &ret_dat, nack);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(MINI_WATCH_I2C, cmd, portMAX_DELAY);
    /* ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_cmd_begin(MINI_WATCH_I2C, cmd, portMAX_DELAY)); */
    i2c_cmd_link_delete(cmd);
    return ret_dat;
}

void rx8052_init()
{
	Write_RTC(0x0D,0x00);
	Write_RTC(0x0E,0x00);
	Write_RTC(0x0F,0x40);
}

unsigned char Read_Time(unsigned char Add)
{
	unsigned char dat,decade,unit;
	dat = Read_RTC(Add);
	decade = dat/16;
	unit   = dat%16;
	dat    = decade*10 + unit;
	return dat;
}

unsigned char Read_Week()
{
	unsigned char dat,week = 0;
	dat = Read_RTC(Week_Add);
	for(;dat!=1;dat>>=1)
	{
		week++;
	}
	return week;
}

unsigned char DecToBCD(unsigned char Dec,bool week)
{
	unsigned char unit,decade;
	unsigned char BCD;
	if(week)
	{
		BCD = 0x01<<Dec;					//0是周日，6是周六
	}
	else
	{
		unit 	 = Dec % 10;
		decade = Dec / 10;
		BCD    = decade*16 + unit;
	}
	return BCD;
}

//5207
unsigned int Day_Count()
{
	unsigned int Day;
	unsigned int year_start,month_start,day_start,year_end,month_end,day_end;
	unsigned int y2,m2,d2;
	unsigned int y1,m1,d1;
	year_start = 14;
	month_start = 4;
	day_start = 4;
	//year_end = Read_Time(Year_Add);
	//month_end = Read_Time(Month_Add);
	//day_end = Read_Time(Day_Add);
	year_end = 17;
	month_end = 1;
	day_end = 24;
	
	d2 = year_end*365 + (year_end/4) + 1 + (month_end - 1) * 31 + (day_end - 1);
	//Day = d2 - d1;
	d2 = 6789;
  return d2;
}


void rtcTask(void *pvParameter)
{
    i2c_init();
    printf("yuki: i2c init success");
    rx8052_init();

/* //0:sec 1:min 2:hour 3:week 4:day 5:month 6:year */ 
/* esp_err_t Write_RTC(unsigned char command,unsigned char dat) */
    
    Write_RTC(2, 12);
    Write_RTC(1, 0);
    Write_RTC(1, 0);


    uint8_t time[7] = {0};
    while(1)
    {
        for(int i = 0; i < 7; i++)
            time[i] = Read_Time(i);
        printf("%d : %d : %d\n", time[2], time[1], time[0]);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

}

