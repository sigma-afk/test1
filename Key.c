增加一行错误代码
初始位置增加一行代码
#include "stc15.h"
#include "Key.h"
#include "oled.h"
#include "usemoduls.h"
sbit Led1 = P3^7;
sbit Led2 = P2^0;
//#define KEYPORT P1

volatile bit Key_long = 0;        //有长长按键
volatile bit Key1_longPress = 0;  //按键1长按
volatile bit Key2_longPress = 0;  //按键2长按
volatile bit Key3_longPress = 0;  //按键3长按

volatile bit Key1_clickPress = 0;  //按键1短按
volatile bit Key2_clickPress = 0;  //按键2短按
volatile bit Key3_clickPress = 0;  //按键3短按

volatile bit Key13_longPress = 0;  //按键1，3组合按
volatile bit Key12_longPress = 0;  //按键1，2组合按

volatile bit  PowerClose  = 0;          //开机过程
volatile bit Key_click = 0;


sbit Key1=P1^4;     //开机键
sbit Key2=P1^3;     //位选键
sbit Key3=P1^5;     //数选键



 static unsigned char Key_Read(void)
{
	if(!Key3)
	{	
			return Key3_Press;   //按键3按下
	}
	 if(!Key2)
	{	
		return Key2_Press;//按键2按下
	}
	if(!Key1)
	{
		return Key1_Press;  //按键1按下
	}
	 return Key_None;//无按键按下

}
/*****************************************************************************
函数名称：按键1获取返回值程序
功    能：检测按键，并返回按键值
返 回 值：key1_press	
备    注：将长按，短按，组合按键封装成一个函数，解决组合按键时，两个按键不能保证同时按下的问题
*****************************************************************************/
unsigned char Key1_Read(void)
{
	static char Key1_state = 0;//按键1初始状态为0         此处加static目的为保存按键按下时的状态
	static unsigned char Key_Last = 0,Key_Now = 0;
	static unsigned char Key1_time=0;             //判断按键的按键时间
	unsigned char KeyValue = 0; //按键1返回值          此处不加static是为了让按键松开后到下次按键按下中无返回值
	Key_Now = Key_Read();
	switch(Key1_state)
	{
		case Key1_state_0:                 // 按键初始态
			if (Key_Now!= Key_Last)          //判断是否有按键按下
			{
				Key1_state = Key1_state_1;// 键被按下，状态转换到键确认态
			}
		break;	
			
    case Key1_state_1:                   // 按键确认态
			if (Key_Now ==Key_Last )                 //确认有按键按下
			{	
				Key1_time=0;
				KeyValue = Key_Short|Key_Last;      
				Key1_state = Key1_state_2;// 状态转换到键值确认态
			}
			else
			{
				Key1_state = Key1_state_0; // 无按键按下
			}
      break;					
			case Key1_state_2:                    //按键长按确认状态
				if(Key_Now==Key_Last)               //按键仍被按下
				{	
					Key1_time++;                     //长按开始计时
					if (Key1_time>100)   //按键持续按下，时间超过1000ms，判断为长按
					{	
						Key1_time=0;
						KeyValue = Key_Long|Key_Last;    //长按
						Key1_state=Key1_state_3;   //状态转换到释放状态
					}
				}
				else 
				{
					Key1_state = Key1_state_0;//跳出按键
				}
				break;
			case Key1_state_3:                  //按键确认态
				if(Key_Now!= Key_Last)
				{
					Key1_state=Key1_state_0;   //按键已释放，转换到按键初始态
				}
				break;
			default:
				break;			
	}
	Key_Last = Key_Now;
	return KeyValue;	                    //返回按键值
	
}

/*****************************************************************************
函数名称：组合按键获取返回值程序
功    能：检测组合长按按键，并返回按键值
返 回 值：	
备    注：
*****************************************************************************/

void  KeyG_Group_Read()                //组合按键，先按1,再长按3
{
	static  char KeyG_state=0;
	static  char KeyG_time=0;
	unsigned char KeyG_return=0;
	
	switch(KeyG_state)
	{
		case KeyG_state_0:                         //检测第二个按键
			if (!Key1)                             //按键1按下
			{
					KeyG_state=KeyG_state_1;				
			}
			break;
		case KeyG_state_1:                        
			if (!Key1)             //判断第一个按键和第三个按键
			{
				KeyG_time=0;
				KeyG_state=KeyG_state_2;
			}
			else
			{
				KeyG_state=KeyG_state_0;
			}
			break;
		case KeyG_state_2:                        //检测第一个第二个按键是否长按
			if (!Key1)                              //检测第一个按键
			{
				KeyG_time++;
				if (KeyG_time>100)
				{
					KeyG_state=KeyG_state_3;
//					OLED_DrawBMP(20,2,71,4,BMP1);
					OLED_Power_On();         //开机显示
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
					KeyG_state = KeyG_state_4;     //进入第三个状态
					KeyG_time=0;
				}
			}
			else 
			{				
				KeyG_state = KeyG_state_0;     //进入原始状态
				PowerClose = 1;					
				Key1_longPress = 1;  //按键1长按

			}
		
		break;	
		case KeyG_state_4:                //Key3长按
			if(!Key3)
				{
					KeyG_time++;
					if (KeyG_time>100)
					{
						OLED_Clear();
						PowerClose = 1;
						Key13_longPress = 1;//按键1，3长按
						Key1_longPress = 0;
						KeyG_state = KeyG_state_5;	
						OLED_ShowCHinese(0,2,0);//密
						OLED_ShowCHinese(18,2,1);//码
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
				KeyG_state = KeyG_state_0;     //进入原始个状态
			}
		}
		break;
	}
}

#if 0
/*****************************************************************************
函数名称：按键2获取返回值程序
功    能：检测按键，并返回按键值
返 回 值：key2_press	
备    注：
*****************************************************************************/
unsigned char Key13_Read(void)
{
	static  char Key13_state=0;//按键1初始状态为0
	static  char Key13_time=0; //判断按键的按键时间
	unsigned char Key13_Value=0; //按键1返回值
	static unsigned char Key13_Now = 0;
	Key13_Now = Key_Read();
	switch(Key13_state)
	{
		case Key13_state_0:                 // 按键初始态
			if (Key13_Now == 0x01)
			{
				Key13_state = Key13_state_1;// 键被按下，状态转换到键确认态
			}
		break;	
			
    case Key13_state_1:                   // 按键确认态
			if (Key13_Now==0x10)
			{		
				Key13_time=0;
				//Key1_return = 1;         // 按键仍按下，按键确认输出为“1”
         Key13_state = Key13_state_2;// 状态转换到键释放态
      }
			else
				{
					Key13_state = Key13_state_0; // 按键已抬起，转换到按键初始态
			  }
      break;	
				
		case Key13_state_2:
			if(Key13_Now!=0x10) 
			{
//				Key2_return=Key2_click;    //判断按键为短按，返回值为0
			  Key13_state = Key13_state_0; //转换到按键初始状态
      }
			else 
			{
				Key13_time++;
				if (Key13_time>100)   //按键持续按下，时间超过2000ms，判断为长按
				{
					Key13_time=0;
					Key13_Value=Key13_long;			//返回值为1
					Key13_state=Key13_state_3;
				}
			}
      break;
				
			case Key13_state_3:
				if (Key13_Now!=0x10)
				{
					Key13_state=Key13_state_0;   //按键已释放，转换到按键初始态
				}
				break;
			default:
				break;
	}
	return Key13_Value;	                    //返回按键值
}

/*****************************************************************************
函数名称：按键3获取返回值程序
功    能：检测按键，并返回按键值
返 回 值：key3_press	
备    注：
*****************************************************************************/
	
unsigned char Key3_Read(void)
{
	static  char Key3_state=0;//按键1初始状态为0
	static  char Key3_time=0; //判断按键的按键时间
	unsigned char Key3_return=0; //按键1返回值
	
	switch(Key3_state)
	{
		case Key3_state_0:                 // 按键初始态
			if (!Key3)
			{
				Key3_state = Key3_state_1;// 键被按下，状态转换到键确认态
			}
		break;	
			
    case Key3_state_1:                   // 按键确认态
			if (!Key3)
			{		
				Key3_time=0;
				//Key1_return = 1;         // 按键仍按下，按键确认输出为“1”
         Key3_state = Key3_state_2;// 状态转换到键释放态
      }
			else
				{
					Key3_state = Key3_state_0; // 按键已抬起，转换到按键初始态
			  }
      break;	
				
		case Key3_state_2:
			if(Key3) 
			{
				Key3_return=Key3_click;    //判断按键为短按，返回值为0
			  Key3_state = Key3_state_0; //转换到按键初始状态
      }
			else 
			{
				Key3_time++;
				if (Key3_time>100)   //按键持续按下，时间超过1000ms，判断为长按
				{
					Key3_time=0;
					Key3_return=Key1_long;			//返回值为1
					Key3_state=Key1_state_3;
				}
			}
      break;
				
				
			case Key3_state_3:
				if (Key3)
				{
					Key3_state=Key3_state_0;   //按键已释放，转换到按键初始态
				}
				break;
			default:
				break;
	}
	return Key3_return;	                    //返回按键值
}

/*****************************************************************************
函数名称：组合按键获取返回值程序
功    能：检测组合长按按键，并返回按键值
返 回 值：	
备    注：
*****************************************************************************/

unsigned char Key_Group_Read(void)                //组合按键，先按1,再长按3
{
	static  char KeyG_state=0;
	static  char KeyG_time=0;
	unsigned char KeyG_return=0;
	
	switch(KeyG_state)
	{
		case KeyG_state_0:                         //检测第二个按键
			if (!Key1)                             //按键1按下
			{

					KeyG_state=KeyG_state_1;
				
			}
			break;
		case KeyG_state_1:                        
			if (!Key1)             //判断第一个按键和第三个按键
			{
					KeyG_time=0;
					KeyG_state=KeyG_state_2;
			}
			else
			{
				KeyG_state=KeyG_state_0;
			}
			break;
		case KeyG_state_2:                        //检测第一个第二个按键是否长按
			if (!Key1)                              //检测第一个按键
			{
				KeyG_time++;
				if (KeyG_time>100)
				{
					KeyG_state=KeyG_state_3;
					KeyG_return=KeyG_group_long;        //输出长按标志位
					OLED_Power_On();         //开机显示
					KeyG_time=0;
				}
			}
			break;
		case KeyG_state_3:
			if ((Key1))                   
			{
				KeyG_state = KeyG_state_0;     //进入原始状态
				PowerClose = 1;
			}
			else
			{
				if(!Key3)
				{
					KeyG_state = KeyG_state_4;     //进入第三个状态
					KeyG_time=0;
				}
			}		
		break;	
		case KeyG_state_4:                //Key3长按
			if(!Key3)
				{
					KeyG_time++;
					if (KeyG_time>100)
					{
						KeyG_state=KeyG_state_5;	
						OLED_ShowCHinese(0,2,0);//密
						OLED_ShowCHinese(18,2,1);//码
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
				KeyG_state = KeyG_state_0;     //进入原始个状态
			}
		}
		break;
	}
}

/*****************************************************************************
函数名称：组合按键2和3获取返回值程序
功    能：检测组合按键2和3，并返回按键值
返 回 值：	
备    注：
*****************************************************************************/
unsigned char Key_Group23_Read(void)      
{
	static  char Key23_state=0;
	static  char Key23_time=0;
	unsigned char Key23_return=0;
	
	switch(Key23_state)
	{
		case  Key23_state_0:                     //按键2和3初始状态
			if (!Key2||Key3)                       //按键2和3第一次按下
			{
				Key23_state=Key23_state_1;
			}
		break;
			
		case Key23_state_1:                      //按键2和3确认被按下
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
			
		case Key23_state_2:                    //按键2和3确认是否长按
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
函数名称：ScanKeyCode()
功    能：扫描按键
返 回 值：	
备    注：
*****************************************************************************/
void ScanKeyCode()
{
	unsigned char Keycode = Key1_Read();
//	OLED_ShowNum()
	
	/*在每次按键扫描的时候，把所有标志位均复位，防止引起按键提前按下，引起后端动作*/
	
//	if(Key1_clickPress)  Key1_clickPress = 0;   
//	if(Key2_clickPress)  Key2_clickPress = 0;
//	if(Key3_clickPress)  Key3_clickPress = 0;
//	if(Key1_longPress)   Key1_longPress = 0;
//	if(Key2_longPress)   Key2_longPress = 0;
//	if(Key3_longPress)   Key3_longPress = 0;
	
	
	if(Keycode ==Key_None) return;
	
	if(Keycode&Key_Short)   //短按
	{
		if(Keycode&Key1_Press)  //按键1按下
		{
//			OLED_ShowNum(0,2,0,1,16);
//			Led1 = !Led1;
				Key1_clickPress = 1;//按键1短按
				
		}
		else if(Keycode&Key2_Press)   //按键2短按
		{
//			OLED_ShowNum(0,2,1,1,16);
				Key2_clickPress = 1;  //按键2短按

		}
		else if(Keycode&Key3_Press)   //按键3短按
		{
//			OLED_ShowNum(0,2,2,1,16);
				Key3_clickPress = 1;  //按键3长按
		}
	}
	
	else if(Keycode&Key_Long)   //长按
	{
		Key_long = 1;   //有长按键
		if(Keycode&Key1_Press)  //按键1长按
		{
			Key1_longPress = 1;  //按键1长按
		}

		 if (Keycode&Key3_Press)
		{
			Key3_longPress = 1;  //按键3长按
		}
	}
	
}

增加一行代码
增加二行代码
增加三行代码
