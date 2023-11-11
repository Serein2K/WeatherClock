#include "stm32f10x.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"
#include "esp8266.h"

#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "malloc.h"
#include <string.h>

//WIFI和密码·
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"420_right\",\"wqy20020303\"\r\n"
//心知天气的API
#define Weather_TCP		"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
//心知天气GET报文
/*这里城市 杭州    语言为  英文*/
#define Weather_GET		"GET https://api.seniverse.com/v3/weather/now.json?key=SWuyJfw-Yx4hDPPih&location=xian&language=zh-Hans&unit=c\r\n"

#define Life_GET       "GET https://api.seniverse.com/v3/life/suggestion.json?key=SWuyJfw-Yx4hDPPih&location=xian&language=en&days=1\r\n"

//苏宁后台获取时间的API
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n"
#define Time_tcp        "AT+CIPSTART=\"TCP\",\"api.k780.com\",80\r\n"
//苏宁后台获取时间GET报文
#define Time_GET		"GET https://quan.suning.com/getSysTime.do\r\n"
#define Time_get     "GET http://api.k780.com/?app=life.time&appkey=70964&sign=e8b287188394862232a921a1e6f25135&format=json\r\n\r\n"


//ESP8266数据存放
unsigned char esp8266_buf[1300] = {0};
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
//存放天气数据
unsigned char Weather_buff[300];   //位数是随机确定的
//存放时间数据
unsigned char Time_buff[100];   //位数是随机确定的
unsigned char Life_buff[1300];

uint8_t Time_results[19];
char Life_results[][50] = {{0}};

uint8_t Process = 0;

extern uint16_t Year;
extern uint8_t Month;
extern uint8_t Day;

extern uint8_t Hour;
extern uint8_t Minute;
extern uint8_t Second;

extern uint8_t ProcessFlag;

/**************************************************************************/
//函数作用：ESP8266_Init初始化函数
//函数名称：ESP8266_Init(void);
//内部参数：
//修改日期：2022年4月18日  下午16：18
//作者：    大屁桃
/**************************************************************************/
void ESP8266_Init(void)
{
		
	ESP8266_Clear();
	
 /*让WIFI推出透传模式*/
	while(ESP8266_SendCmd("+++", ""))  
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	UsartPrintf(USART_DEBUG, "1.AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//加一步ESP8266复位操作
	
	UsartPrintf(USART_DEBUG, "2.RST\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UsartPrintf(USART_DEBUG, "3.CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	UsartPrintf(USART_DEBUG, "4.AT+CIPMUX\r\n");
	while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"))
		delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	UsartPrintf(USART_DEBUG, "5.CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "WIFI GOT IP"))
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	UsartPrintf(USART_DEBUG, "ESP8266_Init OK\r\n");
}

void Get_Current_Weather(void)
{
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "6.Weather_TCP OK\r\n");	
	while(ESP8266_SendCmd(Weather_TCP, "CONNECT"))
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	UsartPrintf(USART_DEBUG, "7.AT+CIPMODE=1 OK\r\n");		
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"))
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	/*sizeof(Weather_GET)，必须用sizeof函数，用strlen没有用*/  	
	ESP8266_SendData((u8 *)Weather_GET, sizeof(Weather_GET)); //发送AT+CIPSEND  以及 Weather_GET
	//UsartPrintf(USART_DEBUG, "Weather_Get OK\r\n");
	ESP8266_GetIPD_GET(200,Weather_buff);
	ESP8266_Clear();//清除缓存数据	
	UsartPrintf(USART_DEBUG, (char *)Weather_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*退出透传模式*/
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //验证是否退出透传模式
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "退出透传模式成功！\r\n");
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
}

void My_ParseLife(uint16_t startNum, uint16_t resultsNum)
{
	uint16_t i;
	for(i=0; i<30; i++)
	{
		if(Life_buff[startNum+i] == '"')break;
		Life_results[resultsNum][i] = Life_buff[startNum+i];
	}
}

char* My_strstr(char *src, char *dst)  
{       
      while(NULL != src)  
      {  
          char *temp1 = src;     
          char *temp2 = dst;        
          char *res = NULL;
          if(*temp1 == *temp2)  
          {  
                res = temp1;  
                while(*temp1 && *temp2 && *temp1++ == *temp2++)  
                ;  
                  
                if(*temp2 == '\0')
		{
                      return res;   
		}      
          }  
          src++;  
      }  
      return NULL;  
} 

void My_ParseLife2(char * str, uint8_t resultNum)
{
	char *p, *p_brief, *p_temp;
	char result[30] = {0};
	char *p_result = result;
	
	uint8_t pNum;
	pNum = 0;
	p = My_strstr((char *)Life_buff, str);
	p_brief = strstr(p, "brief");
	p_result = p_brief + 8;
	p_temp = strstr(p_result, "\",\"");
	pNum = p_temp - p_result;

	p_result[pNum] = '\0';
//	do
//	{
//		Life_results[resultNum][i] = p_result[i];
//		i++;
//	}
//	while(p_result[i] != '\0');
//	Life_results[resultNum][i] = p_result[i];
	strcpy(Life_results[resultNum], p_result);
	UsartPrintf(USART_DEBUG, "%s is %s : , lenth : %d\r\n",str, p_result, pNum);
//	UsartPrintf(USART_DEBUG, "%s : %d\r\n", Life_results[resultNum], resultNum);
}

void Get_Current_Life(void)
{
	ESP8266_Clear();
	
//	UsartPrintf(USART_DEBUG, "6.Weather_TCP OK\r\n");	
//	while(ESP8266_SendCmd(Weather_TCP, "CONNECT"))
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	UsartPrintf(USART_DEBUG, "7.AT+CIPMODE=1 OK\r\n");		
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"))
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	/*sizeof(Weather_GET)，必须用sizeof函数，用strlen没有用*/  	
	ESP8266_SendData((u8 *)Life_GET, sizeof(Life_GET)); //发送AT+CIPSEND  以及 Weather_GET
	//UsartPrintf(USART_DEBUG, "Weather_Get OK\r\n");
	ESP8266_GetIPD_GET(200,Life_buff);
	ESP8266_Clear();//清除缓存数据	
	UsartPrintf(USART_DEBUG, (char *)Life_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*退出透传模式*/
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //验证是否退出透传模式
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "退出透传模式成功！\r\n");
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}

////	My_ParseLife(211, 0);//ac
////	My_ParseLife(262, 0);//air_pollution
//	My_ParseLife(301, 0);//airing
//	My_ParseLife(345, 1);//allergy
//	My_ParseLife(389, 2);//beer
//	My_ParseLife(528, 3);//comfort
////	My_ParseLife(656, 5);//flu
//	My_ParseLife(836, 4);//morning_sport
	
	My_ParseLife2("airing", 0);
	My_ParseLife2("allergy", 1);
	My_ParseLife2("beer", 2);
	My_ParseLife2("comfort", 3);
	My_ParseLife2("morning_sport", 4);
}

/**************************************************************************/
//函数作用：获取后台时间
//函数名称：Get_current_time(void);
//内部参数：
//修改日期：2022年4月18日  晚上20：32
//作者：    大屁桃
/**************************************************************************/
void Get_Current_Time(void)
{
	uint8_t i;
	Year = 0;
	Month = 0;
	Day = 0;
	Hour = 0;
	Minute = 0;
	Second = 0;
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "6.Time_TCP OK\r\n");	
	while(ESP8266_SendCmd(Time_tcp, "CONNECT"))
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
  UsartPrintf(USART_DEBUG, "7.AT+CIPMODE=1 OK\r\n");		
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"))
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
/*sizeof(Weather_GET)，必须用sizeof函数，用strlen没有用*/  	
//	ESP8266_SendData((u8 *)Time_GET, sizeof(Time_GET)); //发送AT+CIPSEND  以及 Weather_GET
	ESP8266_SendData((u8 *)Time_get, sizeof(Time_get));
	
	ESP8266_GetIPD_GET(500,Time_buff);  //将串口数据取出来
	ESP8266_Clear();//清除缓存数据	
	UsartPrintf(USART_DEBUG, (char *)Time_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*退出透传模式*/
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //验证是否退出透传模式
		delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	UsartPrintf(USART_DEBUG, "1.AT\r\n");
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	
	for(i=0; i<19; i++)
	{
		Time_results[i] = Time_buff[64+i];
	}
	for(i=0; i<4; i++)
	{
		Year += (Time_results[i]-'0')*mypow(10,3-i);
	}
	for(i=0; i<2; i++)
	{
		Month += (Time_results[i+5]-'0')*mypow(10,1-i);
	}
	for(i=0; i<2; i++)
	{
		Day += (Time_results[i+8]-'0')*mypow(10,1-i);
	}
	
	for(i=0; i<2; i++)
	{
		Hour += (Time_results[i+11]-'0')*mypow(10,1-i);
	}
	for(i=0; i<2; i++)
	{
		Minute += (Time_results[i+14]-'0')*mypow(10,1-i);
	}
	for(i=0; i<2; i++)
	{
		Second += (Time_results[i+17]-'0')*mypow(10,1-i);
	}
}

/**************************************************************************/
//函数作用：串口二中断函数
//函数名称：USART2_IRQHandler();
//内部参数：
//修改日期：2022年4月18日  下午4：18
//作者：    大屁桃
/**************************************************************************/
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{

		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆				
		esp8266_buf[esp8266_cnt++] = USART2->DR;  

//		USART_SendData(USART1,USART2->DR);      //让接收到的数据打印在串口一上
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}



//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}
//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}




//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	copy天气数据到Weather_buff数组里面
//
//	返回参数：	平台返回的原始数据
//
//	说明：		copy天气数据到Weather_buff
//==========================================================

unsigned char *ESP8266_GetIPD_GET(unsigned short timeOut,u8 *buff)//这里我用了一个全局变量将esp8266buf储存到这个全局变量里面
{
	do
	{
		delay_ms(5);													
	} while(timeOut--);
	strcpy((char*)buff,(char*)esp8266_buf);
	return buff;														
}

/*还未用到*/
//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}
//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	uint16_t timeOut = 800;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{		
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
				
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND\r\n");		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		UsartPrintf(USART_DEBUG, "8.AT+CIPSEND\r\n");
		/*发送请求数据*/
		Usart_SendString(USART2, data, len);		//发送设备连接请求数据		
	}
}


