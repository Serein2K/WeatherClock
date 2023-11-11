#include "delay.h"
#include "sys.h"
#include "led.h"
#include "lcd_init.h"
#include "lcd.h"
#include "stm32f10x_tim.h"

#include "usart.h"
#include "usart2.h"
#include "esp8266.h"
#include "cJSON.h" //解析天气
#include "W25Q64.h"
#include "Timer.h"
#include <string.h>


extern unsigned char Weather_buff[300];
extern unsigned char Time_buff[300];
//天气解析
Results Weather_results[] = {{0}};
extern unsigned char Life_buff[1300];

extern char Life_results[][50];
extern uint16_t strRam[128];

uint8_t gImage[4096];

uint16_t Year = 0;
uint8_t Month = 0;
uint8_t Day = 0;

uint8_t Hour = 0;
uint8_t Minute = 0;
uint8_t Second = 0;

uint8_t Weather_Code = 0;
uint8_t Weather_Temperature = 0;

uint16_t count = 0;
uint16_t Life_count = 0;
uint8_t TimeGetFlag = 0;
uint8_t TimeShowFlag = 0;
uint8_t LifeShowFlag = 0;

uint8_t ProcessFlag = 0;

int main(void)
{
	delay_init();
	LED_Init();//LED初始化
	LCD_Init();//LCD初始化
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	
	Usart1_Init(115200);
	Usart2_Init(115200);
	
	ESP8266_Init();
	Get_Current_Weather();
	cJSON_WeatherParse((char *)Weather_buff, Weather_results);
	Get_Current_Life();
	ESP8266_Init();
	Get_Current_Time();
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	ProcessFlag = 1;

	LCD_ShowDate(0, 0, BLACK, WHITE, 16, Year, Month, Day);
	LCD_ShowWeather(10, 45, Weather_Code, Weather_Temperature);
	TimeShowFlag = 1;
	LifeShowFlag = 1;
	
	Timer3_Init();

	while(1)
	{
		if(count >= 600)
		{
			count = 0;
			TimeShowFlag = 0;
			LifeShowFlag = 0;
			
			delay_ms(1000);
			ESP8266_Init();
			Get_Current_Weather();
			cJSON_WeatherParse((char *)Weather_buff, Weather_results);
			LCD_ShowWeather(10, 45, Weather_Code, Weather_Temperature);
			delay_ms(1000);
			
			TimeShowFlag = 1;
			LifeShowFlag = 1;
		}
		if(TimeGetFlag == 1)
		{
			TimeGetFlag = 0;
			TimeShowFlag = 0;
			LifeShowFlag = 0;
			
			delay_ms(1000);
			ESP8266_Init();
			Get_Current_Weather();
			Get_Current_Life();
			ESP8266_Init();
			Get_Current_Time();
			LCD_ShowDate(0, 0, BLACK, WHITE, 16, Year, Month, Day);
			delay_ms(1000);
			
			TimeShowFlag = 1;
			LifeShowFlag = 1;
		}
		LCD_ShowLife();
	}
}

