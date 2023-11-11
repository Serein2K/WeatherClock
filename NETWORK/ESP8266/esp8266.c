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

//WIFI�����롤
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"420_right\",\"wqy20020303\"\r\n"
//��֪������API
#define Weather_TCP		"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
//��֪����GET����
/*������� ����    ����Ϊ  Ӣ��*/
#define Weather_GET		"GET https://api.seniverse.com/v3/weather/now.json?key=SWuyJfw-Yx4hDPPih&location=xian&language=zh-Hans&unit=c\r\n"

#define Life_GET       "GET https://api.seniverse.com/v3/life/suggestion.json?key=SWuyJfw-Yx4hDPPih&location=xian&language=en&days=1\r\n"

//������̨��ȡʱ���API
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n"
#define Time_tcp        "AT+CIPSTART=\"TCP\",\"api.k780.com\",80\r\n"
//������̨��ȡʱ��GET����
#define Time_GET		"GET https://quan.suning.com/getSysTime.do\r\n"
#define Time_get     "GET http://api.k780.com/?app=life.time&appkey=70964&sign=e8b287188394862232a921a1e6f25135&format=json\r\n\r\n"


//ESP8266���ݴ��
unsigned char esp8266_buf[1300] = {0};
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
//�����������
unsigned char Weather_buff[300];   //λ�������ȷ����
//���ʱ������
unsigned char Time_buff[100];   //λ�������ȷ����
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
//�������ã�ESP8266_Init��ʼ������
//�������ƣ�ESP8266_Init(void);
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����16��18
//���ߣ�    ��ƨ��
/**************************************************************************/
void ESP8266_Init(void)
{
		
	ESP8266_Clear();
	
 /*��WIFI�Ƴ�͸��ģʽ*/
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
	//��һ��ESP8266��λ����
	
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
	
	/*sizeof(Weather_GET)��������sizeof��������strlenû����*/  	
	ESP8266_SendData((u8 *)Weather_GET, sizeof(Weather_GET)); //����AT+CIPSEND  �Լ� Weather_GET
	//UsartPrintf(USART_DEBUG, "Weather_Get OK\r\n");
	ESP8266_GetIPD_GET(200,Weather_buff);
	ESP8266_Clear();//�����������	
	UsartPrintf(USART_DEBUG, (char *)Weather_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*�˳�͸��ģʽ*/
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //��֤�Ƿ��˳�͸��ģʽ
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "�˳�͸��ģʽ�ɹ���\r\n");
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
	
	/*sizeof(Weather_GET)��������sizeof��������strlenû����*/  	
	ESP8266_SendData((u8 *)Life_GET, sizeof(Life_GET)); //����AT+CIPSEND  �Լ� Weather_GET
	//UsartPrintf(USART_DEBUG, "Weather_Get OK\r\n");
	ESP8266_GetIPD_GET(200,Life_buff);
	ESP8266_Clear();//�����������	
	UsartPrintf(USART_DEBUG, (char *)Life_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*�˳�͸��ģʽ*/
	delay_ms(500);
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //��֤�Ƿ��˳�͸��ģʽ
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "�˳�͸��ģʽ�ɹ���\r\n");
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
//�������ã���ȡ��̨ʱ��
//�������ƣ�Get_current_time(void);
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����20��32
//���ߣ�    ��ƨ��
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
	
/*sizeof(Weather_GET)��������sizeof��������strlenû����*/  	
//	ESP8266_SendData((u8 *)Time_GET, sizeof(Time_GET)); //����AT+CIPSEND  �Լ� Weather_GET
	ESP8266_SendData((u8 *)Time_get, sizeof(Time_get));
	
	ESP8266_GetIPD_GET(500,Time_buff);  //����������ȡ����
	ESP8266_Clear();//�����������	
	UsartPrintf(USART_DEBUG, (char *)Time_buff);
	delay_ms(500);
	delay_ms(500);
	while(ESP8266_SendCmd("+++", ""))      /*�˳�͸��ģʽ*/
	delay_ms(500);
	if(ProcessFlag == 0)
	{
		Process += 4; 
		LCD_ShowProcess(Process, BLACK);
	}
	UsartPrintf(USART_DEBUG, "+++ OK\r\n");	

	while(ESP8266_SendCmd("AT\r\n", "OK"))   //��֤�Ƿ��˳�͸��ģʽ
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
//�������ã����ڶ��жϺ���
//�������ƣ�USART2_IRQHandler();
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����4��18
//���ߣ�    ��ƨ��
/**************************************************************************/
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{

		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��				
		esp8266_buf[esp8266_cnt++] = USART2->DR;  

//		USART_SendData(USART1,USART2->DR);      //�ý��յ������ݴ�ӡ�ڴ���һ��
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}



//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}
//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}




//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	copy�������ݵ�Weather_buff��������
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		copy�������ݵ�Weather_buff
//==========================================================

unsigned char *ESP8266_GetIPD_GET(unsigned short timeOut,u8 *buff)//����������һ��ȫ�ֱ�����esp8266buf���浽���ȫ�ֱ�������
{
	do
	{
		delay_ms(5);													
	} while(timeOut--);
	strcpy((char*)buff,(char*)esp8266_buf);
	return buff;														
}

/*��δ�õ�*/
//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		delay_ms(5);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}
//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	uint16_t timeOut = 800;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{		
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
				return 0;
				
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND\r\n");		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		UsartPrintf(USART_DEBUG, "8.AT+CIPSEND\r\n");
		/*������������*/
		Usart_SendString(USART2, data, len);		//�����豸������������		
	}
}


