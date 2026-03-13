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
* File Name        : r_cg_userdefine.h
* Version          : 1.0.51
* Device(s)        : R7F100GGJxFB
* Description      : User header file for code generation.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#ifndef USER_DEF_H
#define USER_DEF_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/* Start user code for register. Do not edit comment generated here */
#define	POLCH1			P1_bit.no5		//���Կ���
#define	POLCH2			P1_bit.no6
#define	POLCH3			P1_bit.no7
#define	POLCH4			P5_bit.no1
#define	POLCH5			P5_bit.no0

#define	ERLCH1			P14_bit.no6		//�̵�������
#define	ERLCH2			P14_bit.no7
#define	ERLCH3			P3_bit.no0
#define	ERLCH4			P7_bit.no0
#define	ERLCH5			P7_bit.no3

#define	CONCH1			P3_bit.no1		//��������
#define	CONCH2			P7_bit.no5
#define	CONCH3			P7_bit.no4
#define	CONCH4			P6_bit.no3
#define	CONCH5			P6_bit.no2
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Start user code for macro define. Do not edit comment generated here */
#define	DIERCCUU		0		//ֱ��ģʽ
#define	SINEWAVE		1		//���Ҳ�ģʽ
#define	SQUAWAVE		2		//���β�ģʽ
#define	TRIAWAVE		3		//���ǲ�ģʽ

#define	WAVEPOINT		500UL		//���ε�λ��
#define	FCLK			10000000UL	//����	FCLK/WAVEPOINT/CHxFREQ
#define	FIHP			32000000UL	//��Ƶ  	FIHP/WAVEPOINT/CHxFREQ

#define	MAXPWM			0x063F+1	//���ڱ궨��������ѹ 100%PWM��Ƶ
#define	OUTLOAD			5			//������غ�
#define	MAXVOLT			500			//����ʱ��ʾ�ĵ�ѹֵ

#define	PZERO			1

//#define R_RFD_MCU_FLASH_T01_CATEGORY01
//#define R_RFD_MCU_FLASH_T01_CATEGORY02
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Start user code for type define. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
extern unsigned char ReceiveBuffer0;			//���ڽ���Buff
extern unsigned char ReceiveBuffer1;			//
extern   signed int  WaveTable[5][WAVEPOINT];	//ÿ��ͨ���Ĳ�������
extern unsigned int  pointer[5];				//ÿ��ͨ�����εĳ�ʼָ��
extern unsigned int  DutyCycle[5];				//ÿ��ͨ�����ε�ռ�ձ�
extern unsigned char Phase[5];					//����
extern unsigned char Switch[5];					//5·������ 0x00:���� 0xFF:�ɿ�
extern unsigned int  SysTime;
/* End user code. Do not edit comment generated here */
#endif
