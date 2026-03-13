
#ifndef __24C02_H__
#define __24C02_H__

#define ADD1   0xa0  	//第一个器件地址
//#define ADD2   0xa2  	//第二个器件地址
//#define ADD3   0xa4  	//第三个器件地址
//#define ADD4   0xa6  	//第四个器件地址
extern void i2c_writebyte(unsigned char dat, unsigned char slave, unsigned char add);
extern unsigned char i2c_readbyte(unsigned char slave, unsigned char add);
extern void i2c_writedoublebyte(unsigned int dat, unsigned char slave, unsigned char add);
extern unsigned int i2c_readdoublebyte(unsigned char slave, unsigned char add);
#endif