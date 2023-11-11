#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"
#include "delay.h"
#include "pic.h"
#include "W25Q64.h"
#include <string.h>
#include <stdio.h>
//#include "weather_pic.h"

extern uint8_t Hour;
extern uint8_t Minute;
extern uint8_t Second;

extern uint8_t Weather_Code;
extern uint8_t gImage[4096];
extern unsigned char Time_buff[300];

uint16_t strRam[128];
uint16_t G_Ram[][128];
extern char Life_results[][50];

char buff[50] = {0};
char resultsBuff[20] = {0};

/******************************************************************************
      ����˵������ָ�����������ɫ
      ������ݣ�xsta,ysta   ��ʼ����
                xend,yend   ��ֹ����
								color       Ҫ������ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//������ʾ��Χ
	for(i=ysta;i<yend;i++)
	{													   	 	
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	} 					  	    
}

/******************************************************************************
      ����˵������ָ��λ�û���
      ������ݣ�x,y ��������
                color �����ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//���ù��λ�� 
	LCD_WR_DATA(color);
} 


/******************************************************************************
      ����˵��������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
                color   �ߵ���ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/******************************************************************************
      ����˵����������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
                color   ���ε���ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      ����˵������Բ
      ������ݣ�x0,y0   Բ������
                r       �뾶
                color   Բ����ɫ
      ����ֵ��  ��
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//�ж�Ҫ���ĵ��Ƿ��Զ
		{
			b--;
		}
	}
}

/******************************************************************************
      ����˵������ʾ���ִ�
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ��ִ�
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ� ��ѡ 16 24 32
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
		if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;
		x+=sizey;
	}
}

/******************************************************************************
      ����˵������ʾ����16x16����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;//���㷨ֻ�������ֿ�����ָߣ����ָ���8�ı������֣�
	                          //Ҳ�����û�ʹ��������С����,������ʾ���׳����⣡
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else//���ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
} 


/******************************************************************************
      ����˵������ʾ����24x24����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;//���㷨ֻ�������ֿ�����ָߣ����ָ���8�ı������֣�
	                          //Ҳ�����û�ʹ��������С����,������ʾ���׳����⣡
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else//���ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
} 

/******************************************************************************
      ����˵������ʾ����32x32����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;//���㷨ֻ�������ֿ�����ָߣ����ָ���8�ı������֣�
	                          //Ҳ�����û�ʹ��������С����,������ʾ���׳����⣡
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else//���ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
}


/******************************************************************************
      ����˵������ʾ�����ַ�
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ���ַ�
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t;
	u16 i,TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	sizex=sizey/2;
	TypefaceNum=sizex/8*sizey;
	num=num-' ';    //�õ�ƫ�ƺ��ֵ
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //���ù��λ�� 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==16)temp=ascii_1608[num][i];		       //����8x16����
		else if(sizey==32)temp=ascii_3216[num][i];		 //����16x32����
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//�ǵ���ģʽ
			{
				if(temp&(0x01<<t))LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
			}
			else//����ģʽ
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//��һ����
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}

void LCD_Refresh_Chinese(u16 fc,u16 bc)
{
	uint8_t i,j;
	for(i=0; i<128; i++)
	{
		LCD_Address_Set(i,108,i,123);
		for(j=0;j<16;j++)
		{	
			if(strRam[i]&(0x8000>>j))LCD_WR_DATA(fc);
			else LCD_WR_DATA(bc);
		}
	}	
}

void LCD_Refresh_Ram(u16 fc,u16 bc)
{
	uint8_t i,j;
	for(i=0 ;i<16; i++)	
	{
		for(j=0 ;j<128; j++)
		{
			if(strRam[j]&(0x8000>>i))G_Ram[i][j] = fc;
			else G_Ram[i][j] = fc;
		}
	}
	
}

/******************************************************************************
      ����˵������ʾ�����ַ�
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ���ַ�
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowRollChar(u8 *s,u16 fc,u16 bc)
{
	u8 i,n=0;
	u16 k;
	u16 HZnum;//������Ŀ
	                        
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//ͳ�ƺ�����Ŀ

	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 				
			for(n=0; n<16; n++)
			{
				for(i=0; i<16; i++)
				{
					if(n <= 7)
					{
						if(tfont16[k].Msk[i*2] & (0x01<<n))
						{
							strRam[127] |= 1;
						}
						if(i*2 == 30)break;
						strRam[127]  = strRam[127] << 1;
					}
					else
					{
						if(tfont16[k].Msk[i*2+1] & (0x01<<(n-8)))
						{
							strRam[127] |= 1;
						}
						if(i*2+1 == 31)break;
						strRam[127]  = strRam[127] << 1;
					}

				}
				delay_ms(25);
			}
			break;
		}
		
	}
}

/******************************************************************************
      ����˵����������ʾ�ַ���
      ������ݣ�x,y��ʾ����
                *p Ҫ��ʾ���ַ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowRollString(u8 *p,u16 fc,u16 bc)
{         
	while(*p!='\0')
	{       
		LCD_ShowRollChar(p,fc,bc);
		p++;
	}  
}

/******************************************************************************
      ����˵������ʾ�ַ���
      ������ݣ�x,y��ʾ����
                *p Ҫ��ʾ���ַ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}


/******************************************************************************
      ����˵������ʾ����
      ������ݣ�m������nָ��
      ����ֵ��  ��
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}


/******************************************************************************
      ����˵������ʾ��������
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ��������
                len Ҫ��ʾ��λ��
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
} 

/******************************************************************************
      ����˵������ʾ16���Ʊ���
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ16���Ʊ���
                len Ҫ��ʾ��λ��
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowHexNum(u16 x,u16 y,u32 num,u8 len,u16 fc,u16 bc,u8 sizey)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < len; i++)							
	{
		SingleNumber = num / mypow(16, len - i - 1) % 16;
		if (SingleNumber < 10)
		{
			LCD_ShowChar(x+8*i, y, SingleNumber + '0', fc, bc, sizey, 0);
		}
		else
		{
			LCD_ShowChar(x+8*i, y, SingleNumber - 10 + 'A', fc, bc, sizey, 0);
		}
	}
}

/******************************************************************************
      ����˵������ʾ��λС������
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾС������
                len Ҫ��ʾ��λ��
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}


/******************************************************************************
      ����˵������ʾͼƬ
      ������ݣ�x,y�������
                length ͼƬ����
                width  ͼƬ���
                pic[]  ͼƬ����    
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
	u16 i,j,k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);
			LCD_WR_DATA8(pic[k*2+1]);
			k++;
		}
	}			
}

void LCD_ShowTime(u16 x,u16 y)
{
	
	uint8_t H_Num, L_Num, i;
	H_Num = Hour / 10;
	L_Num = Hour % 10;
	LCD_ShowPicture(x, y, 14, 20, gImage_Num[H_Num]);
	LCD_ShowPicture(x+15, y, 14, 20, gImage_Num[L_Num]);

	H_Num = Minute / 10;
	L_Num = Minute % 10;
	LCD_ShowPicture(x+37, y, 14, 20, gImage_Num[H_Num]);
	LCD_ShowPicture(x+52, y, 14, 20, gImage_Num[L_Num]);

	H_Num = Second / 10;
	L_Num = Second % 10;
	LCD_ShowPicture(x+74, y, 14, 20, gImage_Num[H_Num]);
	LCD_ShowPicture(x+89, y, 14, 20, gImage_Num[L_Num]);
	/*******************************************************/
	
	LCD_DrawPoint(x+32, y+5, BLACK);
	LCD_DrawPoint(x+33, y+5, BLACK);
	LCD_DrawPoint(x+32, y+6, BLACK);
	LCD_DrawPoint(x+33, y+6, BLACK);
	
	LCD_DrawPoint(x+32, y+13, BLACK);
	LCD_DrawPoint(x+33, y+13, BLACK);
	LCD_DrawPoint(x+32, y+14, BLACK);
	LCD_DrawPoint(x+33, y+14, BLACK);
	
	LCD_DrawPoint(x+70, y+5, BLACK);
	LCD_DrawPoint(x+71, y+5, BLACK);
	LCD_DrawPoint(x+70, y+6, BLACK);
	LCD_DrawPoint(x+71, y+6, BLACK);
	
	LCD_DrawPoint(x+70, y+13, BLACK);
	LCD_DrawPoint(x+71, y+13, BLACK);
	LCD_DrawPoint(x+70, y+14, BLACK);
	LCD_DrawPoint(x+71, y+14, BLACK);
	
	LCD_Address_Set(0,y+23,128,y+23);
	for(i=0;i<128;i++)
	{
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x00);	
	}
}

void LCD_ShowWeather(u16 x, u16 y, u8 code, u8 temperature)
{
	uint8_t blockNum,i;
	switch(code)
	{
		case 2:code = 0;break;
		case 3:code = 1;break;
		case 7:code = 5;break;
		case 8:code = 6;break;
		case 18:code = 17;break;
		case 27:code = 26;break;
		case 29:code = 28;break;
		case 33:code = 32;break;
		case 35:code = 34;break;
		default:break;
	}
	if(code <=15)
	{
		blockNum = 1;
	}
	else if(code >=16 && code <= 31)
	{
		blockNum = 2;
	}
	else 
	{
		blockNum = 3;
	}
	W25Q64_ReadSectorData(blockNum, code-16*(blockNum-1), gImage);
	LCD_ShowPicture(x, y+10, 43, 43, gImage);
	W25Q64_ReadSectorData(4, 0, gImage);
	LCD_ShowPicture(x+55, y+32, 25, 25, gImage);
	LCD_ShowString(x+55, y+6, "        ", BLACK, WHITE, 16, 0);
	
	LCD_ShowIntNum(x+83, y+37, temperature, 2, BLACK, WHITE, 16);
	LCD_ShowChinese(x+100, y+37, "��", BLACK, WHITE, 16, 0);
	switch(code)
	{
		case 0:
		case 1:LCD_ShowChinese(x+79, y+6, "��", BLACK, WHITE, 16, 0);break;
		case 4:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 5:                  
		case 6:LCD_ShowChinese(x+55, y+6, "������", BLACK, WHITE, 16, 0);break;
		case 9:LCD_ShowChinese(x+79, y+6, "��", BLACK, WHITE, 16, 0);break;
		case 10:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 11:LCD_ShowChinese(x+63, y+6, "������", BLACK, WHITE, 16, 0);break;
		case 13:LCD_ShowChinese(x+71, y+6, "С��", BLACK, WHITE, 16, 0);break;
		case 14:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 15:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 16:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 17:LCD_ShowChinese(x+63, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 19:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 20:LCD_ShowChinese(x+63, y+6, "���ѩ", BLACK, WHITE, 16, 0);break;
		case 21:LCD_ShowChinese(x+71, y+6, "��ѩ", BLACK, WHITE, 16, 0);break;
		case 22:LCD_ShowChinese(x+71, y+6, "Сѩ", BLACK, WHITE, 16, 0);break;
		case 23:LCD_ShowChinese(x+71, y+6, "��ѩ", BLACK, WHITE, 16, 0);break;
		case 24:LCD_ShowChinese(x+71, y+6, "��ѩ", BLACK, WHITE, 16, 0);break;
		case 25:LCD_ShowChinese(x+71, y+6, "��ѩ", BLACK, WHITE, 16, 0);break;
		case 26:LCD_ShowChinese(x+71, y+6, "����", BLACK, WHITE, 16, 0);break;
		case 28:LCD_ShowChinese(x+63, y+6, "ɳ����", BLACK, WHITE, 16, 0);break;
		case 30:LCD_ShowChinese(x+79, y+6, "��", BLACK, WHITE, 16, 0);break;
		case 31:LCD_ShowChinese(x+79, y+6, "��", BLACK, WHITE, 16, 0);break;
		case 32:LCD_ShowChinese(x+71, y+6, "���", BLACK, WHITE, 16, 0);break;
		case 34:LCD_ShowChinese(x+71, y+6, "쫷�", BLACK, WHITE, 16, 0);break;
		case 36:LCD_ShowChinese(x+63, y+6, "�����", BLACK, WHITE, 16, 0);break;
		default:break;
	}
	
	LCD_Address_Set(x+53, y, x+53, y+60);
	for(i=0;i<63;i++)
	{
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x00);	
	}
	LCD_Address_Set(x+53, y+28, x+118, y+28);
	for(i=0;i<65;i++)
	{
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x00);	
	}
	LCD_Address_Set(0, y+60, 128, y+60);
	for(i=0;i<128;i++)
	{
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x00);	
	}
}

void LCD_ShowDate(u16 x, u16 y, u16 fc,u16 bc,u8 sizey, uint16_t Year, uint8_t Month, uint8_t Day)
{
//	LCD_ShowChinese(x+32, y, "��", fc, bc, sizey, 0);
//	LCD_ShowChinese(x+64, y, "��", fc, bc, sizey, 0);
//	LCD_ShowChinese(x+96, y, "��", fc, bc, sizey, 0);
//	LCD_ShowIntNum(x, y, Year, 4, fc, bc, sizey);
//	LCD_ShowIntNum(x+48, y, Month, 2, fc, bc, sizey);
//	LCD_ShowIntNum(x+80, y, Day, 2, fc, bc, sizey);
	
	uint8_t Week,i;
	LCD_ShowChinese(x+16, y, "��", fc, bc, sizey, 0);
	LCD_ShowChinese(x+48, y, "��", fc, bc, sizey, 0);
	LCD_ShowIntNum(x, y, Month, 2, fc, bc, sizey);
	LCD_ShowIntNum(x+32, y, Day, 2, fc, bc, sizey);
	LCD_ShowChinese(x+96, y, "��", fc, bc, sizey, 0);
	Week = Time_buff[144] - '0';
	switch(Week)
	{
		case 0:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		case 1:LCD_ShowChinese(x+112, y, "һ", fc, bc, sizey, 0);break;
		case 2:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		case 3:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		case 4:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		case 5:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		case 6:LCD_ShowChinese(x+112, y, "��", fc, bc, sizey, 0);break;
		default:break;
	}	
	LCD_Address_Set(x,y+18,x+128,y+18);
	for(i=0;i<128;i++)
	{
		LCD_WR_DATA8(0x00);
		LCD_WR_DATA8(0x00);	
	}
}

void LCD_ShowLife(void)
{
	uint8_t i;
	for(i=0; i<5; i++)
	{
		if(!strcmp(Life_results[i], "Not Good"))strcpy(resultsBuff , "����");
		else if(!strcmp(Life_results[i], "Good"))strcpy(resultsBuff , "����");
		else if(!strcmp(Life_results[i], "Suitable"))strcpy(resultsBuff , "����");
		else if(!strcmp(Life_results[i], "Great"))strcpy(resultsBuff , "����");
		else if(!strcmp(Life_results[i], "Relatively suitable"))strcpy(resultsBuff , "������");
		else if(!strcmp(Life_results[i], "Average"))strcpy(resultsBuff , "һ��");
		else if(!strcmp(Life_results[i], "Unfavorable"))strcpy(resultsBuff , "����");
		else if(!strcmp(Life_results[i], "Relatively unsuitable"))strcpy(resultsBuff , "�ϲ���");
		else if(!strcmp(Life_results[i], "Unsuitable"))strcpy(resultsBuff, "������");
		else if(!strcmp(Life_results[i], "Comfortable"))strcpy(resultsBuff, "����");
		else if(!strcmp(Life_results[i], "Relatively comfortable"))strcpy(resultsBuff, "������");
		else if(!strcmp(Life_results[i], "Very Comfortable"))strcpy(resultsBuff, "������");
		else if(!strcmp(Life_results[i], "Relatively uncomfortable"))strcpy(resultsBuff , "�ϲ�����");
		else if(!strcmp(Life_results[i], "Uncomfortable"))strcpy(resultsBuff , "������");
		else if(!strcmp(Life_results[i], "Very uncomfortable"))strcpy(resultsBuff, "�ܲ�����");
		else if(!strcmp(Life_results[i], "Extremely uncomfortable"))strcpy(resultsBuff , "�ǳ�������");
		else if(!strcmp(Life_results[i], "Unlikely"))strcpy(resultsBuff , "���׷�");
		else if(!strcmp(Life_results[i], "Very Unlikely"))strcpy(resultsBuff , "�����׷�");
		else if(!strcmp(Life_results[i], "Rarely"))strcpy(resultsBuff , "�ٷ�");
		else strcpy(resultsBuff , "δƥ��");
		
		switch(i)
		{
			case 0: sprintf(buff, "��ɹ��%s  ", resultsBuff);break;
			case 1: sprintf(buff, "������%s  ", resultsBuff);break;
			case 2: sprintf(buff, "ơ�ƣ�%s  ", resultsBuff);break;
			case 3: sprintf(buff, "���ʶȣ�%s  ", resultsBuff);break;
			case 4: sprintf(buff, "������%s  ", resultsBuff);break;
			default:break;
		}
		LCD_ShowRollString((u8 *)buff, BLACK, WHITE);
	}
}

void LCD_ShowProcess(uint16_t Num, u16 color)
{
	uint16_t i,j;
	LCD_DrawLine(12, 50, 116, 50, color);
	LCD_DrawLine(12, 51, 116, 51, color);
	LCD_DrawLine(12, 62, 118, 62, color);
	LCD_DrawLine(12, 63, 118, 63, color);
	LCD_DrawLine(12, 50, 12, 63, color);
	LCD_DrawLine(13, 50, 13, 63, color);
	LCD_DrawLine(116, 50, 116, 63, color);
	LCD_DrawLine(117, 50, 117, 63, color);
	
	LCD_ShowString(13, 66, "Waiting...", color, WHITE, 16, 0);
	
	if((0 < Num) && (Num <= 100))
	{
		for(i=1; i<Num; i++ )
		{
			LCD_Address_Set(i+14, 53, i+14, 60);
			for(j=0; j<8; j++)
			{
				LCD_WR_DATA(color);
			}
			delay_ms(10);
		}
	}
	

}
