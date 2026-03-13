/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020-2022 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DATE        :                                                      */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
#include "r_smc_entry.h"
#include "MODBUS_SLAVE.h"
#include "24C02.h"
//MODBUS影子寄存器///////////////////
unsigned int  Working;					//工作标志(不记忆)
unsigned int  Enables;					//使能标志
unsigned char Baudrate;					//波特率
unsigned char Comcon;					//通信权配置
unsigned char Stacon;					//启动权配置
unsigned int  CHxMODE[5]={0,0,0,0,0};	//波形模式：0,直流 1,正弦 2,方波 3,三角
unsigned int  CHxFREQ[5]={0,0,0,0,0};	//频率
  signed int  CHxPPAK[5]={0,0,0,0,0};	//正峰值
  signed int  CHxNPAK[5]={0,0,0,0,0};	//负峰值
unsigned int  CHxPAR1[5]={0,0,0,0,0};	//直流:无效 正弦:无效 方波:正脉宽 三角:上升
unsigned int  CHxPAR2[5]={0,0,0,0,0};	//直流:无效 正弦:无效 方波:负脉宽 三角:下降
unsigned int  CHxGRO[5] ={0,0,0,0,0};
unsigned int  CHxROW[5] ={0,0,0,0,0};

//5个通道的波形文件///////
  signed int  WaveCache[WAVEPOINT];		//1个通道的波形缓存 (波形发生用)
unsigned int  WaveTable[5][WAVEPOINT];	//每个通道的波形数据
unsigned int  pointer[5];				//每个通道波形的初始指针
unsigned int  DutyCycle[5];				//每个通道波形的占空比	频率定时器自动调用波形数据存放于该变量 PWM中断后通过该变量修改下一PWM的占空比

//开关量///////////
unsigned char Switch[5];				//5路开关量 0x00:按下 0xFF:松开 
unsigned char Start;					//工作启停使能
unsigned int  SysTime;					//继电器节拍

//通道保护
unsigned int  Guard[5];					//通道保护标志
//unsigned int  Guarding;					//
////实际电流(ADC的值
unsigned int Current[8]={99,99,99,99,99};

const signed int  WaveSIN[500]=			//正弦波原始波形 ±1600
{
0,20,40,60,80,100,121,141,161,181,201,220,240,260,280,300,
320,339,359,378,398,417,437,456,475,494,514,533,551,570,589,608,
626,645,663,681,699,717,735,753,771,788,806,823,840,857,874,891,
908,924,940,957,973,989,1004,1020,1035,1051,1066,1081,1095,1110,1124,1138,
1152,1166,1180,1194,1207,1220,1233,1246,1258,1270,1283,1294,1306,1318,1329,1340,
1351,1362,1372,1382,1392,1402,1412,1421,1430,1439,1448,1456,1464,1472,1480,1488,
1495,1502,1509,1515,1522,1528,1534,1539,1545,1550,1555,1559,1564,1568,1572,1575,
1579,1582,1585,1587,1590,1592,1594,1595,1597,1598,1599,1599,1600,1600,1600,1599,
1599,1598,1597,1595,1594,1592,1590,1587,1585,1582,1579,1575,1572,1568,1564,1559,
1555,1550,1545,1539,1534,1528,1522,1515,1509,1502,1495,1488,1480,1472,1464,1456,
1448,1439,1430,1421,1412,1402,1392,1382,1372,1362,1351,1340,1329,1318,1306,1294,
1283,1270,1258,1246,1233,1220,1207,1194,1180,1166,1152,1138,1124,1110,1095,1081,
1066,1051,1035,1020,1004,989,973,957,940,924,908,891,874,857,840,823,
806,788,771,753,735,717,699,681,663,645,626,608,589,570,551,533,
514,494,475,456,437,417,398,378,359,339,320,300,280,260,240,220,
201,181,161,141,121,100,80,60,40,20,0,-20,-40,-60,-80,-100,
-121,-141,-161,-181,-201,-220,-240,-260,-280,-300,-320,-339,-359,-378,-398,-417,
-437,-456,-475,-494,-514,-533,-551,-570,-589,-608,-626,-645,-663,-681,-699,-717,
-735,-753,-771,-788,-806,-823,-840,-857,-874,-891,-908,-924,-940,-957,-973,-989,
-1004,-1020,-1035,-1051,-1066,-1081,-1095,-1110,-1124,-1138,-1152,-1166,-1180,-1194,-1207,-1220,
-1233,-1246,-1258,-1270,-1283,-1294,-1306,-1318,-1329,-1340,-1351,-1362,-1372,-1382,-1392,-1402,
-1412,-1421,-1430,-1439,-1448,-1456,-1464,-1472,-1480,-1488,-1495,-1502,-1509,-1515,-1522,-1528,
-1534,-1539,-1545,-1550,-1555,-1559,-1564,-1568,-1572,-1575,-1579,-1582,-1585,-1587,-1590,-1592,
-1594,-1595,-1597,-1598,-1599,-1599,-1600,-1600,-1600,-1599,-1599,-1598,-1597,-1595,-1594,-1592,
-1590,-1587,-1585,-1582,-1579,-1575,-1572,-1568,-1564,-1559,-1555,-1550,-1545,-1539,-1534,-1528,
-1522,-1515,-1509,-1502,-1495,-1488,-1480,-1472,-1464,-1456,-1448,-1439,-1430,-1421,-1412,-1402,
-1392,-1382,-1372,-1362,-1351,-1340,-1329,-1318,-1306,-1294,-1283,-1270,-1258,-1246,-1233,-1220,
-1207,-1194,-1180,-1166,-1152,-1138,-1124,-1110,-1095,-1081,-1066,-1051,-1035,-1020,-1004,-989,
-973,-957,-940,-924,-908,-891,-874,-857,-840,-823,-806,-788,-771,-753,-735,-717,
-699,-681,-663,-645,-626,-608,-589,-570,-551,-533,-514,-494,-475,-456,-437,-417,
-398,-378,-359,-339,-320,-300,-280,-260,-240,-220,-201,-181,-161,-141,-121,-100,
-80,-60,-40,-20
};

void main(void);
void Timer1Resetting(void);				//根据频率重置CHx通道定时器
void Timer2Resetting(void);
void Timer3Resetting(void);
void Timer4Resetting(void);
void Timer5Resetting(void);
void DIERMode(unsigned char channel);
void SINEMode(unsigned char channel);
void SQUAMode(unsigned char channel);
void TRIAMode(unsigned char channel);
void ConfigWaveData(unsigned char channel);
void AutoConfigReset(void);
void LoadRegister(void);
void StartControl(void);
void ChannelEnable(unsigned char save);
void TeamUp(void);
static void PortControl(void);
static unsigned int ReVolt(unsigned char channel);//合成显示电压
void Display(void);
	
//////////////////////////////////////////////////////
void main(void)
{
	R_UARTA_Set_PowerOn();
	R_Config_UARTA0_Start();
	R_Config_UARTA0_Receive(&ReceiveBuffer0,1);
	R_Config_UARTA1_Start();
	R_Config_UARTA1_Receive(&ReceiveBuffer1,1);
	
	R_ITL_Set_PowerOn();
	R_ITL_Start_Interrupt();
	
	R_Config_ADC_Set_OperationOn();
	R_Config_ADC_Start();	
	
	R_TAU0_Set_PowerOn();
	R_Config_TAU0_1_Start();			//通用定时器
	R_Config_TAU0_2_Start();			//PWM启动
	EI();                       		// 

	QueueReset();						//初始化MODBUS
	R_Config_WDT_Restart();				//喂狗
	LoadRegister();						//上电加载寄存器参数到变量
	R_Config_WDT_Restart();				//喂狗
	ChannelEnable(0);					//上电后，通过影子寄存器与真实寄存器的差，触发通道打开
	while(1)
	{
		R_Config_WDT_Restart();			//喂狗
		MODBUS_COMMUN();				//MODBUS服务
		AutoConfigReset();				//若参数变化则通过参数修改波形文件
		StartControl();					//根据寄存器参数控制启动
		ChannelEnable(1);				//通道开启或关闭服务
		Display();						//刷新显示
		
		//CH5CURR=Current[1];			//CH5
		//CH4CURR=Current[2];			//CH4
		//CH3CURR=Current[3];			//CH3
		//CH2CURR=Current[4];			//CH2
		//CH1CURR=Current[5];			//CH1
		//CH1VOLT=Current[0];			//温度
		//CH2VOLT=Current[6];			//保留
		//CH3VOLT=Current[7];			//保留
	}
}
//刷新显示//////////////////////
void Display(void)
{
	if((ENABLE&0x01)&&(Start&0x01))	//CH1通道已经打开
	{
		CH1VOLT=ReVolt(0);CH1CURR=Current[5];
	}else{CH1VOLT=0;CH1CURR=0;}
	if((ENABLE&0x02)&&(Start&0x02))	//CH2通道已经打开
	{
		CH2VOLT=ReVolt(1);CH2CURR=Current[4];
	}else{CH2VOLT=0;CH2CURR=0;}
	if((ENABLE&0x04)&&(Start&0x04))	//CH3通道已经打开
	{
		CH3VOLT=ReVolt(2);CH3CURR=Current[3];
	}else{CH3VOLT=0;CH3CURR=0;}	
	if((ENABLE&0x08)&&(Start&0x08))	//CH4通道已经打开
	{
		CH4VOLT=ReVolt(3);CH4CURR=Current[2];
	}else{CH4VOLT=0;CH4CURR=0;}
	if((ENABLE&0x10)&&(Start&0x10))	//CH5通道已经打开
	{
		CH5VOLT=ReVolt(4);CH5CURR=Current[1];
	}else{CH5VOLT=0;CH5CURR=0;}	
	
	if(Guard[0]){WARNING|=0x0001;}else{WARNING&=0xFFFE;}	//标记异常
	if(Guard[1]){WARNING|=0x0002;}else{WARNING&=0xFFFD;}
	if(Guard[2]){WARNING|=0x0004;}else{WARNING&=0xFFFB;}
	if(Guard[3]){WARNING|=0x0008;}else{WARNING&=0xFFF7;}
	if(Guard[4]){WARNING|=0x0010;}else{WARNING&=0xFFEF;}
}
static unsigned int ReVolt(unsigned char channel)			//(完成)合成显示电压
{
	unsigned int Tim=0;
	switch(CHxMODE[channel])
	{
		case DIERCCUU:	//直流模式
		return(INVOLT)*(abs(CHxPPAK[channel]))/MAXVOLT;
		
		case SQUAWAVE:	//矩形波模式	CHxPAR1[channel]//正脉宽(低8位)+负脉宽(高8位)
		return(INVOLT*(abs(CHxPPAK[channel])*(CHxPAR1[channel])+abs(CHxNPAK[channel])*(CHxPAR2[channel]))/MAXDUTY/MAXVOLT);

		case SINEWAVE:	//正弦波模式
		if(((CHxPPAK[channel]>=0)&&(CHxNPAK[channel]>=0))||((CHxPPAK[channel]<=0)&&(CHxNPAK[channel]<=0)))
		{
			return(INVOLT)*(abs(CHxPPAK[channel])+abs(CHxNPAK[channel]))/MAXVOLT/2;			//中心线算法 波形位于1或4相限
		}else
		{
			return(INVOLT)*(abs(CHxPPAK[channel])+abs(CHxNPAK[channel]))*141/MAXVOLT/400U;	//均方根算法 波形横跨1和4相限
		}
		case TRIAWAVE:	//三角波模式 //上升(CHxPAR1[channel]+下降CHxPAR1[channel]
		if(((CHxPPAK[channel]>=0)&&(CHxNPAK[channel]>=0))||((CHxPPAK[channel]<=0)&&(CHxNPAK[channel]<=0)))
		{
			return(INVOLT)*(abs(CHxPPAK[channel])+abs(CHxNPAK[channel]))/MAXVOLT/2;			//中心线算法 波形位于1或4相限
		}else
		{
			Tim=(200UL-(CHxPAR2[channel])-(CHxPAR1[channel])); 	//将占空比演变成为均方根 A=(2-B)/2 
			return(INVOLT)*(abs(CHxPPAK[channel])+abs(CHxNPAK[channel]))*Tim/MAXVOLT/400U;	//类均方根算法 波形横跨1和4相限
		}
	}
	return 0;
}
//(完成)使能控制/继电器管理////////////
void ChannelEnable(unsigned char save)
{
	//ENABLE,Enables,Start,SysTime
	//TimerxResetting();TeamUp();
	/*****************************************************
	B0:	x / 0 / 1 / 1 / 0	//旧值 Enables 	B0-4
	B0:	x / 0 / 1 / 0 / 1	//新值 ENABLE  	B0-4
	B0:	0 / 1 / 1 / 1 / 1	//启停 Start		B0-4		
	 	|	|   |	|	|	
	 	|	|	|	|	|--数据从0变1 	通道需要打开 先开继电器再开定时器和PWM
		|	|	|	|------数据从1变0 	通道需要关闭 先关定时器和PWM再关继电器
		|	|	|----------数据1没有变化 	通道处于打开状态
		|	|--------------数据0没有变化 	通道处于关闭状态
		|------------------只关闭定时器和PWM
	******************************************************/
	static unsigned char EnFlag=0;		//标志使能变化后只做完第一步,B7为保存标志
	static unsigned char _start=0xFF;	//工作启停控制位的旧值(Start影子寄存器)
	//使能标志位发生变化//////////////////////////////////////////////
	if(ENABLE&0xFFE0)				//空余的标志位无效
	{
		ENABLE &= 0x001F;			//清除无效位
	}
	if(Enables != ENABLE)
	{
		if((ENABLE&0x01)!=(Enables&0x01))	//CH1状态有差异
		{
			if((EnFlag&0x01)==0)	//还没有设置任务(先做第一步)
			{
				if(Enables&0x01)				//如果旧值1(新值0) 需要关闭通道
				{
					Timer1Resetting();
					POLCH1=0;					//无论是否运行先关闭输出使能
				}
				else							//否则旧值0(新值1) 需要打开通道
				{
					ERLCH1=1;					//先打开继电器	
				}
				EnFlag |= 0x81;					//建立任务标志(第一步已做完)
				SysTime = 500;					//设置第一步与第二步工作间的延时
			}				
			else if(SysTime==0)		//有未完成的任务(任务2)
			{
				if(Enables&0x01)				//如果旧值1(新值0) 需要关闭通道
				{
					ERLCH1=0;					//再关闭通道
					Enables&=0xFFFE;			//完成任务，清除CH1通道差异(新值0)
				}
				else							//否则旧值0(新值1) 需要打开通道
				{
					Timer1Resetting();			//再打开定时器开始输出
					Enables|=0x01;				//完成任务，清除CH1通道差异
				}
				EnFlag&=0xFE;					//第二步做完,清除任务标志(新值1)	
			}
		}
		if((ENABLE&0x02)!=(Enables&0x02))	//CH2状态有差异
		{
			if((EnFlag&0x02)==0)
			{
				if(Enables&0x02)
				{
					Timer2Resetting();
					POLCH2=0;
				}
				else
				{
					ERLCH2=1;
				}
				EnFlag |= 0x82;
				SysTime = 500;
			}
			else if(SysTime==0)		//有未完成的任务
			{
				if(Enables&0x02)
				{
					ERLCH2=0;
					Enables&=0xFFFD;
				}
				else
				{
					Timer2Resetting();
					Enables|=0x02;
				}
				EnFlag&=0xFD;
			}
		}
		if((ENABLE&0x04)!=(Enables&0x04))	//CH3状态有差异
		{
			if((EnFlag&0x04)==0)	//还没有设置任务(先做第一步)
			{
				if(Enables&0x04)
				{
					Timer3Resetting();
					POLCH3=0;
				}
				else
				{
					ERLCH3=1;
				}
				EnFlag |= 0x84;
				SysTime = 500;
			}
			else if(SysTime==0)		//有未完成的任务
			{
				if(Enables&0x04)
				{
					ERLCH3=0;
					Enables&=0xFFFB;
				}
				else
				{
					Timer3Resetting();
					Enables|=0x04;
				}
				EnFlag&=0xFB;				
			}
		}		
		if((ENABLE&0x08)!=(Enables&0x08))	//CH4状态有差异
		{
			if((EnFlag&0x08)==0)	//还没有设置任务(先做第一步)
			{
				if(Enables&0x08)
				{
					Timer4Resetting();
					POLCH4=0;
				}
				else
				{
					ERLCH4=1;
				}
				EnFlag |= 0x88;
				SysTime = 500;
			}
			else if(SysTime==0)		//有未完成的任务
			{
				if(Enables&0x08)
				{
					ERLCH4=0;
					Enables&=0xFFF7;
				}
				else
				{
					Timer4Resetting();
					Enables|=0x08;
				}
				EnFlag&=0xF7;
			}		
		}		
		if((ENABLE&0x10)!=(Enables&0x10))	//CH5状态有差异
		{
			if((EnFlag&0x10)==0)			//还没有设置任务(先做第一步)
			{
				if(Enables&0x10)
				{
					Timer5Resetting();
					POLCH5=0;
				}
				else
				{
					ERLCH5=1;
				}
				EnFlag |= 0x90;
				SysTime = 500;
			}
			else if(SysTime==0)		//有未完成的任务
			{
				if(Enables&0x10)
				{
					ERLCH5=0;
					Enables&=0xFFEF;
				}
				else
				{
					Timer5Resetting();
					Enables|=0x10;
				}
				EnFlag&=0xEF;
			}		
		}
		if((save)&&(EnFlag==0x80))	//保存参数到IIC
		{
			i2c_writedoublebyte((ENABLE),ADD1,SENABLE);
			EnFlag=0;
		}
	}
	else if(_start != Start)	//启停控制位变化重新组队
	{
		TeamUp();
		_start = Start;
	}
	else						//工作模式没变化 新值旧值相同 刷新一遍继电器和输出
	{
		//-使能打开--------------------启动中且无过载--------------------------停止中-----------使能关闭----------------- 						
		//---------------继电器开----------------------------------输出开----------输出关-----------继电器关---输出关----
		if(Enables&0x01){ERLCH1=1;if((Start&0x01)&&(Guard[0]==0)){POLCH1=1;}else{POLCH1=0;}}else{ERLCH1=0;POLCH1=0;}	
		if(Enables&0x02){ERLCH2=1;if((Start&0x02)&&(Guard[1]==0)){POLCH2=1;}else{POLCH2=0;}}else{ERLCH2=0;POLCH2=0;}
		if(Enables&0x04){ERLCH3=1;if((Start&0x04)&&(Guard[2]==0)){POLCH3=1;}else{POLCH3=0;}}else{ERLCH3=0;POLCH3=0;}
		if(Enables&0x08){ERLCH4=1;if((Start&0x08)&&(Guard[3]==0)){POLCH4=1;}else{POLCH4=0;}}else{ERLCH4=0;POLCH4=0;}
		if(Enables&0x10){ERLCH5=1;if((Start&0x10)&&(Guard[4]==0)){POLCH5=1;}else{POLCH5=0;}}else{ERLCH5=0;POLCH5=0;}
		EnFlag = 0x00;			//清除所有未完成的任务 
	}
}
//(完成)启动控制////////////
void StartControl(void)
{
	//CONFIG CONTROL Switch[x] Start			//控制位
	//void PortControl()
	static unsigned char Compete = 0;			//自由竞争控制权标志
	switch(STACON)	//0,自由 1,开关 2,远程 3,本地
	{
		case 0:		//自由抢占控制权模式
			if(Compete)			//工作中，只允许释放控制权，不可竞争
			{
				//释放控制权
				if(CONTROL==0){Compete &= 0xFE;}			//modbus通信启动结束
				if((SCRFLG)==0){Compete &= 0x0FD;}			//屏幕启动结束
				if((0xFF&Switch[0]&Switch[1]&Switch[2]&Switch[3]&Switch[4])==0xFF){Compete &= 0xFB;}
				//根据控制权选择控制信号
				if(Compete&0x01){Start = CONTROL;}			//启动使用modbus通信接口
				else if(Compete&0x02){Start = SCRFLG;}		//启动使用触摸屏
				else if(Compete&0x04){PortControl();}		//启动使用IO口
				else{Compete = 0x00;Start = 0;IOFLG = 0;}	//没有任何有效控制者 清除启动使能
			}
			else				//空闲中，允许竞争控制权
			{
				if(CONTROL){Compete |= 0x01;}				//modbus通信启动
				else if(SCRFLG){Compete |= 0x02;}			//屏幕启动
				else if((0xFF&Switch[0]&Switch[1]&Switch[2]&Switch[3]&Switch[4])==0x00){Compete |= 0x04;}	//IO口启动
				else{Compete = 0x00;Start = 0;}				//没有控制者
			}
		break;
		
		case 1:
			PortControl();		//由开关量控制启停，5个开关量对应5个通道
		break;
		
		case 2:
			Start = CONTROL;	//modbus通信控制通道启停
			IOFLG = 0;
		break;
		
		case 3:
			Start = SCRFLG;		//本地屏幕启动/停止按钮控制5个通道的启停
			IOFLG = 0;
		break;
		
		default:
			Start = 0;
		break;
	}
}
static void PortControl(void)
{
	if(Switch[0]==0xFF){Start&=0xFE;IOFLG&=0xFE;}			//外控释放 清除启动标志
	else if(Switch[0]==0x00){Start|=0x01;IOFLG|=0x01;}		//外控闭合 置位启动标志
	if(Switch[1]==0xFF){Start&=0xFD;IOFLG&=0xFD;}
	else if(Switch[1]==0x00){Start|=0x02;IOFLG|=0x02;}
	if(Switch[2]==0xFF){Start&=0xFB;IOFLG&=0xFB;}
	else if(Switch[2]==0x00){Start|=0x04;IOFLG|=0x04;}
	if(Switch[3]==0xFF){Start&=0xF7;IOFLG&=0xF7;}
	else if(Switch[3]==0x00){Start|=0x08;IOFLG|=0x08;}
	if(Switch[4]==0xFF){Start&=0xEF;IOFLG&=0xEF;}
	else if(Switch[4]==0x00){Start|=0x10;IOFLG|=0x10;}
}
//(完成)加载寄存器的数据到影子寄存器
void LoadRegister(void)
{
//Enables;					//使能标志
//Comcon;					//通信权配置
//Stacon;					//启动权配置
//CHxMODE[5]={0,0,0,0,0};	//波形模式：0,直流 1,正弦 2,方波 3,三角
//CHxFREQ[5]={0,0,0,0,0};	//频率
//CHxPPAK[5]={0,0,0,0,0};	//正峰值
//CHxNPAK[5]={0,0,0,0,0};	//负峰值
//CHxPAR1[5]={0,0,0,0,0};	//直流:无效 正弦:无效 方波:正脉宽 三角:上升
//CHxPAR2[5]={0,0,0,0,0};	//直流:无效 正弦:无效 方波:负脉宽 三角:下降
//CHxGRO[5] ={0,0,0,0,0};	//70.74.78.82
//CHxROW[5] ={0,0,0,0,0};	//72.76.80.84
	unsigned char i;
	
	ENABLE = i2c_readdoublebyte(ADD1,SENABLE);					//加载启用禁用使能
	if(ENABLE>31){ENABLE=0;}	//参数异常初始化
	Enables = 0x00;				//上电默认通道全关闭 需要通过MODBUS寄存器值和影子寄存器的差异触发通道继电器打开
	
	COMCON = i2c_readbyte(ADD1,SCOMCON);						//加载通信权
	if(COMCON>0x02){COMCON=0;}else{Comcon = COMCON;}
	
	STACON = i2c_readbyte(ADD1,SSTACON);						//加载启动权
	if(STACON>0x03){STACON=0;}else{Stacon = STACON;}
	
	BAUD = i2c_readbyte(ADD1,SBAUD);		//加载波特率
	if(BAUD>0x01){BAUD=0;}
	Baudrate = 0;				//上电默认9600  其他波特率通过寄存器差异重新加载波特率
	
	for(i=0;i<5;i++)			//加载其他地址连续的参数
	{
		Register[6*i+24] = i2c_readdoublebyte(ADD1,12*i+10);	//加载波形模式
		if(Register[6*i+24]>4){Register[6*i+24]=0;}				//默认0模式
		CHxMODE[i]= 0xFF;				//上电刷新波形(强制)
		
		Register[6*i+25] = i2c_readdoublebyte(ADD1,12*i+12);	//频率
		if(Register[6*i+25]==0){Register[6*i+25]=1;}	//默认1Hz
		if(Register[6*i+25]>99){Register[6*i+25]=99;}	//默认99Hz
		CHxFREQ[i]= Register[6*i+25];	//25/31/37/43/49
		
		Register[6*i+26] = i2c_readdoublebyte(ADD1,12*i+14);
		if((Register[6*i+26]<0xFE0C)&&(Register[6*i+26]>0x01F4))
		{if(Register[6*i+26]&0x1000){Register[6*i+26]=0xFE0C;}else{Register[6*i+26]=0x01F4;}}
		CHxPPAK[i]= Register[6*i+26];	//正峰值(带符号)
		Register[6*i+27] = i2c_readdoublebyte(ADD1,12*i+16);
		if((Register[6*i+27]<0xFE0C)&&(Register[6*i+27]>0x01F4))
		{if(Register[6*i+27]&0x1000){Register[6*i+27]=0xFE0C;}else{Register[6*i+27]=0x01F4;}}			
		CHxNPAK[i]= Register[6*i+27];	//负峰值(带符号)
		
		Register[6*i+28] = i2c_readdoublebyte(ADD1,12*i+18);
		Register[6*i+29] = i2c_readdoublebyte(ADD1,12*i+20);
		if((Register[6*i+28]+Register[6*i+29])>100)	//数据超限
		{
			Register[6*i+28]=45;
			Register[6*i+29]=45;
			Register[  i+17]=10;
		}
		else
		{Register[i+17]=100U-Register[6*i+28]-Register[6*i+29];}	//重算参数3
		CHxPAR1[i]= Register[2*i+28];	//方波:正脉宽 三角:上升
		CHxPAR2[i]= Register[2*i+29];	//方波:负脉宽 三角:下降
		
		Register[2*i+54] = i2c_readdoublebyte(ADD1,4*i+70);	//组
		if(Register[2*i+54]>9)Register[2*i+54]=5;			//限制到5
		CHxGRO[i]= Register[2*i+54];	//
		Register[2*i+55] = i2c_readdoublebyte(ADD1,4*i+72);	//队
		if(Register[2*i+55]>9)Register[2*i+55]=1+i;
		CHxGRO[i]= Register[2*i+55];	//
	}
}
//(完成)根据组队参数修改CHx通道的波形指针初始值
void TeamUp(void)
{
	//pointer[i];			
	//CHxGRO[5];CHxROW[5];
	unsigned char i;
	unsigned int temp;
	for(i=0;i<5;i++)
	{
		if((CHxGRO[i])&&(CHxROW[i]))	//参数有效 非零
		{
			temp = WAVEPOINT /(CHxROW[i]);	
			pointer[i] = temp*(CHxGRO[i]);
		}
		else
		{
			pointer[i]=0;		//参数无效,不需要组队
		}
	}
	Timer1Resetting();			//
	Timer2Resetting();
	Timer3Resetting();
	Timer4Resetting();
	Timer5Resetting();
}
//(完成)根据频率重置CHx通道定时器，用于修改对应通道输出波形的频率
void Timer1Resetting(void)		//定时器ITL000
{
	//CHxFREQ[0]
	unsigned long number; 
	unsigned char prescale;
	if(CHxFREQ[0])
	{
		prescale = 0;
		number = (FCLK/WAVEPOINT)/CHxFREQ[0];		//计数总计数 500点位
		while(number > 0xFF)						//从0轮询到7 0分频--7分频
		{
			number = number>>1;
			prescale++;								//如果超8位 预分频计数加1
		}
		R_Config_ITL000_Stop();
		ITLFDIV00 &= 0xF8;							//清除原先的设置
		ITLFDIV00 |= prescale;
		ITLCMP000 = number-1;
		R_Config_ITL000_Start();
	}
}
void Timer2Resetting(void)		//定时器ITL001
{
	//CHxFREQ[1]
	unsigned long number; 
	unsigned char prescale;
	if(CHxFREQ[1])
	{
		prescale = 0;
		number = (FCLK/WAVEPOINT)/CHxFREQ[1];		//计数总计数 500点位
		while(number > 0xFF)						//从0轮询到7 0分频--7分频
		{
			number = number>>1;
			prescale++;								//如果超8位 预分频计数加1
		}
		R_Config_ITL001_Stop();	
		ITLFDIV00 &= 0x8F;							//清除原先的设置
		ITLFDIV00 |= prescale<<4;
		ITLCMP001 = number-1;
		R_Config_ITL001_Start();
	}
}
void Timer3Resetting(void)		//定时器ITL012
{
	//CHxFREQ[2]
	unsigned long number; 
	unsigned char prescale;
	if(CHxFREQ[2])
	{
		prescale = 0;
		number = (FCLK/WAVEPOINT)/CHxFREQ[2];		//计数总计数 500点位
		while(number > 0xFF)						//从0轮询到7 0分频--7分频
		{
			number = number>>1;
			prescale++;								//如果超8位 预分频计数加1
		}
		R_Config_ITL012_Stop();	
		ITLFDIV01 &= 0xF8;							//清除原先的设置
		ITLFDIV01 |= prescale;
		ITLCMP012 = number-1;
		R_Config_ITL012_Start();
	}
}
void Timer4Resetting(void)		//定时器ITL013
{
	//CHxFREQ[3]
	unsigned long number; 
	unsigned char prescale;
	if(CHxFREQ[3])
	{
		prescale = 0;
		number = (FCLK/WAVEPOINT)/CHxFREQ[3];		//计数总计数 500点位
		while(number > 0xFF)						//从0轮询到7 0分频--7分频
		{
			number = number>>1;
			prescale++;								//如果超8位 预分频计数加1
		}
		R_Config_ITL013_Stop();	
		ITLFDIV01 &= 0x8F;							//清除原先的设置
		ITLFDIV01 |= prescale<<4;
		ITLCMP013 = number-1;
		R_Config_ITL013_Start();
	}
}
void Timer5Resetting(void)		//定时器
{
	//CHxFREQ[4]
	unsigned long number; 
	if(CHxFREQ[4])
	{
		number = (FIHP/WAVEPOINT)/CHxFREQ[4];		//计数总计数 500点位
		R_Config_TAU0_0_Stop();
		TDR00 = number-1;
		R_Config_TAU0_0_Start();
	}
}

/*********************************************
1，加载模型波形到波形数组
	正弦波使用数组模型
	三角波使用上升/平顶/下降构建模型
	方波使用正脉宽/负脉宽构建模型
	直流直接填充
2，通过公式修正每个波形点的幅度
	a，中心值 =(峰值-谷值)/2
	b，偏移值 = 峰值-中心值
	c，输出值 = 中心值*模型数值/最大占空比+偏移值
*********************************************/
//(完成)直流模式数据填充
void DIERMode(unsigned char channel)
{	
	//CHxPPAK[channel]
	unsigned int x;
	for(x=0;x<WAVEPOINT;x++)
	{WaveTable[channel][x]=(MAXPWM>>1)+(CHxPPAK[channel]*7/5);}	//(800-700)~(800+700)
}
//(完成)正弦波模式数据填充
void SINEMode(unsigned char channel)
{	
	//CHxFREQ[channel]
	//CHxPPAK[channel]
	//CHxNPAK[channel]
	//WaveSIN[500]
	unsigned int x;
	  signed int center;		//中心值 =(峰值-谷值)/2
	  signed int offset;		//偏移值 = 峰值-中心值
	  signed long temp;
	  
	center=(CHxPPAK[channel]*7/5)-(CHxNPAK[channel]*7/5);
	offset=(CHxPPAK[channel]*14/5)-center;
	for(x=0;x<WAVEPOINT;x++)	//通过模型数据重构
	{	
		temp =offset+(signed long)center*WaveSIN[x]/MAXPWM;
		WaveCache[x] = temp;
	}

	for(x=0;x<WAVEPOINT;x++)	//从临时组搬运到波形数组
	{
		WaveTable[channel][x]=(MAXPWM>>1)+(WaveCache[x]/2); 
	}
}
//(完成)矩形波模式数据填充
void SQUAMode(unsigned char channel)
{	
	//常量MAXPWM = 1600/WAVEPOINT=500
	//CHxPPAK[channel]		//全局正幅度 范围-300 到 +300
	//CHxNPAK[channel]		//全局负幅度 范围-300 到 +300
	//CHxPAR1[channel]		//全局正脉宽(低8位) CHxPAR1[channel]+CHxPAR2[channel]<100
	//CHxPAR2[channel]		//全局负脉宽(低8位)
	unsigned int  x=0,y;		//从0地址开始
	  signed int  center;	//中心值 =(峰值-谷值)/2
	  signed int  offset;	//偏移值 = 峰值-中心值	 	  
//----------------------------------------------------------------------
	//前部
	y=8;
	while(x<y)WaveCache[x++]=(x)*200; 					//0->1400
	
	y+=(CHxPAR1[channel]*468)/100;
	while(x<y)WaveCache[x++]=1600;						//1600
	
	y+=8;
	while(x<y)WaveCache[x++]=1600-(x-(y-8))*200;		//1600->200
	
	y+=((100-CHxPAR1[channel]-CHxPAR2[channel])*234)/100;
	while(x<y)WaveCache[x++]=0;							//填充0
	
	//后部
	y+=8;
	while(x<y)WaveCache[x++]=(int)(x-(y-8))*-200;		//0->-1400
	
	y+=(CHxPAR2[channel]*468)/100;
	while(x<y)WaveCache[x++]=-1600;						//-1600
	
	y+=8;
	while(x<y)WaveCache[x++]=-1600+(int)(x-(y-8))*200;	//-1600->-200
	
	while(x<WAVEPOINT)WaveCache[x++]=0;					//填充0
//-------------------------------------------------------------------------
	center=(CHxPPAK[channel]*7/5)-(CHxNPAK[channel]*7/5);
	offset=(CHxPPAK[channel]*7/5)+(CHxNPAK[channel]*7/5);
	for(x=0;x<WAVEPOINT;x++)						
	{	
		WaveCache[x] = (signed long)center*(WaveCache[x])/MAXPWM +offset;//通过模型数据重构
		WaveTable[channel][x]=(MAXPWM>>1)+(WaveCache[x]/2);//数据移到指定通道中
	}
}
//(完成)三角波模式数据填充
void TRIAMode(unsigned char channel)
{	
	//CHxFREQ[channel]
	//CHxPPAK[channel]
	//CHxNPAK[channel]
	//CHxPAR1[channel]		//上升(低8位)
	//CHxPAR2[channel]		//下降(低8位)
	//平顶数=500*平顶值/100/2	//1/2的平顶数量
	//上升斜率=32000/上升/5	//上升斜率
	//上升段=(-16000)+上升斜率
	//下降段=16000-下降斜率
	unsigned int  x=0,y;
	  signed int  center;	//中心值 =(峰值-谷值)/2
	  signed int  offset;	//偏移值 = 峰值-中心值	
	
	unsigned int  pallet;	//平顶数
	float   	  ascent;	//上升斜率(单个数据步进量)
	float 		  declin;	//下降斜率(单个数据步进量)
//-------------------------------------------------------------------------
	pallet = (100-(CHxPAR1[channel])-(CHxPAR2[channel]))*5/2;		//平顶 500 2个平顶
	ascent = (3200.0f/5)/(CHxPAR1[channel]);						//上升斜率 -1600->1600 = 3200
	declin = (3200.0f/5)/(CHxPAR2[channel]);						//下降斜率
	
	y = pallet+1; 						//低部平台
	while(x<y) WaveCache[x++] = -1600L;
	y = pallet+CHxPAR1[channel]*5; 		//上升段 
	while(x<y) 
	{
   	 	WaveCache[x] = WaveCache[x-1]+ascent;
    	if(WaveCache[x]>1600L) WaveCache[x]=1600L;
    	x++;
	}
	y = pallet*2+CHxPAR1[channel]*5; 	//顶部平台
	while(x<y) WaveCache[x++] = 1600L;	
	while(x<WAVEPOINT) 					//下降段
	{
    	WaveCache[x] = WaveCache[x-1]-declin;
    	if(WaveCache[x]<-1600L) WaveCache[x]=-1600L;
    	x++;
	}		
//---------------------------------------------------------------------------
	center=(CHxPPAK[channel]*7/5)-(CHxNPAK[channel]*7/5);		//将PPAK/NPAK扩大到500
	offset=(CHxPPAK[channel]*7/5)+(CHxNPAK[channel]*7/5);
	for(x=0;x<WAVEPOINT;x++)						
	{	
		WaveCache[x] = (signed long)center*(WaveCache[x])/MAXPWM +offset;//通过模型数据重构
		WaveTable[channel][x]=(MAXPWM>>1)+(WaveCache[x]/2);//数据移到指定通道中
	}
}
//(完成)根据参数加载波形到相应通道的数据缓存
void ConfigWaveData(unsigned char channel)
{
	switch(CHxMODE[channel])
	{
		case DIERCCUU:	//直流模式
		DIERMode(channel);
		break;
		case SINEWAVE:	//正弦波模式
		SINEMode(channel);		
		break;
		case SQUAWAVE:	//矩形波模式
		SQUAMode(channel);		
		break;
		case TRIAWAVE:	//三角波模式
		TRIAMode(channel);		
		break;			
	}	
}
//(完成)参数变化后，自动重置CHx通道的波形数据，并且自动保存参数
void AutoConfigReset(void)
{
	/********************************************************************
	该函数通过寄存器对应的影子寄存器匹配数据是否变化，若数据变化则存储新数据到24C02，
	同时根据寄存器变化做出相应动作：
	1，波形模式寄存器发生变化，通过《ConfigWaveData(x)》重构当前通道的波形数组。
	2，频率寄存器发生变化，通过《Timer1Resetting()》重新配置定时器速度控制输出频率。
	3，正峰值/负峰值/上升/下降/平顶/正脉宽/负脉宽的数据发生变化，
	通过<CHxMODE[1]=0xFF>改变模式的方式强迫重构波形数组。
	4，组号/成员号发生变化，通过《TeamUp()》重置所有通道的波形数组指针
	********************************************************************/
	//配置数据发生变化()//////////////////////////////////////////////	
	if(Comcon != (COMCON&0xFF))	//保存参数
	{
		Comcon = (unsigned char)COMCON;
		i2c_writebyte((unsigned char)COMCON,ADD1,SCOMCON);	//只保存低字节
	}
	else if(Stacon != (STACON&0xFF))
	{
		Stacon = (unsigned char)STACON;
		i2c_writebyte((unsigned char)STACON,ADD1,SSTACON);	//只保存低字节
	}
	else if(Baudrate != (BAUD&0xFF))
	{
		R_Config_UARTA1_Stop();
		
		if(BAUD&0xFF){R_Config_UARTA1_19200();}				//19200
		else {R_Config_UARTA1_9600();}						//9600
		
		R_Config_UARTA1_Start();							//
		R_Config_UARTA1_Receive(&ReceiveBuffer1,1);
		
		QueueReset();						//初始化MODBUS
		Baudrate = (unsigned char)BAUD;
		i2c_writebyte((unsigned char)BAUD,ADD1,SBAUD);		//只保存低字节
	}
	//CHx频率发生变化//////////////////////////////////////////////	
	else if(CH1FREQ!=CHxFREQ[0])	//CHx频率发生变化
	{
		if(CH1FREQ>99){CH1FREQ=99;}
		CHxFREQ[0]=CH1FREQ;
		i2c_writedoublebyte(CHxFREQ[0],ADD1,SCH1FREQ);
		Timer1Resetting();			//只重置定时器的中断节奏
	}
	else if(CH2FREQ!=CHxFREQ[1])
	{
		if(CH2FREQ>99){CH2FREQ=99;}
		CHxFREQ[1]=CH2FREQ;
		i2c_writedoublebyte(CHxFREQ[1],ADD1,SCH2FREQ);
		Timer2Resetting();
	}	
	else if(CH3FREQ!=CHxFREQ[2])
	{
		if(CH3FREQ>99){CH3FREQ=99;}
		CHxFREQ[2]=CH3FREQ;
		i2c_writedoublebyte(CHxFREQ[2],ADD1,SCH3FREQ);
		Timer3Resetting();
	}		
	else if(CH4FREQ!=CHxFREQ[3])
	{
		if(CH4FREQ>99){CH4FREQ=99;}
		CHxFREQ[3]=CH4FREQ;
		i2c_writedoublebyte(CHxFREQ[3],ADD1,SCH4FREQ);
		Timer4Resetting();
	}		
	else if(CH5FREQ!=CHxFREQ[4])
	{
		if(CH5FREQ>99){CH5FREQ=99;}
		CHxFREQ[4]=CH5FREQ;
		i2c_writedoublebyte(CHxFREQ[4],ADD1,SCH5FREQ);
		Timer5Resetting();
	}
	
	//CHx正峰值发生变化//////////////////////////////////////////////
	else if(CH1PPAK!=CHxPPAK[0])	//CHx正峰值发生变化
	{
		if((CH1PPAK<0xFE0C)&&(CH1PPAK>0x01F4))
		{if(CH1PPAK&0x8000){CH1PPAK=0xFE0C;}else{CH1PPAK=0x01F4;}}
		//if((CH1PPAK)>500L){CH1PPAK=500L;}
		//else if((CH1PPAK)<(-500L)){CH1PPAK=(-500L);}
		CHxPPAK[0]=CH1PPAK;
		i2c_writedoublebyte(CHxPPAK[0],ADD1,SCH1PPAK);
		CHxMODE[0]=0xFF;			//强迫修改模式，实现波形数据重置
	}
	else if(CH2PPAK!=CHxPPAK[1])
	{
		if((CH2PPAK<0xFE0C)&&(CH2PPAK>0x01F4))
		{if(CH2PPAK&0x8000){CH2PPAK=0xFE0C;}else{CH2PPAK=0x01F4;}}		
		//if((CH2PPAK)>500L){CH2PPAK=500L;}
		//else if((CH2PPAK)<(-500L)){CH2PPAK=(-500L);}
		CHxPPAK[1]=CH2PPAK;
		i2c_writedoublebyte(CHxPPAK[1],ADD1,SCH2PPAK);
		CHxMODE[1]=0xFF;
	}
	else if(CH3PPAK!=CHxPPAK[2])
	{
		if((CH3PPAK<0xFE0C)&&(CH3PPAK>0x01F4))
		{if(CH3PPAK&0x8000){CH3PPAK=0xFE0C;}else{CH3PPAK=0x01F4;}}
		//if((CH3PPAK)>500L){CH3PPAK=500L;}
		//else if((CH3PPAK)<(-500L)){CH3PPAK=(-500L);}
		CHxPPAK[2]=CH3PPAK;
		i2c_writedoublebyte(CHxPPAK[2],ADD1,SCH3PPAK);
		CHxMODE[2]=0xFF;
	}
	else if(CH4PPAK!=CHxPPAK[3])
	{
		if((CH4PPAK<0xFE0C)&&(CH4PPAK>0x01F4))
		{if(CH4PPAK&0x8000){CH4PPAK=0xFE0C;}else{CH4PPAK=0x01F4;}}		
		//if((CH4PPAK)>500L){CH4PPAK=500L;}
		//else if((CH4PPAK)<(-500L)){CH4PPAK=(-500L);}
		CHxPPAK[3]=CH4PPAK;
		i2c_writedoublebyte(CHxPPAK[3],ADD1,SCH4PPAK);
		CHxMODE[3]=0xFF;
	}
	else if(CH5PPAK!=CHxPPAK[4])
	{
		if((CH5PPAK<0xFE0C)&&(CH5PPAK>0x01F4))
		{if(CH5PPAK&0x8000){CH5PPAK=0xFE0C;}else{CH5PPAK=0x01F4;}}		
		//if((CH5PPAK)>500L){CH5PPAK=500L;}
		//else if((CH5PPAK)<(-500L)){CH5PPAK=(-500L);}
		CHxPPAK[4]=CH5PPAK;
		i2c_writedoublebyte(CHxPPAK[4],ADD1,SCH5PPAK);
		CHxMODE[4]=0xFF;			
	}
	
	//CHx负峰值发生变化//////////////////////////////////////////////
	else if(CH1NPAK!=CHxNPAK[0])	//CHx正峰值发生变化
	{
		if((CH1NPAK<0xFE0C)&&(CH1NPAK>0x01F4))
		{if(CH1NPAK&0x8000){CH1NPAK=0xFE0C;}else{CH1NPAK=0x01F4;}}		
		//if((CH1NPAK)>500L){CH1NPAK=500L;}
		//else if((CH1NPAK)<(-500L)){CH1NPAK=(-500L);}
		CHxNPAK[0]=CH1NPAK;
		i2c_writedoublebyte(CHxNPAK[0],ADD1,SCH1NPAK);
		CHxMODE[0]=0xFF;			//强迫修改模式，实现波形数据重置
	}
	else if(CH2NPAK!=CHxNPAK[1])
	{
		if((CH2NPAK<0xFE0C)&&(CH2NPAK>0x01F4))
		{if(CH2NPAK&0x8000){CH2NPAK=0xFE0C;}else{CH2NPAK=0x01F4;}}		
		//if((CH2NPAK)>500L){CH2NPAK=500L;}
		//else if((CH2NPAK)<(-500L)){CH2NPAK=(-500L);}
		CHxNPAK[1]=CH2NPAK;
		i2c_writedoublebyte(CHxNPAK[1],ADD1,SCH2NPAK);
		CHxMODE[1]=0xFF;
	}
	else if(CH3NPAK!=CHxNPAK[2])
	{
		if((CH3NPAK<0xFE0C)&&(CH3NPAK>0x01F4))
		{if(CH3NPAK&0x8000){CH3NPAK=0xFE0C;}else{CH3NPAK=0x01F4;}}		
		//if((CH3NPAK)>500L){CH3NPAK=500L;}
		//else if((CH3NPAK)<(-500L)){CH3NPAK=(-500L);}
		CHxNPAK[2]=CH3NPAK;
		i2c_writedoublebyte(CHxNPAK[2],ADD1,SCH3NPAK);
		CHxMODE[2]=0xFF;
	}
	else if(CH4NPAK!=CHxNPAK[3])
	{
		if((CH4NPAK<0xFE0C)&&(CH4NPAK>0x01F4))
		{if(CH4NPAK&0x8000){CH4NPAK=0xFE0C;}else{CH4NPAK=0x01F4;}}		
		//if((CH4NPAK)>500L){CH4NPAK=500L;}
		//else if((CH4NPAK)<(-500L)){CH4NPAK=(-500L);}
		CHxNPAK[3]=CH4NPAK;
		i2c_writedoublebyte(CHxNPAK[3],ADD1,SCH4NPAK);
		CHxMODE[3]=0xFF;
	}
	else if(CH5NPAK!=CHxNPAK[4])
	{
		if((CH5NPAK<0xFE0C)&&(CH5NPAK>0x01F4))
		{if(CH5NPAK&0x8000){CH5NPAK=0xFE0C;}else{CH5NPAK=0x01F4;}}		
		//if((CH5NPAK)>500L){CH5NPAK=500L;}
		//else if((CH5NPAK)<(-500L)){CH5NPAK=(-500L);}
		CHxNPAK[4]=CH5NPAK;
		i2c_writedoublebyte(CHxNPAK[4],ADD1,SCH5NPAK);
		CHxMODE[4]=0xFF;
	}
	
	//CHx其他参数发生变化(被动适配)//////////////////////////////////////////////
	else if(CH1PAR1!=CHxPAR1[0])	//CHx方波:正脉宽 三角:上升
	{
		if(CH1PAR1>100){CH1PAR1=100;}		//用户输入错误
		if((CH1PAR2+CH1PAR1)>100)			//
		{
			CH1PAR3=0;
			CH1PAR2=100-CH1PAR1;			//被动修改参数2且清零参数3
			CHxPAR2[0]=CH1PAR2;				//保存参数2
			i2c_writedoublebyte(CHxPAR2[0],ADD1,SCH1PAR2);
		}
		else{CH1PAR3=100-CH1PAR1-CH1PAR2;}	//被动修改参数3且参数2不变
		CHxPAR1[0]=CH1PAR1;
		i2c_writedoublebyte(CHxPAR1[0],ADD1,SCH1PAR1);	//保存参数1
		if((CH1MODE==SQUAWAVE)||(CH1MODE==TRIAWAVE)){CHxMODE[0]=0xFF;}	//矩形波和三角波模式时波形数据重置
	}
	else if(CH2PAR1!=CHxPAR1[1])
	{
		if(CH2PAR1>100){CH2PAR1=100;}
		if((CH2PAR2+CH2PAR1)>100)
		{
			CH2PAR3=0;
			CH2PAR2=100-CH2PAR1;	
			CHxPAR2[1]=CH2PAR2;
			i2c_writedoublebyte(CHxPAR2[1],ADD1,SCH2PAR2);
		}
		else
		{CH2PAR3=100-CH2PAR1-CH2PAR2;}
		CHxPAR1[1]=CH2PAR1;
		i2c_writedoublebyte(CHxPAR1[1],ADD1,SCH2PAR1);	
		if((CH2MODE==SQUAWAVE)||(CH2MODE==TRIAWAVE)){CHxMODE[1]=0xFF;}
	}
	else if(CH3PAR1!=CHxPAR1[2])
	{
		if(CH3PAR1>100){CH3PAR1=100;}
		if((CH3PAR2+CH3PAR1)>100)
		{
			CH3PAR3=0;
			CH3PAR2=100-CH3PAR1;	
			CHxPAR2[2]=CH3PAR2;
			i2c_writedoublebyte(CHxPAR2[2],ADD1,SCH3PAR2);
		}
		else{CH3PAR3=100-CH3PAR1-CH3PAR2;}
		CHxPAR1[2]=CH3PAR1;
		i2c_writedoublebyte(CHxPAR1[2],ADD1,SCH3PAR1);	
		if((CH3MODE==SQUAWAVE)||(CH3MODE==TRIAWAVE)){CHxMODE[2]=0xFF;}
	}
	else if(CH4PAR1!=CHxPAR1[3])
	{
		if(CH4PAR1>100){CH4PAR1=100;}
		if((CH4PAR2+CH4PAR1)>100)
		{
			CH4PAR3=0;
			CH4PAR2=100-CH4PAR1;	
			CHxPAR2[3]=CH4PAR2;
			i2c_writedoublebyte(CHxPAR2[3],ADD1,SCH4PAR2);
		}
		else{CH4PAR3=100-CH4PAR1-CH4PAR2;}
		CHxPAR1[3]=CH4PAR1;
		i2c_writedoublebyte(CHxPAR1[3],ADD1,SCH4PAR1);	
		if((CH4MODE==SQUAWAVE)||(CH4MODE==TRIAWAVE)){CHxMODE[3]=0xFF;}
	}
	else if(CH5PAR1!=CHxPAR1[4])
	{
		if(CH5PAR1>100){CH5PAR1=100;}
		if((CH5PAR2+CH5PAR1)>100)
		{
			CH5PAR3=0;
			CH5PAR2=100-CH5PAR1;	
			CHxPAR2[4]=CH5PAR2;
			i2c_writedoublebyte(CHxPAR2[4],ADD1,SCH5PAR2);
		}
		else{CH5PAR3=100-CH5PAR1-CH5PAR2;}
		CHxPAR1[4]=CH5PAR1;
		i2c_writedoublebyte(CHxPAR1[4],ADD1,SCH5PAR1);	
		if((CH5MODE==SQUAWAVE)||(CH5MODE==TRIAWAVE)){CHxMODE[4]=0xFF;}
	}
	//--------------------------------------------
	else if(CH1PAR2!=CHxPAR2[0])	//CHx方波:负脉宽 三角:下降
	{
		if(CH1PAR2>100){CH1PAR2=100;}		//用户输入错误
		if((CH1PAR2+CH1PAR1)>100)			//
		{
			CH1PAR3=0;	
			CH1PAR1=100-CH1PAR2;			//被动修改参数1且清零参数3
			CHxPAR1[0]=CH1PAR1;
			i2c_writedoublebyte(CHxPAR1[0],ADD1,SCH1PAR1);	//保存参数1			
		}
		else{CH1PAR3=100-CH1PAR1-CH1PAR2;}	//被动修改参数3且参数2不变				
		CHxPAR2[0]=CH1PAR2;					//保存参数2
		i2c_writedoublebyte(CHxPAR2[0],ADD1,SCH1PAR2);
		if((CH1MODE==SQUAWAVE)||(CH1MODE==TRIAWAVE)){CHxMODE[0]=0xFF;}	//矩形波和三角波模式时波形数据重置		
	}
	else if(CH2PAR2!=CHxPAR2[1])
	{
		if(CH2PAR2>100){CH2PAR2=100;}
		if((CH2PAR2+CH2PAR1)>100)	
		{
			CH2PAR3=0;	
			CH2PAR1=100-CH2PAR2;
			CHxPAR1[1]=CH2PAR1;
			i2c_writedoublebyte(CHxPAR1[1],ADD1,SCH2PAR1);		
		}
		else{CH2PAR3=100-CH2PAR1-CH2PAR2;}
		CHxPAR2[1]=CH2PAR2;
		i2c_writedoublebyte(CHxPAR2[1],ADD1,SCH2PAR2);
		if((CH2MODE==SQUAWAVE)||(CH2MODE==TRIAWAVE)){CHxMODE[1]=0xFF;}	
	}
	else if(CH3PAR2!=CHxPAR2[2])
	{
		if(CH3PAR2>100){CH3PAR2=100;}
		if((CH3PAR2+CH3PAR1)>100)	
		{
			CH3PAR3=0;	
			CH3PAR1=100-CH3PAR2;
			CHxPAR1[2]=CH3PAR1;
			i2c_writedoublebyte(CHxPAR1[2],ADD1,SCH3PAR1);		
		}
		else{CH3PAR3=100-CH3PAR1-CH3PAR2;}
		CHxPAR2[2]=CH3PAR2;
		i2c_writedoublebyte(CHxPAR2[2],ADD1,SCH3PAR2);
		if((CH3MODE==SQUAWAVE)||(CH3MODE==TRIAWAVE)){CHxMODE[2]=0xFF;}	
	}
	else if(CH4PAR2!=CHxPAR2[3])
	{
		if(CH4PAR2>100){CH4PAR2=100;}
		if((CH4PAR2+CH4PAR1)>100)	
		{
			CH4PAR3=0;	
			CH4PAR1=100-CH4PAR2;
			CHxPAR1[3]=CH4PAR1;
			i2c_writedoublebyte(CHxPAR1[3],ADD1,SCH4PAR1);		
		}
		else{CH4PAR3=100-CH4PAR1-CH4PAR2;}
		CHxPAR2[3]=CH4PAR2;
		i2c_writedoublebyte(CHxPAR2[3],ADD1,SCH4PAR2);
		if((CH4MODE==SQUAWAVE)||(CH4MODE==TRIAWAVE)){CHxMODE[3]=0xFF;}	
	}
	else if(CH5PAR2!=CHxPAR2[4])
	{
		if(CH5PAR2>100){CH5PAR2=100;}
		if((CH5PAR2+CH5PAR1)>100)	
		{
			CH5PAR3=0;	
			CH5PAR1=100-CH5PAR2;
			CHxPAR1[4]=CH5PAR1;
			i2c_writedoublebyte(CHxPAR1[4],ADD1,SCH5PAR1);		
		}
		else{CH5PAR3=100-CH5PAR1-CH5PAR2;}
		CHxPAR2[4]=CH5PAR2;
		i2c_writedoublebyte(CHxPAR2[4],ADD1,SCH5PAR2);
		if((CH5MODE==SQUAWAVE)||(CH5MODE==TRIAWAVE)){CHxMODE[4]=0xFF;}	
	}
	//组和队//////////////////////////////////////////////
	else if(CH1GRO!=CHxGRO[0])
	{
		if(CH1GRO>9)CH1GRO=9;
		CHxGRO[0]=CH1GRO;
		i2c_writedoublebyte(CHxGRO[0],ADD1,SCH1GRO);
		TeamUp();			//重置队列
	}
	else if(CH1ROW!=CHxROW[0])
	{
		if(CH1ROW>9)CH1ROW=9;
		CHxROW[0]=CH1ROW;
		i2c_writedoublebyte(CHxROW[0],ADD1,SCH1ROW);
		TeamUp();			//重置队列
	}
	else if(CH2GRO!=CHxGRO[1])
	{
		if(CH2GRO>9)CH2GRO=9;
		CHxGRO[1]=CH2GRO;
		i2c_writedoublebyte(CHxGRO[1],ADD1,SCH2GRO);
		TeamUp();
	}
	else if(CH2ROW!=CHxROW[1])
	{
		if(CH2ROW>9)CH2ROW=9;
		CHxROW[1]=CH2ROW;
		i2c_writedoublebyte(CHxROW[1],ADD1,SCH2ROW);	
		TeamUp();	
	}
	else if(CH3GRO!=CHxGRO[2])
	{
		if(CH3GRO>9)CH3GRO=9;
		CHxGRO[2]=CH3GRO;
		i2c_writedoublebyte(CHxGRO[2],ADD1,SCH3GRO);
		TeamUp();
	}
	else if(CH3ROW!=CHxROW[2])
	{
		if(CH3ROW>9)CH3ROW=9;
		CHxROW[2]=CH3ROW;
		i2c_writedoublebyte(CHxROW[2],ADD1,SCH3ROW);
		TeamUp();		
	}	
	else if(CH4GRO!=CHxGRO[3])
	{
		if(CH4GRO>9)CH4GRO=9;
		CHxGRO[3]=CH4GRO;
		i2c_writedoublebyte(CHxGRO[3],ADD1,SCH4GRO);
		TeamUp();
	}
	else if(CH4ROW!=CHxROW[3])
	{
		if(CH4ROW>9)CH4ROW=9;
		CHxROW[3]=CH4ROW;
		i2c_writedoublebyte(CHxROW[3],ADD1,SCH4ROW);	
		TeamUp();	
	}
	else if(CH5GRO!=CHxGRO[4])
	{
		if(CH5GRO>9)CH5GRO=9;
		CHxGRO[4]=CH5GRO;
		i2c_writedoublebyte(CHxGRO[4],ADD1,SCH5GRO);
		TeamUp();
	}
	else if(CH5ROW!=CHxROW[4])
	{
		if(CH5ROW>9)CH5ROW=9;
		CHxROW[4]=CH5ROW;
		i2c_writedoublebyte(CHxROW[4],ADD1,SCH5ROW);	
		TeamUp();	
	}
		//CHx模式发生变化//////////////////////////////////////////////
	else if(CH1MODE!=CHxMODE[0])	//CHx模式发生变化
	{
		if(CH1MODE>3){CH1MODE = CHxMODE[0];return;}		//参数错误则回滚参数
		if(CHxMODE[0]<0xFF){i2c_writedoublebyte(CH1MODE,ADD1,SCH1MODE);}
		CHxMODE[0]= CH1MODE;		//非强迫，真实模式切换，需保存模式
		ConfigWaveData(0);			//重置CH1通道的波形数据
		Timer1Resetting();			//重置CH1通道的频率
	}
	else if(CH2MODE!=CHxMODE[1])
	{
		if(CH2MODE>3){CH2MODE = CHxMODE[1];return;}
		if(CHxMODE[1]<0xFF){i2c_writedoublebyte(CH2MODE,ADD1,SCH2MODE);}
		CHxMODE[1]= CH2MODE;
		ConfigWaveData(1);
		Timer2Resetting();
	}	
	else if(CH3MODE!=CHxMODE[2])
	{
		if(CH3MODE>3){CH3MODE = CHxMODE[2];return;}
		if(CHxMODE[2]<0xFF){i2c_writedoublebyte(CH3MODE,ADD1,SCH3MODE);}
		CHxMODE[2]= CH3MODE;
		ConfigWaveData(2);
		Timer3Resetting();
	}	
	else if(CH4MODE!=CHxMODE[3])
	{
		if(CH4MODE>3){CH4MODE = CHxMODE[3];return;}
		if(CHxMODE[3]<0xFF){i2c_writedoublebyte(CH4MODE,ADD1,SCH4MODE);}
		CHxMODE[3]= CH4MODE;
		ConfigWaveData(3);
		Timer4Resetting();
	}	
	else if(CH5MODE!=CHxMODE[4])
	{
		if(CH5MODE>3){CH5MODE = CHxMODE[4];return;}
		if(CHxMODE[4]<0xFF){i2c_writedoublebyte(CH5MODE,ADD1,SCH5MODE);}
		CHxMODE[4]= CH5MODE;
		ConfigWaveData(4);
		Timer5Resetting();
	}
}

