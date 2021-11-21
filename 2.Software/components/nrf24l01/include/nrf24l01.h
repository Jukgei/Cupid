#ifndef NRF24L01_H
#define NRF24L01_H

#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define TX_ADR_WIDTH 	5  	//发射地址宽度
#define TX_PLOAD_WIDTH  4   //发射数据通道有效数据宽度0~32Byte 

#define RX_ADR_WIDTH    5
#define RX_PLOAD_WIDTH  4



#define CHANAL 40	//频道选择 

// SPI(nRF24L01) commands ,	NRF的SPI命令宏定义，详见NRF功能使用文档
#define NRF_READ_REG    0x00  // Define read command to register
#define NRF_WRITE_REG   0x20  // Define write command to register
#define RD_RX_PLOAD 0x61  // Define RX payload register address
#define WR_TX_PLOAD 0xA0  // Define TX payload register address
#define FLUSH_TX    0xE1  // Define flush TX register command
#define FLUSH_RX    0xE2  // Define flush RX register command
#define REUSE_TX_PL 0xE3  // Define reuse TX payload register command
#define NOP         0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ，NRF24L01 相关寄存器地址的宏定义
#define CONFIG      0x00  // 'Config' register address
#define EN_AA       0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02  // 'Enabled RX addresses' register address
#define SETUP_AW    0x03  // 'Setup address width' register address
#define SETUP_RETR  0x04  // 'Setup Auto. Retrans' register address
#define RF_CH       0x05  // 'RF channel' register address
#define RF_SETUP    0x06  // 'RF setup' register address
#define STATUS      0x07  // 'Status' register address
#define OBSERVE_TX  0x08  // 'Observe TX' register address
#define CD          0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  // 'RX address pipe5' register address
#define TX_ADDR     0x10  // 'TX address' register address
#define RX_PW_P0    0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  // 'FIFO Status Register' register address

#define MAX_RT      0x10 //达到最大重发次数中断标志位
#define TX_DS		0x20 //发送完成中断标志位	  // 

#define RX_DR		0x40 //接收到数据中断标志位

#define SSIG 0x80
#define SPEN 0x40
#define DORD 0x20
#define MSTR 0x10
#define CPOL 0x08
#define CPHA 0x04
#define SPDHH 0x00
#define SPDH 0x01
#define SPDL 0x02
#define SPDLL 0x03
#define SPIF 0x80
#define WCOL 0x40
//sfr AUXR1  = 0xA2;

/* sbit CSN = P1^4; */
/* sbit CE  = P1^3; */
/* sbit IQR = P2^4; */

/* #define RECEIVER */ 
#ifndef RECEIVER
#define NFR_MISO_PIN  13
#define NRF_MOSI_PIN  12 
#define NRF_CLK_PIN   14 
#define NRF_CS_PIN    27
#define NRF_CE_PIN    26
#define NRF_IQR_PIN   15
#define NRF_OUTPUT_PIN_SEL   ((1ULL<<NRF_CS_PIN)|(1ULL<<NRF_CE_PIN)) 
#define NRF_INPUT_PIN_SEL (1ULL<<NRF_IQR_PIN)
#else
#define NFR_MISO_PIN  22
#define NRF_MOSI_PIN  15 
#define NRF_CLK_PIN   14
#define NRF_CS_PIN    27
#define NRF_CE_PIN    26
#define NRF_IQR_PIN   25
#define NRF_OUTPUT_PIN_SEL   ((1ULL<<NRF_CS_PIN)|(1ULL<<NRF_CE_PIN)) 
#define NRF_INPUT_PIN_SEL (1ULL<<NRF_IQR_PIN)
#endif

#define NRF_SPI_HOST  SPI3_HOST
#define NRF_DMA_CHAN  2


#define NRF_CLK_FRQ   (4*1000*1000)

#define NRF_TASK_DELAY 10

/* void nrf_cs_high(spi_transaction_t * t); */
/* void nrf_cs_low (spi_transaction_t * t); */
void nrf_cs_high(void );
void nrf_cs_low (void );
bool NRF24L01_Check(spi_device_handle_t nrf_spi);
/* unsigned char NRF24L01_Check(spi_device_handle_t nrf_spi); */
spi_device_handle_t nrf_spi_init(void);
void NRF_TX_Mode(spi_device_handle_t nrf_spi);
unsigned char NRF_Tx_Dat(spi_device_handle_t nrf_spi, unsigned char *txbuf);
void NRF_Sleep(spi_device_handle_t nrf_spi);
void NRF_Work(spi_device_handle_t nrf_spi);
void nrf_example(void);
void receiver(spi_device_handle_t nrf_spi, SemaphoreHandle_t nrf_sdcard_semaphore);
void sender(spi_device_handle_t nrf_spi, SemaphoreHandle_t nrf_sdcard_semaphore);
void nrf_task(void * pvParameter);


#endif
