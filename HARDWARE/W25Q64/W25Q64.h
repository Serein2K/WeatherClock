#ifndef __W25Q64_H
#define __W25Q64_H

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_ChipErase(void);
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint32_t Count);
void W25Q64_SectorProgram(uint8_t BlockNum, uint8_t SectorNum, uint8_t *DataArray);
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);
void W25Q64_ReadSectorData(uint8_t BlockNum, uint8_t SectorNum, uint8_t *DataArray);
void W25Q64_WriteFlash(void);

#endif
