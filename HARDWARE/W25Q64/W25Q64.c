#include "stm32f10x.h"                  // Device header
#include "lcd_init.h"
#include "W25Q64_Ins.h"
#include "weather_pic.h"

void W25Q64_Init(void)
{
	MySPI_Init();
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Flash_Start();
	MySPI_SwapByte(0x9F);
	*MID = MySPI_SwapByte(0xFF);
	*DID = MySPI_SwapByte(0xFF);
	*DID <<= 8;
	*DID |= MySPI_SwapByte(0xFF);
	MySPI_Flash_Stop();
}

void W25Q64_WriteEnable(void)
{
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Flash_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout;
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while ((MySPI_SwapByte(0xFF) & 0x01) == 0x01)
	{
		Timeout --;
		if (Timeout == 0)
		{
			break;
		}
	}
	MySPI_Flash_Stop();
}

void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	MySPI_Flash_Stop();
	W25Q64_WaitBusy();
}

void W25Q64_ChipErase(void)
{
//	W25Q64_WriteEnable();
//	MySPI_Flash_Start();
//	MySPI_SwapByte(W25Q64_CHIP_ERASE);
//	MySPI_Flash_Stop();
//	W25Q64_WaitBusy();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint8_t i;
	W25Q64_WriteEnable();
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		 MySPI_SwapByte(DataArray[i]);
	}
	MySPI_Flash_Stop();
	W25Q64_WaitBusy();
}

void W25Q64_SectorProgram(uint8_t BlockNum, uint8_t SectorNum, uint8_t *DataArray)
{
	uint8_t i;
	uint8_t *DataPoint;
	uint32_t Address = 0x000000;
	Address = Address + SectorNum * 0x001000 + BlockNum * 0x010000;
	DataPoint = DataArray;
	W25Q64_SectorErase(Address);
	for(i = 0; i < 16; i ++)
	{
		W25Q64_PageProgram(Address, DataPoint, 255);
		Address += 0x000100;
		DataPoint += 256;
	}
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = MySPI_SwapByte(0xFF);
	}
	MySPI_Flash_Stop();
}

void W25Q64_ReadSectorData(uint8_t BlockNum, uint8_t SectorNum, uint8_t *DataArray)
{
	uint32_t i;
	uint32_t Address = 0x000000;
	Address = Address + SectorNum * 0x001000 + BlockNum * 0x010000;
	MySPI_Flash_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for (i = 0; i < 4095; i ++)
	{
		DataArray[i] = MySPI_SwapByte(0xFF);
	}
	MySPI_Flash_Stop();
}

void W25Q64_WriteFlash(void)
{
//	W25Q64_SectorProgram(0, 0, gImage_sunny);
//	W25Q64_SectorProgram(0, 1, gImage_moon);
//	W25Q64_SectorProgram(0, 2, gImage_rain);
//	W25Q64_SectorProgram(0, 3, gImage_rainAndSnow);
//	W25Q64_SectorProgram(0, 4, gImage_lightning);
//	W25Q64_SectorProgram(0, 5, gImage_lightingWithRain);
//	W25Q64_SectorProgram(0, 6, gImage_snow);
//	W25Q64_SectorProgram(0, 7, gImage_rainAtNight);
//	W25Q64_SectorProgram(0, 8, gImage_rainAtDay);
//	W25Q64_SectorProgram(0, 9, gImage_cloudy);
//	W25Q64_SectorProgram(0, 10, gImage_sunnyToCloudyAtDay);
//	W25Q64_SectorProgram(0, 11, gImage_sunnyToCloudyAtNight);
//	W25Q64_ReadSectorData(11, gImage);


//	W25Q64_SectorProgram(1, 0, xinzhi_0);
//	W25Q64_SectorProgram(1, 1, xinzhi_1);
//	W25Q64_SectorProgram(1, 4, xinzhi_4);
//	W25Q64_SectorProgram(1, 5, xinzhi_5);
//	W25Q64_SectorProgram(1, 6, xinzhi_6);
//	W25Q64_SectorProgram(1, 9, xinzhi_9);
//	W25Q64_SectorProgram(1, 10, xinzhi_10);
//	W25Q64_SectorProgram(1, 11, xinzhi_11);
//	W25Q64_SectorProgram(1, 12, xinzhi_12);
//	W25Q64_SectorProgram(1, 13, xinzhi_13);
//	W25Q64_SectorProgram(1, 14, xinzhi_14);
//	W25Q64_SectorProgram(1, 15, xinzhi_15);
//	W25Q64_SectorProgram(2, 16-16, xinzhi_16);
//	W25Q64_SectorProgram(2, 17-16, xinzhi_17);
//	W25Q64_SectorProgram(2, 19-16, xinzhi_19);
//	W25Q64_SectorProgram(2, 20-16, xinzhi_20);
//	W25Q64_SectorProgram(2, 21-16, xinzhi_21);
//	W25Q64_SectorProgram(2, 22-16, xinzhi_22);
//	W25Q64_SectorProgram(2, 23-16, xinzhi_23);
//	W25Q64_SectorProgram(2, 24-16, xinzhi_24);
//	W25Q64_SectorProgram(2, 25-16, xinzhi_25);
//	W25Q64_SectorProgram(2, 26-16, xinzhi_26);
//	W25Q64_SectorProgram(2, 28-16, xinzhi_28);
//	W25Q64_SectorProgram(2, 30-16, xinzhi_30);
//	W25Q64_SectorProgram(2, 31-16, xinzhi_31);
//	W25Q64_SectorProgram(3, 32-16*2, xinzhi_32);
//	W25Q64_SectorProgram(3, 34-16*2, xinzhi_34);
//	W25Q64_SectorProgram(3, 36-16*2, xinzhi_36);

//	W25Q64_SectorProgram(4, 0, gImage_temperature);
}
