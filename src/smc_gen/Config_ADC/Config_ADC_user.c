/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021, 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_ADC_user.c
* Component Version: 1.8.0
* Device(s)        : R7F100GGJxFB
* Description      : This file implements device driver for Config_ADC.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_ADC.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_Config_ADC_interrupt(vect=INTAD)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_ADC_Create_UserInit
* Description  : This function adds user code after initializing the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_ADC_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_ADC_interrupt
* Description  : This function is INTAD interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_Config_ADC_interrupt(void)
{
    /* Start user code for r_Config_ADC_interrupt. Do not edit comment generated here */
	//unsigned char Current[5]={99,99,99,99,99};
	#define SAMPLING 32U										//采样次数
	static e_ad_channel_t ADCpointer = ADCHANNEL2;				//adc通道指针
	static uint8_t SmooPointer = 0x00;							//adc采样次数
	static uint16_t ANIx_Data[6][SAMPLING]={0,};				//数据缓存表
	static uint32_t ANIx_sum[6] = {0x0000,};					//累计数据
	uint16_t temp;
	
	R_Config_ADC_Stop();
	R_Config_ADC_Get_Result_10bit(&temp);						//获取ADC 4095
	ANIx_sum[ADCpointer-ADCHANNEL2] -= ANIx_Data[ADCpointer-ADCHANNEL2][SmooPointer];	//从累计数里面剔除最旧的数据
	ANIx_Data[ADCpointer-ADCHANNEL2][SmooPointer] = temp;					//将最新的数据保存到缓存表					
	ANIx_sum[ADCpointer-ADCHANNEL2] += temp;								//将最新的数据累计到累计数
	Current[ADCpointer-ADCHANNEL2] = ANIx_sum[ADCpointer-ADCHANNEL2] / SAMPLING /75;		//更新adc的数据
	////75对应5A 38对应10A显示
	if(Current[ADCpointer-ADCHANNEL2] > GUARD)
	{	
		switch(ADCpointer)					//关闭对通带的PWM输出
		{
			case ADCHANNEL2: break;
			case ADCHANNEL3: POLCH5=0;Guard[4]=GUARDING;break;	//关闭CH5输出使能
			case ADCHANNEL4: POLCH4=0;Guard[3]=GUARDING;break;	//关闭CH4输出使能
			case ADCHANNEL5: POLCH3=0;Guard[2]=GUARDING;break;	//关闭CH3输出使能
			case ADCHANNEL6: POLCH2=0;Guard[1]=GUARDING;break;	//关闭CH2输出使能
			case ADCHANNEL7: POLCH1=0;Guard[0]=GUARDING;break;	//关闭CH1输出使能
		}
	}	
	
	if((++ADCpointer)>ADCHANNEL7)								//下次处理下一个通道的数据
	{
		ADCpointer=ADCHANNEL2;
		SmooPointer = (SmooPointer +1) %SAMPLING;				//采样次数加1
	}
	R_Config_ADC_Set_ADChannel(ADCpointer);						//转换下一个通道
	R_Config_ADC_Start();										//开始转换	
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
