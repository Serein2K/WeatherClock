#include "lcd_init.h"
#include "delay.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_spi.h"

void MySPI_LCD_CS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}

void MySPI_Flash_CS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);
}

void MySPI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);
	
	MySPI_LCD_CS(1);
	MySPI_Flash_CS(1);
}

void MySPI_LCD_Start(void)
{
	MySPI_LCD_CS(0);
}

void MySPI_LCD_Stop(void)
{
	MySPI_LCD_CS(1);
}

void MySPI_Flash_Start(void)
{
	MySPI_Flash_CS(0);
}

void MySPI_Flash_Stop(void)
{
	MySPI_Flash_CS(1);
}

uint8_t MySPI_SwapByte(u8 dat) 
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);
	
	SPI_I2S_SendData(SPI1, dat);
	
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);
	
	return SPI_I2S_ReceiveData(SPI1);
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	MySPI_LCD_Start();
	MySPI_SwapByte(dat);
	MySPI_LCD_Stop();
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	MySPI_LCD_Start();
	MySPI_SwapByte(dat>>8);
	MySPI_SwapByte(dat);
	MySPI_LCD_Stop();
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	MySPI_LCD_Start();
	LCD_DC_Clr();//д����
	MySPI_SwapByte(dat);
	LCD_DC_Set();//д����
	MySPI_LCD_Stop();
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+3);
		LCD_WR_DATA(y2+3);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//������д
	}
	else
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+3);
		LCD_WR_DATA(x2+3);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//������д
	}
}

void LCD_Init(void)
{
	MySPI_Init();//��ʼ��GPIO
	
	LCD_RES_Clr();//��λ
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_BLK_Set();//�򿪱���
  delay_ms(100);
	
	LCD_WR_REG(0x11); //Sleep out
	delay_ms(120); //Delay 120ms
	//------------------------------------ST7735S Frame rate-------------------------------------------------//
	LCD_WR_REG(0xB1); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB2); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_REG(0xB3); //Frame rate 80Hz
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x35);
	LCD_WR_DATA8(0x36);
	//------------------------------------End ST7735S Frame rate-------------------------------------------//
	LCD_WR_REG(0xB4); //Dot inversion
	LCD_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence-----------------------------------------//
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0xA2);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x84);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence---------------------------------------//
	LCD_WR_REG(0xC5); //VCOM
	LCD_WR_DATA8(0x0a);
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0XE0);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	LCD_WR_REG(0XE1);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x10);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	LCD_WR_REG(0x3A); //65k mode
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x29); //Display on 
}








