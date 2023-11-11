#include "stm32f10x.h"                  // Device header
#include "stm32f10x_tim.h"
#include "lcd.h"

extern uint8_t Hour;
extern uint8_t Minute;
extern uint8_t Second;
extern uint16_t count;
extern uint16_t Life_count;
extern uint8_t TimeGetFlag;
extern uint8_t TimeShowFlag;
extern uint8_t LifeShowFlag;

extern uint16_t strRam[128];

void Timer2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

void Timer3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 2500-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Second++;
		count++;
		Life_count++;
		if(Second >= 60)
		{
			Second = 0;
			Minute++;
			if(Minute == 60)
			{
				Minute = 0;
				Hour++;
				if(Hour == 24)
				{
					Hour = 0;
					TimeGetFlag = 1;
				}
			}
		}
		LCD_ShowTime(13, 22);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM3_IRQHandler(void)
{
	uint8_t i;
	static uint8_t count_25ms;
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		count_25ms++;
		if(count_25ms >= 39)
		{
			count_25ms = 0;
			Second++;
			count++;
			Life_count++;
			if(Second >= 60)
			{
				Second = 0;
				Minute++;
				if(Minute == 60)
				{
					Minute = 0;
					Hour++;
					if(Hour == 24)
					{
						Hour = 0;
						TimeGetFlag = 1;
					}
				}
			}
			if(TimeShowFlag == 1)
			{
				LCD_ShowTime(13, 22);
			}
		}
		if(LifeShowFlag == 1)
		{
			LCD_Refresh_Chinese(BLACK, WHITE);
			for(i=0; i<127; i++)
			{
				strRam[i] = strRam[i+1];
			}
			strRam[127] = 0;
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
