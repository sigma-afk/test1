����һ�д������
��ʼλ������һ�д���
#include "stc15.h"
#include "Key.h"
#include "oled.h"
#include "usemoduls.h"
sbit Led1 = P3^7;
sbit Led2 = P2^0;
//#define KEYPORT P1

volatile bit Key_long = 0;        //�г�������
volatile bit Key1_longPress = 0;  //����1����
volatile bit Key2_longPress = 0;  //����2����
volatile bit Key3_longPress = 0;  //����3����

volatile bit Key1_clickPress = 0;  //����1�̰�
volatile bit Key2_clickPress = 0;  //����2�̰�
volatile bit Key3_clickPress = 0;  //����3�̰�

volatile bit Key13_longPress = 0;  //����1��3��ϰ�
volatile bit Key12_longPress = 0;  //����1��2��ϰ�

volatile bit  PowerClose  = 0;          //��������
volatile bit Key_click = 0;


sbit Key1=P1^4;     //������
sbit Key2=P1^3;     //λѡ��
sbit Key3=P1^5;     //��ѡ��



 static unsigned char Key_Read(void)
{
	if(!Key3)
	{	
			return Key3_Press;   //����3����
	}
	 if(!Key2)
	{	
		return Key2_Press;//����2����
	}
	if(!Key1)
	{
		return Key1_Press;  //����1����
	}
	 return Key_None;//�ް�������

}
/*****************************************************************************
�������ƣ�����1��ȡ����ֵ����
��    �ܣ���ⰴ���������ذ���ֵ
�� �� ֵ��key1_press	
��    ע�����������̰�����ϰ�����װ��һ�������������ϰ���ʱ�������������ܱ�֤ͬʱ���µ�����
*****************************************************************************/
unsigned char Key1_Read(void)
{
	static char Key1_state = 0;//����1��ʼ״̬Ϊ0         �˴���staticĿ��Ϊ���水������ʱ��״̬
	static unsigned char Key_Last = 0,Key_Now = 0;
	static unsigned char Key1_time=0;             //�жϰ����İ���ʱ��
	unsigned char KeyValue = 0; //����1����ֵ          �˴�����static��Ϊ���ð����ɿ����´ΰ����������޷���ֵ
	Key_Now = Key_Read();
	switch(Key1_state)
	{
		case Key1_state_0:                 // ������ʼ̬
			if (Key_Now!= Key_Last)          //�ж��Ƿ��а�������
			{
				Key1_state = Key1_state_1;// �������£�״̬ת������ȷ��̬
			}
		break;	
			
    case Key1_state_1:                   // ����ȷ��̬
			if (Key_Now ==Key_Last )                 //ȷ���а�������
			{	
				Key1_time=0;
				KeyValue = Key_Short|Key_Last;      
				Key1_state = Key1_state_2;// ״̬ת������ֵȷ��̬
			}
			else
			{
				Key1_state = Key1_state_0; // �ް�������
			}
      break;					
			case Key1_state_2:                    //��������ȷ��״̬
				if(Key_Now==Key_Last)               //�����Ա�����
				{	
					Key1_time++;                     //������ʼ��ʱ
					if (Key1_time>100)   //�����������£�ʱ�䳬��1000ms���ж�Ϊ����
					{	
						Key1_time=0;
						KeyValue = Key_Long|Key_Last;    //����
						Key1_state=Key1_state_3;   //״̬ת�����ͷ�״̬
					}
				}
				else 
				{
					Key1_state = Key1_state_0;//��������
				}
				break;
			case Key1_state_3:                  //����ȷ��̬
				if(Key_Now!= Key_Last)
				{
					Key1_state=Key1_state_0;   //�������ͷţ�ת����������ʼ̬
				}
				break;
			default:
				break;			
	}
	Key_Last = Key_Now;
	return KeyValue;	                    //���ذ���ֵ
	
}

/*****************************************************************************
�������ƣ���ϰ�����ȡ����ֵ����
��    �ܣ������ϳ��������������ذ���ֵ
�� �� ֵ��	
��    ע��
*****************************************************************************/

void  KeyG_Group_Read()                //��ϰ������Ȱ�1,�ٳ���3
{
	static  char KeyG_state=0;
	static  char KeyG_time=0;
	unsigned char KeyG_return=0;
	
	switch(KeyG_state)
	{
		case KeyG_state_0:                         //���ڶ�������
			if (!Key1)                             //����1����
			{
					KeyG_state=KeyG_state_1;				
			}
			break;
		case KeyG_state_1:                        
			if (!Key1)             //�жϵ�һ�������͵���������
			{
				KeyG_time=0;
				KeyG_state=KeyG_state_2;
			}
			else
			{
				KeyG_state=KeyG_state_0;
			}
			break;
		case KeyG_state_2:                        //����һ���ڶ��������Ƿ񳤰�
			if (!Key1)                              //����һ������
			{
				KeyG_time++;
				if (KeyG_time>100)
				{
					KeyG_state=KeyG_state_3;
//					OLED_DrawBMP(20,2,71,4,BMP1);
					OLED_Power_On();         //������ʾ
//					GPIO_PWMOUT(40000);
					KeyG_time=0;
				}
			}
			break;
		case KeyG_state_3:
			if (!Key1)                   
			{
				if(!Key3)
				{
					KeyG_state = KeyG_state_4;     //���������״̬
					KeyG_time=0;
				}
			}
			else 
			{				
				KeyG_state = KeyG_state_0;     //����ԭʼ״̬
				PowerClose = 1;					
				Key1_longPress = 1;  //����1����

			}
		
		break;	
		case KeyG_state_4:                //Key3����
			if(!Key3)
				{
					KeyG_time++;
					if (KeyG_time>100)
					{
						OLED_Clear();
						PowerClose = 1;
						Key13_longPress = 1;//����1��3����
						Key1_longPress = 0;
						KeyG_state = KeyG_state_5;	
						OLED_ShowCHinese(0,2,0);//��
						OLED_ShowCHinese(18,2,1);//��
						OLED_ShowString(40,2,":",16);
						OLED_ShowNum(50,2,pw[3],1,16);
						OLED_ShowNum(60,2,pw[2],1,16);
						OLED_ShowNum(70,2,pw[1],1,16);
						OLED_ShowNum(80,2,pw[0],1,16);
						KeyG_time=0;
					}
				}
				break;
		case KeyG_state_5:
		{
			if(Key3)
			{
				KeyG_state = KeyG_state_0;     //����ԭʼ��״̬
			}
		}
		break;
	}
}

#if 0
/*****************************************************************************
�������ƣ�����2��ȡ����ֵ����
��    �ܣ���ⰴ���������ذ���ֵ
�� �� ֵ��key2_press	
��    ע��
*****************************************************************************/
unsigned char Key13_Read(void)
{
	static  char Key13_state=0;//����1��ʼ״̬Ϊ0
	static  char Key13_time=0; //�жϰ����İ���ʱ��
	unsigned char Key13_Value=0; //����1����ֵ
	static unsigned char Key13_Now = 0;
	Key13_Now = Key_Read();
	switch(Key13_state)
	{
		case Key13_state_0:                 // ������ʼ̬
			if (Key13_Now == 0x01)
			{
				Key13_state = Key13_state_1;// �������£�״̬ת������ȷ��̬
			}
		break;	
			
    case Key13_state_1:                   // ����ȷ��̬
			if (Key13_Now==0x10)
			{		
				Key13_time=0;
				//Key1_return = 1;         // �����԰��£�����ȷ�����Ϊ��1��
         Key13_state = Key13_state_2;// ״̬ת�������ͷ�̬
      }
			else
				{
					Key13_state = Key13_state_0; // ������̧��ת����������ʼ̬
			  }
      break;	
				
		case Key13_state_2:
			if(Key13_Now!=0x10) 
			{
//				Key2_return=Key2_click;    //�жϰ���Ϊ�̰�������ֵΪ0
			  Key13_state = Key13_state_0; //ת����������ʼ״̬
      }
			else 
			{
				Key13_time++;
				if (Key13_time>100)   //�����������£�ʱ�䳬��2000ms���ж�Ϊ����
				{
					Key13_time=0;
					Key13_Value=Key13_long;			//����ֵΪ1
					Key13_state=Key13_state_3;
				}
			}
      break;
				
			case Key13_state_3:
				if (Key13_Now!=0x10)
				{
					Key13_state=Key13_state_0;   //�������ͷţ�ת����������ʼ̬
				}
				break;
			default:
				break;
	}
	return Key13_Value;	                    //���ذ���ֵ
}

/*****************************************************************************
�������ƣ�����3��ȡ����ֵ����
��    �ܣ���ⰴ���������ذ���ֵ
�� �� ֵ��key3_press	
��    ע��
*****************************************************************************/
	
unsigned char Key3_Read(void)
{
	static  char Key3_state=0;//����1��ʼ״̬Ϊ0
	static  char Key3_time=0; //�жϰ����İ���ʱ��
	unsigned char Key3_return=0; //����1����ֵ
	
	switch(Key3_state)
	{
		case Key3_state_0:                 // ������ʼ̬
			if (!Key3)
			{
				Key3_state = Key3_state_1;// �������£�״̬ת������ȷ��̬
			}
		break;	
			
    case Key3_state_1:                   // ����ȷ��̬
			if (!Key3)
			{		
				Key3_time=0;
				//Key1_return = 1;         // �����԰��£�����ȷ�����Ϊ��1��
         Key3_state = Key3_state_2;// ״̬ת�������ͷ�̬
      }
			else
				{
					Key3_state = Key3_state_0; // ������̧��ת����������ʼ̬
			  }
      break;	
				
		case Key3_state_2:
			if(Key3) 
			{
				Key3_return=Key3_click;    //�жϰ���Ϊ�̰�������ֵΪ0
			  Key3_state = Key3_state_0; //ת����������ʼ״̬
      }
			else 
			{
				Key3_time++;
				if (Key3_time>100)   //�����������£�ʱ�䳬��1000ms���ж�Ϊ����
				{
					Key3_time=0;
					Key3_return=Key1_long;			//����ֵΪ1
					Key3_state=Key1_state_3;
				}
			}
      break;
				
				
			case Key3_state_3:
				if (Key3)
				{
					Key3_state=Key3_state_0;   //�������ͷţ�ת����������ʼ̬
				}
				break;
			default:
				break;
	}
	return Key3_return;	                    //���ذ���ֵ
}

/*****************************************************************************
�������ƣ���ϰ�����ȡ����ֵ����
��    �ܣ������ϳ��������������ذ���ֵ
�� �� ֵ��	
��    ע��
*****************************************************************************/

unsigned char Key_Group_Read(void)                //��ϰ������Ȱ�1,�ٳ���3
{
	static  char KeyG_state=0;
	static  char KeyG_time=0;
	unsigned char KeyG_return=0;
	
	switch(KeyG_state)
	{
		case KeyG_state_0:                         //���ڶ�������
			if (!Key1)                             //����1����
			{

					KeyG_state=KeyG_state_1;
				
			}
			break;
		case KeyG_state_1:                        
			if (!Key1)             //�жϵ�һ�������͵���������
			{
					KeyG_time=0;
					KeyG_state=KeyG_state_2;
			}
			else
			{
				KeyG_state=KeyG_state_0;
			}
			break;
		case KeyG_state_2:                        //����һ���ڶ��������Ƿ񳤰�
			if (!Key1)                              //����һ������
			{
				KeyG_time++;
				if (KeyG_time>100)
				{
					KeyG_state=KeyG_state_3;
					KeyG_return=KeyG_group_long;        //���������־λ
					OLED_Power_On();         //������ʾ
					KeyG_time=0;
				}
			}
			break;
		case KeyG_state_3:
			if ((Key1))                   
			{
				KeyG_state = KeyG_state_0;     //����ԭʼ״̬
				PowerClose = 1;
			}
			else
			{
				if(!Key3)
				{
					KeyG_state = KeyG_state_4;     //���������״̬
					KeyG_time=0;
				}
			}		
		break;	
		case KeyG_state_4:                //Key3����
			if(!Key3)
				{
					KeyG_time++;
					if (KeyG_time>100)
					{
						KeyG_state=KeyG_state_5;	
						OLED_ShowCHinese(0,2,0);//��
						OLED_ShowCHinese(18,2,1);//��
						OLED_ShowString(40,2,":",16);
						OLED_ShowNum(50,2,pw[3],1,16);
						OLED_ShowNum(60,2,pw[2],1,16);
						OLED_ShowNum(70,2,pw[1],1,16);
						OLED_ShowNum(80,2,pw[0],1,16);
						KeyG_time=0;
					}
				}
				break;
		case KeyG_state_5:
		{
			if(Key3)
			{
				KeyG_state = KeyG_state_0;     //����ԭʼ��״̬
			}
		}
		break;
	}
}

/*****************************************************************************
�������ƣ���ϰ���2��3��ȡ����ֵ����
��    �ܣ������ϰ���2��3�������ذ���ֵ
�� �� ֵ��	
��    ע��
*****************************************************************************/
unsigned char Key_Group23_Read(void)      
{
	static  char Key23_state=0;
	static  char Key23_time=0;
	unsigned char Key23_return=0;
	
	switch(Key23_state)
	{
		case  Key23_state_0:                     //����2��3��ʼ״̬
			if (!Key2||Key3)                       //����2��3��һ�ΰ���
			{
				Key23_state=Key23_state_1;
			}
		break;
			
		case Key23_state_1:                      //����2��3ȷ�ϱ�����
			if (!Key2&&!Key3)
			{
				Key23_time=0;
				Key23_state=Key23_state_2;
			}
			else
			{
				Key23_state=Key23_state_0;
			}
		break;
			
		case Key23_state_2:                    //����2��3ȷ���Ƿ񳤰�
			if (!Key2&&!Key3)
			Key23_time++;
			if (Key23_time>80)
			{
				Key23_state=Key23_state_3;
				Key23_return=Key23_group_long;
				Key23_time=0;
			}
		break;
			
		case Key23_state_3: 
			if ((Key2)&&(Key3))
			{
				Key23_state=Key23_state_0;
			}
		break;
			
		}
	return Key23_return;
	
}

#endif
/*****************************************************************************
�������ƣ�ScanKeyCode()
��    �ܣ�ɨ�谴��
�� �� ֵ��	
��    ע��
*****************************************************************************/
void ScanKeyCode()
{
	unsigned char Keycode = Key1_Read();
//	OLED_ShowNum()
	
	/*��ÿ�ΰ���ɨ���ʱ�򣬰����б�־λ����λ����ֹ���𰴼���ǰ���£������˶���*/
	
//	if(Key1_clickPress)  Key1_clickPress = 0;   
//	if(Key2_clickPress)  Key2_clickPress = 0;
//	if(Key3_clickPress)  Key3_clickPress = 0;
//	if(Key1_longPress)   Key1_longPress = 0;
//	if(Key2_longPress)   Key2_longPress = 0;
//	if(Key3_longPress)   Key3_longPress = 0;
	
	
	if(Keycode ==Key_None) return;
	
	if(Keycode&Key_Short)   //�̰�
	{
		if(Keycode&Key1_Press)  //����1����
		{
//			OLED_ShowNum(0,2,0,1,16);
//			Led1 = !Led1;
				Key1_clickPress = 1;//����1�̰�
				
		}
		else if(Keycode&Key2_Press)   //����2�̰�
		{
//			OLED_ShowNum(0,2,1,1,16);
				Key2_clickPress = 1;  //����2�̰�

		}
		else if(Keycode&Key3_Press)   //����3�̰�
		{
//			OLED_ShowNum(0,2,2,1,16);
				Key3_clickPress = 1;  //����3����
		}
	}
	
	else if(Keycode&Key_Long)   //����
	{
		Key_long = 1;   //�г�����
		if(Keycode&Key1_Press)  //����1����
		{
			Key1_longPress = 1;  //����1����
		}

		 if (Keycode&Key3_Press)
		{
			Key3_longPress = 1;  //����3����
		}
	}
	
}

����һ�д���
���Ӷ��д���
�������д���
