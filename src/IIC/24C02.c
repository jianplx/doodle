
#include "24C02.H"
#include "I2C.H"

/********************************************************************************
函数功能: 随机写字节,在指定的地址(add)写入一字节数据(dat)  
	dat:数据 slave:器件地址 add:数据地址(0-xx) 
********************************************************************************/
void i2c_writebyte(unsigned char dat, unsigned char slave, unsigned char add)
{
	i2c_start();	   		    	//启动i2c
	i2c_sendbyte(slave);			//发送器件写地址
	i2c_waitACK();					//检查应答信号
	i2c_sendbyte(add);		 		//发送指定的低地址
	i2c_waitACK();
	i2c_sendbyte(dat);		   		//发送要写入的数据
	i2c_waitACK();
	i2c_stop();				  		//停止i2c
}
//写双字节
void i2c_writedoublebyte(unsigned int dat, unsigned char slave, unsigned char add)
{
	i2c_start();	   		    	//启动i2c
	i2c_sendbyte(slave);			//发送器件写地址
	i2c_waitACK();					//检查应答信号
	i2c_sendbyte(add);		 		//发送指定的低地址
	i2c_waitACK();
	i2c_sendbyte((unsigned char)(dat));		//发送要写入的数据
	i2c_waitACK();
	i2c_sendbyte((unsigned char)(dat>>8));	//发送要写入的数据
	i2c_waitACK();
	i2c_stop();				  		//停止i2c
}
/********************************************************************************
函数功能: 随机读字节,在指定的地址(add)读出一字节数据  
	slave:器件地址   add:数据地址
返回数据: hep
********************************************************************************/
unsigned char i2c_readbyte(unsigned char slave, unsigned char add)
{
	unsigned char hep = 0x00;
	i2c_start();			    	//伪写操作
	i2c_sendbyte(slave);			//发送器件写地址 
	i2c_waitACK();
	i2c_sendbyte(add);		 		//发送指定的低地址
	i2c_waitACK();
	i2c_start();					//重新启动i2c  
	i2c_sendbyte(slave | 1);		//发送器件读操作 
	i2c_waitACK();
	hep = i2c_recbyte();	    	//开始读取1字节数据
	i2c_ACK(0);						//不发送应答信号
	i2c_stop();				    	//停止i2c
	return hep;
}
//读双字节
unsigned int i2c_readdoublebyte(unsigned char slave, unsigned char add)
{	
	unsigned char temL,temH;
	i2c_start();			    	//伪写操作
	i2c_sendbyte(slave);			//发送器件写地址 
	i2c_waitACK();
	i2c_sendbyte(add);		 		//发送指定的低地址
	i2c_waitACK();
	i2c_start();					//重新启动i2c  
	i2c_sendbyte(slave | 1);		//发送器件读操作 
	i2c_waitACK();
	temL = i2c_recbyte();	    	//开始读取1字节数据
	i2c_ACK(1);						//发送应答信号
	temH = i2c_recbyte();	    	//开始读取2字节数据
	i2c_ACK(0);						//发送应答信号
	i2c_stop();				    	//停止i2c
	return (temH<<8)+temL;
}
/********************************************************************************  
	unsigned char i; 
	i2c_writebyte(0x08,ADD2,0x00);		//字节写(数据,器件地址,地址)
	i=i2c_readbyte(ADD2,0x00);	   		//字节读(器件地址,地址)
********************************************************************************/