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
* File Name        : Config_TAU0_2.c
* Component Version: 1.10.1
* Device(s)        : R7F100GGJxFB
* Description      : This file implements device driver for Config_TAU0_2.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_TAU0_2.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_TAU0_2_Create
* Description  : This function initializes the TAU0 channel2 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_TAU0_2_Create(void)
{
    TPS0 &= _FFF0_TAU_CKM0_CLEAR;
    TPS0 |= _0000_TAU_CKM0_FCLK_0;
    TT0 |= (_0080_TAU_CH7_STOP_TRG_ON | _0040_TAU_CH6_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | 
           _0010_TAU_CH4_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON);
    TMMK02 = 1U;    /* disable INTTM02 interrupt */
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
    TMMK03 = 1U;    /* disable INTTM03 interrupt */
    TMIF03 = 0U;    /* clear INTTM03 interrupt flag */
    TMMK04 = 1U;    /* disable INTTM04 interrupt */
    TMIF04 = 0U;    /* clear INTTM04 interrupt flag */
    TMMK05 = 1U;    /* disable INTTM05 interrupt */
    TMIF05 = 0U;    /* clear INTTM05 interrupt flag */
    TMMK06 = 1U;    /* disable INTTM06 interrupt */
    TMIF06 = 0U;    /* clear INTTM06 interrupt flag */
    TMMK07 = 1U;    /* disable INTTM07 interrupt */
    TMIF07 = 0U;    /* clear INTTM07 interrupt flag */
    /* Set INTTM02 level 1 priority */
    TMPR102 = 0U;
    TMPR002 = 1U;
    /* Channel 2 is used as master channel for PWM output function */
    TMR02 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0800_TAU_COMBINATION_MASTER | 
            _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
    TDR02 = _04FF_TAU_TDR02_VALUE;
    TOM0 &= (uint16_t)~_0004_TAU_CH2_SLAVE_OUTPUT;
    TOL0 &= (uint16_t)~_0004_TAU_CH2_OUTPUT_LEVEL_L;
    TO0 &= (uint16_t)~_0004_TAU_CH2_OUTPUT_VALUE_1;
    TOE0 &= (uint16_t)~_0004_TAU_CH2_OUTPUT_ENABLE;
    /* Channel 3 is used as slave channel for PWM output function */
    TMR03 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR03 = _0000_TAU_TDR03_VALUE;
    TOM0 |= _0008_TAU_CH3_SLAVE_OUTPUT;
    TOL0 |= _0008_TAU_CH3_OUTPUT_LEVEL_L;
    TO0 |= _0008_TAU_CH3_OUTPUT_VALUE_1;
    TOE0 |= _0008_TAU_CH3_OUTPUT_ENABLE;
    /* Channel 4 is used as slave channel for PWM output function */
    TMR04 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR04 = _0000_TAU_TDR04_VALUE;
    TOM0 |= _0010_TAU_CH4_SLAVE_OUTPUT;
    TOL0 |= _0010_TAU_CH4_OUTPUT_LEVEL_L;
    TO0 |= _0010_TAU_CH4_OUTPUT_VALUE_1;
    TOE0 |= _0010_TAU_CH4_OUTPUT_ENABLE;
    /* Channel 5 is used as slave channel for PWM output function */
    TMR05 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR05 = _0000_TAU_TDR05_VALUE;
    TOM0 |= _0020_TAU_CH5_SLAVE_OUTPUT;
    TOL0 |= _0020_TAU_CH5_OUTPUT_LEVEL_L;
    TO0 |= _0020_TAU_CH5_OUTPUT_VALUE_1;
    TOE0 |= _0020_TAU_CH5_OUTPUT_ENABLE;
    /* Channel 6 is used as slave channel for PWM output function */
    TMR06 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR06 = _0000_TAU_TDR06_VALUE;
    TOM0 |= _0040_TAU_CH6_SLAVE_OUTPUT;
    TOL0 |= _0040_TAU_CH6_OUTPUT_LEVEL_L;
    TO0 |= _0040_TAU_CH6_OUTPUT_VALUE_1;
    TOE0 |= _0040_TAU_CH6_OUTPUT_ENABLE;
    /* Channel 7 is used as slave channel for PWM output function */
    TMR07 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR07 = _0000_TAU_TDR07_VALUE;
    TOM0 |= _0080_TAU_CH7_SLAVE_OUTPUT;
    TOL0 |= _0080_TAU_CH7_OUTPUT_LEVEL_L;
    TO0 |= _0080_TAU_CH7_OUTPUT_VALUE_1;
    TOE0 |= _0080_TAU_CH7_OUTPUT_ENABLE;
    /* Set TO03 pin */
    POM1 &= 0xEFU;
    PMCE1 &= 0xEFU;
    PFOE0 |= 0x08U;
    P1 &= 0xEFU;
    PM1 &= 0xEFU;
    /* Set TO04 pin */
    POM1 &= 0xF7U;
    PMCA1 &= 0xF7U;
    PMCE1 &= 0xF7U;
    PFOE0 |= 0x10U;
    P1 &= 0xF7U;
    PM1 &= 0xF7U;
    /* Set TO05 pin */
    POM1 &= 0xFBU;
    PMCE1 &= 0xFBU;
    PFOE0 |= 0x20U;
    P1 &= 0xFBU;
    PM1 &= 0xFBU;
    /* Set TO06 pin */
    POM1 &= 0xFDU;
    PMCE1 &= 0xFDU;
    PFOE0 |= 0x40U;
    P1 &= 0xFDU;
    PM1 &= 0xFDU;
    /* Set TO07 pin */
    POM1 &= 0xFEU;
    PMCE1 &= 0xFEU;
    PFOE0 |= 0x80U;
    P1 &= 0xFEU;
    PM1 &= 0xFEU;

    R_Config_TAU0_2_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_TAU0_2_Start
* Description  : This function starts the TAU0 channel2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_TAU0_2_Start(void)
{
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
    TMMK02 = 0U;    /* enable INTTM02 interrupt */
    TOE0 |= (_0080_TAU_CH7_OUTPUT_ENABLE | _0040_TAU_CH6_OUTPUT_ENABLE | _0020_TAU_CH5_OUTPUT_ENABLE | 
            _0010_TAU_CH4_OUTPUT_ENABLE | _0008_TAU_CH3_OUTPUT_ENABLE);
    TS0 |= (_0080_TAU_CH7_START_TRG_ON | _0040_TAU_CH6_START_TRG_ON | _0020_TAU_CH5_START_TRG_ON | 
           _0010_TAU_CH4_START_TRG_ON | _0008_TAU_CH3_START_TRG_ON | _0004_TAU_CH2_START_TRG_ON);
}

/***********************************************************************************************************************
* Function Name: R_Config_TAU0_2_Stop
* Description  : This function stops the TAU0 channel2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_TAU0_2_Stop(void)
{
    TT0 |= (_0080_TAU_CH7_STOP_TRG_ON | _0040_TAU_CH6_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | 
           _0010_TAU_CH4_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON);
    TOE0 &= (uint16_t) ~(_0080_TAU_CH7_OUTPUT_ENABLE | _0040_TAU_CH6_OUTPUT_ENABLE | _0020_TAU_CH5_OUTPUT_ENABLE | 
            _0010_TAU_CH4_OUTPUT_ENABLE | _0008_TAU_CH3_OUTPUT_ENABLE);
    TMMK02 = 1U;    /* disable INTTM02 interrupt */
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
