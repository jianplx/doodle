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
* File Name        : Pin.h
* Version          : 1.0.0
* Device(s)        : R7F100GGJxFB
* Description      : This file implements SMC pin code generation.
***********************************************************************************************************************/

#ifndef PIN_H
#define PIN_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* User's guide for symbolic name.
 * The generated symbolic names can be used in the user application as follows: 
 *
 * Example: Toggle LED1 at Pin P54.
 *  There are 2 ways to toggle LED1
 *   1) Using symbolic name macro
 *    Assuming the symbolic name for P54 is "LED1", the generated macro definition will be:
 *         #define LED1    5,4
 *
 *    To use this macro definition to toggle the LED1, call the symbolic name APIs:
 *         PIN_WRITE(LED1) = ~PIN_READ(LED1)
 *
 *   2) Not using symbolic name macro
 *    Call the symbolic name APIs directly
 *         PIN_WRITE(5,4) = ~PIN_READ(5,4)
 */

/* Symbolic name */
#define Pwm4        1,0
#define I1        14,6
#define I2        14,7
#define CH1        2,7
#define CH2        2,6
#define CH3        2,5
#define CH4        2,4
#define CH5        2,3
#define K1        13,0
#define K2        0,1
#define I5        5,0
#define K3        0,0
#define I4        5,1
#define K4        14,0
#define I3        1,6
#define Pwm1        1,4
#define Pwm2        1,3
#define SCL1        6,0
#define SDA1        6,1
#define ON5x        6,2
#define ON4x        6,3
#define ON3        3,1
#define ON2        7,5
#define ON1        7,4
#define K5        7,3
#define ON5        12,4
#define ON4        12,3
#define Pwm5        1,2
#define Pwm3        1,1

/* User's guide for pin function assignment macros
 * The generated macro definitions can be used in the user application as follows: 
 *
 * Example: Set SCLA0 port to 1U.
 *  PIN_WRITE(SMC_PIN_TO02) = 1;
 *  Xxx = PIN_READ(R_xxx_TI00_Pin);
 */

/* PIOR pin function assignments */
#define SMC_PIN_TO04        1,3
#define SMC_PIN_TO03        1,4
#define SMC_PIN_TO07        1,0
#define SMC_PIN_TO06        1,1
#define SMC_PIN_TO05        1,2

/* Pin write helper */
#define PIN_WRITE_HELPER(x,y)                    ((P##x##_bit.no##y))
/* Pin read helper */
#define PIN_READ_HELPER(x,y)                     ((P##x##_bit.no##y))

/* Pin write API */
#define PIN_WRITE(...)                           (PIN_WRITE_HELPER(__VA_ARGS__))
/* Pin read API */
#define PIN_READ(...)                            (PIN_READ_HELPER(__VA_ARGS__))

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Pins_Create(void);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
