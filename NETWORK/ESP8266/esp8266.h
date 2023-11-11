#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stdio.h"	




#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志



//函数声明
unsigned char *ESP8266_GetIPD_GET(unsigned short timeOut,u8 *buff);

void ESP8266_Clear(void);
_Bool ESP8266_WaitRecive(void);

_Bool ESP8266_WaitRecive(void);

_Bool ESP8266_SendCmd(char *cmd, char *res);
void ESP8266_SendData(unsigned char *data, unsigned short len);

void ESP8266_Init(void);
void Get_Current_Weather(void);
void My_ParseLife2(char * str, uint8_t resultNum);
void Get_Current_Life(void);
void Get_Current_Time(void);

#endif
