#include "lm75.h"

esp_err_t Write_Temp(unsigned char command,unsigned char dat)
{

    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, Temp_Write_Address, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, dat, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(MINI_WATCH_I2C, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);
    return ret;
}

unsigned int Read_Temp(unsigned char command)
{
	unsigned char MS,LS;
	unsigned int temp;

    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_ack_type_t nack = I2C_MASTER_NACK;
    i2c_ack_type_t ack = I2C_MASTER_ACK;
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, Temp_Write_Address, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, Temp_Read_Address, ACK_CHECK_EN);
    ret = i2c_master_read_byte(cmd, &MS, ack);
    ret = i2c_master_read_byte(cmd, &LS, nack);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(MINI_WATCH_I2C, cmd, portMAX_DELAY);
    i2c_cmd_link_delete(cmd);

    temp = MS;												//把8个字节送入16个字节的低位
	temp = temp<<8;										//把低位左移到高位		
	temp = temp|LS;										//写入新的低位
	return temp;
}

void temp_init()
{
	Write_Temp(0x01,0x00);						//01：是控制寄存器的地址 00：Normal Mode
}

int Read_Temperature()
{
	int Temperature;
	float F;
	Temperature = Read_Temp(0x00);		//00是温度寄存器的地址，读取温度寄存器的值
	Temperature = Temperature>>5;			//按Datasheet转换温度	
	//Temperature = 0x738;
	if((Temperature&0x400) == 0x400)
	{
		Temperature = 0x7FF - Temperature;
		F = 1.25 * Temperature;
		Temperature = (int)F;
		Temperature = -Temperature;
	}
	else
	{
		F = 1.25 * Temperature;					//按Datasheet转换温度	
		Temperature = (int)F;
	}
	return Temperature;
}

void Temp_Sleep()
{
	Write_Temp(0x01,0x01);						//01：是控制寄存器的地址 01：Shut-Down Mode
}

void tempTask(void *pvParameter)
{
    temp_init();

    while(1)
    {
        int temp = Read_Temperature();
        printf("yuki: temp is %d\n", temp);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

