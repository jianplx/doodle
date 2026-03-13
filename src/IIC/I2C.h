#ifndef __I2C_H__
#define __I2C_H__

#include "iodefine.h"

/********************************************************************/
#define	SCL			P6_bit.no0		//时钟
#define	SDA			P6_bit.no1		//数据
#define SDAD1 		PM6_bit.no1=0	//输出模式
#define SDAD0		PM6_bit.no1=1	//输入模式
#define SDAU1		PU6_bit.no1=1	//上拉模式
/*********************************************************************/

extern void i2c_start(void);
extern void i2c_stop(void);
extern void i2c_ACK(unsigned char ck);
extern unsigned char  i2c_waitACK(void);
extern void i2c_sendbyte(unsigned char bt);
extern unsigned char i2c_recbyte(void);

//extern void SendACKI2C( void);
//extern void SendNoACKI2C( void);
#endif