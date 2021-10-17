#include "nrf24l01.h"


unsigned char TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  // 定义一个静态发送地址
unsigned char RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};
 
spi_device_handle_t nrf_spi_init(void)
{
	/* SPDAT  = 0; */
	/* SPSTAT = 0xC0; */
	/* SPCTL  = 0xD2; */
	/* IQR = 1; */
   
    gpio_config_t o_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = NRF_OUTPUT_PIN_SEL, 
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&o_config);

    gpio_config_t i_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = NRF_INPUT_PIN_SEL,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&i_config);

    spi_device_handle_t spi;
    spi_bus_config_t buscfg={
        .miso_io_num=NFR_MISO_PIN,
        .mosi_io_num=NRF_MOSI_PIN,
        .sclk_io_num=NRF_CLK_PIN,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz= 64*8,
    };
	
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=NRF_CLK_FRQ,           //Clock out at 4 MHz
        .mode=0,                                //SPI mode 0
        /* .spics_io_num=NRF_CS_PIN,               //CS pin */
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        /* .pre_cb=nrf_cs_low, */  
        /* .post_cb=nrf_cs_high, */
    };

    esp_err_t ret = spi_bus_initialize(NRF_SPI_HOST, &buscfg, NRF_DMA_CHAN);
    ESP_ERROR_CHECK(ret);

    ret = spi_bus_add_device(NRF_SPI_HOST, &devcfg, &spi);
    return spi;
}

void nrf_cs_high(void)
{
    gpio_set_level(NRF_CS_PIN, 1);
}

void nrf_cs_low(void)
{
    gpio_set_level(NRF_CS_PIN, 0);
}

void nrf_ce_low(void)
{
    gpio_set_level(NRF_CE_PIN, 0);
}

void nrf_ce_high(void)
{
    gpio_set_level(NRF_CE_PIN, 1);
}

uint8_t spi_write_byte(spi_device_handle_t nrf_spi, uint8_t data)
{
    esp_err_t ret = spi_device_acquire_bus(nrf_spi, portMAX_DELAY);
    /* if (ret != ESP_OK) return; */
    uint8_t * result = (uint8_t* )malloc(sizeof(uint8_t));
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
        .rx_buffer = result,
        /* .tx_data = {data}, */
        /* .flags = (SPI_TRANS_USE_TXDATA|SPI_TRANS_USE_RXDATA), */
        /* .flags = (SPI_TRANS_USE_RXDATA), */
    };
    /* memset(&t, 0, sizeof(t)); */
    /* t.length = 8; */
    /* /1* t.tx_buffer = &data; *1/ */
    /* t.tx_data = {data}; */
    /* t.flags = SPI_TRANS_USE_TXDATA, */
    ret = spi_device_polling_transmit(nrf_spi, &t);

    spi_device_release_bus(nrf_spi);
    /* return t.rx_data[0]; */
    return *result;
}

uint8_t spi_read_byte(spi_device_handle_t nrf_spi)
{
    spi_transaction_t t = {
        .length = 8,
        .flags = SPI_TRANS_USE_RXDATA,
    };
    /* spi_transaction_t t; */
    /* memset(&t, 0, sizeof(t)); */
    /* t.length = 8; */
    /* t.flags = SPI_TRANS_USE_RXDATA; */

    esp_err_t ret = spi_device_polling_transmit(nrf_spi, &t);
    /* assert( ret == ESP_OK ); */

    /* uint32_t result = *(uint32_t*)t.rx_data; */
    return t.rx_data[0];
}

unsigned char SPI_ReadWriteByte(spi_device_handle_t nrf_spi, unsigned char tx_data)
{
	/* SPDAT = TxData; */
	/* while((SPSTAT & 0x80) == 0); */
	/* SPSTAT = 0xC0; */
	
	/* return SPDAT; */
    /* spi_write_byte(nrf_spi, tx_data); */
    /* return spi_read_byte(nrf_spi); */
    return spi_write_byte(nrf_spi, tx_data);
}

unsigned char SPI_Read_Reg(spi_device_handle_t nrf_spi, unsigned char reg)
{
	unsigned char reg_val;
    nrf_ce_low();
    nrf_cs_low();
    /* printf("nrf calc before: 0x%x\n", reg); */
    /* printf("nrf calc after:  0x%x\n", (reg & 0x1f)|0x00); */
	SPI_ReadWriteByte(nrf_spi, (reg & 0x1f)|0x00);
	reg_val = SPI_ReadWriteByte(nrf_spi, 0x00);
    nrf_cs_high();
	return reg_val;
	
}
//向寄存器写一个字节，同时返回状态字，
unsigned char SPI_RW_Reg(spi_device_handle_t nrf_spi, unsigned char reg,unsigned char value)
{
	unsigned char status;
    /* gpio_set_level(NRF_CE_PIN, 0); */
    nrf_ce_low();
    nrf_cs_low();
	status = SPI_ReadWriteByte(nrf_spi, reg);
	SPI_ReadWriteByte(nrf_spi, value);
    nrf_cs_high();
	return status;
}

//写一个数据包
unsigned char SPI_Write_Buf(spi_device_handle_t nrf_spi, unsigned char reg,unsigned char *pBuf,unsigned char bytes)
{
	unsigned char status,byte_ctr;
    /* gpio_set_level(NRF_CS_PIN, 0); */
    nrf_ce_low();
    nrf_cs_low();
	status = SPI_ReadWriteByte(nrf_spi, reg);
	for(byte_ctr = 0;byte_ctr<bytes;byte_ctr++)
	{
		SPI_ReadWriteByte(nrf_spi, *pBuf++);
	}
    /* gpio_set_level(NRF_CS_PIN, 1); */
    nrf_cs_high();
	return status;
}

//读一个数据包
unsigned char SPI_Read_Buf(spi_device_handle_t nrf_spi, unsigned char reg,unsigned char *pBuf,unsigned char uchars)
{
	unsigned char status ,uchar_ctr;
    nrf_ce_low();
    nrf_cs_low();
    /* gpio_set_level(NRF_CS_PIN, 0); */
	status = SPI_ReadWriteByte(nrf_spi, reg);
	for(uchar_ctr = 0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_ReadWriteByte(nrf_spi, NOP);
    /* gpio_set_level(NRF_CS_PIN, 1); */
    nrf_cs_high();
	return status;
}

unsigned char NRF24L01_Check(spi_device_handle_t nrf_spi)
{
	unsigned char buf[5] = {0xA5,0xA5,0xA5,0xA5,0xA5};
	unsigned char i;
    /* memset(buf, 0, sizeof(buf[5])); */
	SPI_Write_Buf(nrf_spi, NRF_WRITE_REG+TX_ADDR,buf,5);
    memset(buf, 0, sizeof(buf[5]));
	SPI_Read_Buf(nrf_spi, TX_ADDR,buf,5);
    for(i = 0; i< 5; i++)
        printf("0x%x\t", buf[i]);
    printf("\n");
	for(i=0;i<5;i++)
	{
		if(buf[i] != 0xA5)
			break;
	}
	if(i != 5) return 1;         //错误
	else 			 return 0;				 //正确
}

void NRF_TX_Mode(spi_device_handle_t nrf_spi)
{
	/* CE = 0; */
    nrf_ce_low();

	SPI_Write_Buf(nrf_spi, NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);
	SPI_Write_Buf(nrf_spi, NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+EN_AA,0x01);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+EN_RXADDR,0x01);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+SETUP_RETR,0x1a);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+RF_CH,CHANAL);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+RF_SETUP,0x0f);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+CONFIG,0x0e);
	
    nrf_ce_high();
	/* CE = 1; */
}

unsigned char NRF_Tx_Dat(spi_device_handle_t nrf_spi, unsigned char *txbuf)
{
	unsigned char state;
	/* gpio_set_level(NRF_CE_PIN, 0); */
    nrf_ce_low();
	SPI_Write_Buf(nrf_spi, WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);
    nrf_ce_high();
    /* gpio_set_level(NRF_CE_PIN, 1); */
    while(gpio_get_level(NRF_IQR_PIN) != 0){printf("..\t");vTaskDelay(20);}
	state = SPI_Read_Reg(nrf_spi, STATUS);
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+STATUS,state);
	SPI_RW_Reg(nrf_spi, FLUSH_TX,NOP);
	if(state&MAX_RT)                     //达到最大重发次数
			 return MAX_RT; 

	else if(state&TX_DS)                  //发送完成
		 	return TX_DS;
	 else						  
			return state;                 //其他原因发送失败
}

void NRF_Sleep(spi_device_handle_t nrf_spi)
{
	SPI_RW_Reg(nrf_spi, NRF_WRITE_REG+CONFIG,0x08);
}

void NRF_Work(spi_device_handle_t nrf_spi)
{
	SPI_RW_Reg(nrf_spi, CONFIG,0x0A);
	
}

void nrf_example(void)
{
    /* nrf_cs_low(); */
    nrf_cs_high();
    printf("nrf example start\n");
    spi_device_handle_t nrf_spi = nrf_spi_init();
    /* uint8_t result = NRF24L01_Check(nrf_spi); */
    /* if (result == 0) */
    /*     printf("nrf check success\n"); */
    /* else */
    /*     printf("nrf check fail\n"); */
    /* printf("nrf ready to work config\n"); */
    /* NRF_Work(nrf_spi); */     
    /* printf("nrf work config\n"); */
    /* NRF_TX_Mode(nrf_spi); */
    /* printf("nrf tx config\n"); */
    /* uint8_t tx_buff[] = {2,2,2,2}; */
    /* result = NRF_Tx_Dat(nrf_spi, tx_buff); */
    /* printf("nrf send result state 0x%x\n", result); */
    /* if (result==0) */
    /*     printf("nrf send fail, state is 0x%x\n", result); */
    /* else if (result == MAX_RT) */
    /*     printf("nrf send max rt\n"); */
    /* else if (result == TX_DS) */
    /*     printf("nrf send success\n"); */
    /* else */
    /*     printf("nrf other\n"); */
    printf("nrf ready to read reg\n");
    printf("nrf EN_AA: 0x%x\n", SPI_Read_Reg(nrf_spi, EN_AA));

}

