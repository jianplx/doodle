/************************************************************************/
/*  FILE    :MODBUS_SLAVE.c                                          	*/
/*  DATE    :2022 05 28                                    				*/
/*  说明     :MODBUS从站协议 指令接收 回显生成 指令执行                       */
/*  备注     :通用,双口,地址保存,广播号,调试号                               	*/
/************************************************************************/

/************************************************************************
说明:
串口指令接收使用环型列队栈, 串口每接收一个数据, UQH向前增1, 
UQH!=UQT则非空栈, UQH==UQT则空栈, 头部UQH超前尾部UQT,
通过UQT来指明以设备地址DEVICE为数值的数据单元为栈尾, 由函数MobileFrameHead()来完成
若整个列队栈内都未找到以设备地址DEVICE为数值的数据单元, 则UQH与UQT重合, 标记为空栈

移植:
4_1:初始化寄存器
QueueReset();

4_2:完整的MODBUS通信功能
MODBUS_COMMUN();

4_3:串口接收中断
MODBUSQueuePush((unsigned char)(a));

4_4:定时器中断
MODBUSTiming();

通信格式:
000001-Rx:		//读取0000地址16个字节 
20 		设备码
04 		操作码
00 00 	操作地址
00 10 	操作寄存器数量
F7 77  	CRC
000002-Tx:		//本机应答主机16个地址的所有数据
20 		设备码
04 		操作码
20 		返回数据字节数
00 20 00 A4 00 14 00 00 00 05 00 30 05 DC 46 50 
00 00 00 00 00 00 00 00 00 58 00 AB 00 00 00 00
C7 4E	CRC校验

************************************************************************/

#include "CRCA001.h"
#include "MODBUS_SLAVE.h"
#include "24C02.h"
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_UARTA0.h"
#include "Config_UARTA1.h"

unsigned int  device;				//DEVICE影子
unsigned int  Register[ADDRANGE];				//MODBUS从站寄存器 输入寄存器(3X)保持寄存器(4X)
//unsigned char Coils[ADDRANGE1/8];				//MODBUS从站线圈 离散输出(0X)离散输入(1X)
const unsigned char Read_only[ADDRANGE] = {		//MODBUS从站寄存器写保护位
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned int  wait0;				//通信等待
unsigned int  Off_line0;			//通信离线
unsigned int  MODBUSQueueHead0;		//栈头部
unsigned int  MODBUSQueueTail0;		//栈尾部
unsigned char MODBUSQueue0[MODBUS_QUEUE_SIZE];	//接收到主机命令列队缓冲器(栈)
unsigned char MODBUS_CMD0[CMDRANGE];			//MODBUS指令仓

unsigned int  wait1;				//通信等待
unsigned int  Off_line1;			//通信离线
unsigned int  MODBUSQueueHead1;		//栈头部
unsigned int  MODBUSQueueTail1;		//栈尾部
unsigned char MODBUSQueue1[MODBUS_QUEUE_SIZE];	//接收到主机命令列队缓冲器(栈)
unsigned char MODBUS_CMD1[CMDRANGE];			//MODBUS指令仓

////////////////////////////////////////////////
void AddressCheck(void);
static unsigned char MoveFrameHead0(void);
static unsigned char MoveFrameHead1(void);
unsigned char FindCMD0(unsigned char *Queue);
unsigned char FindCMD1(unsigned char *Queue);
unsigned int MODBUS_RTU(unsigned char *modbus_comm);

//串口中断需要执行的函数
//MOBUS主机发送的指令存入到串口接收循环队列
void MODBUSQueuePush0(unsigned char data)
{
	unsigned int pos;
	pos = (MODBUSQueueHead0 +1) % MODBUS_QUEUE_SIZE;
    if(pos != MODBUSQueueTail0)		//如果下一个地址是栈尾则为满栈状态
    {								//非满状态(包含空状态)
        MODBUSQueue0[MODBUSQueueHead0] = data;
        MODBUSQueueHead0 = pos;		//栈头指针指向下一个地址
   	}								//否则满栈缓冲器溢出数据丢失
}
void MODBUSQueuePush1(unsigned char data)
{
	unsigned int pos;
	pos = (MODBUSQueueHead1 +1) % MODBUS_QUEUE_SIZE;
    if(pos != MODBUSQueueTail1)		//如果下一个地址是栈尾则为满栈状态
    {								//非满状态(包含空状态)
        MODBUSQueue1[MODBUSQueueHead1] = data;
        MODBUSQueueHead1 = pos;		//栈头指针指向下一个地址
   	}								//否则满栈缓冲器溢出数据丢失
}
//定时器中断需要执行的程序
void MODBUSTiming(void)
{
	if(wait0)wait0--;
	if(wait1)wait1--;
	if(Off_line0)Off_line0--;
	if(Off_line1)Off_line1--;
}
//MOBUS初始化程序
//串口接收队列（FIFO）复位
void QueueReset(void)
{
	Off_line0 = 0;
	Off_line1 = 0;
	MODBUSQueueHead0 = 0;
	MODBUSQueueTail0 = 0;
	MODBUSQueueHead1 = 0;
	MODBUSQueueTail1 = 0;
	device = i2c_readbyte(ADD1,MODBUS_ADD);	  			//读取地址 字节读(器件地址,地址)
	if((200 > device)&&(device > 10))DEVICE = device;	//合法地址
	else DEVICE = 32;									//非法初始化定义设备地址为32
}

//地址检查，如果地址变化重新保存地址
void AddressCheck(void)
{
	if(device != DEVICE)							//如果地址发生改变				
	{
		if((200 > DEVICE)&&(DEVICE > 10))		
		{
			device = DEVICE;						//使用新地址
			i2c_writebyte(DEVICE,ADD1,MODBUS_ADD);	//保存新地址到存储器
		}
		else DEVICE = device;						//地址不合法用原地址
	}	
}
//MODBUS通信   循环调用时间不可超过5mS
unsigned char MODBUS_COMMUN(void)
{
	static unsigned char returned0;
	static unsigned char returned1;
	AddressCheck();
	/***************************
	200 < wait1 			等待发送
	11  < wait1 <200		开始发送
	1   < wait1 <20		应答发送中...
	0  == wait1			应答结束(允许从栈中查找新指令)
	***************************/
	if(0x00 == wait0)			//空闲状态搜索指令
	{
		if(FindCMD0(MODBUS_CMD0) == 0x00)		//从列队中查找有效指令
		{
  			returned0 = MODBUS_RTU(MODBUS_CMD0);//执行MODBUS指令
			if(MODBUS_CMD0[0]==SILENC)wait0 = 0;//广播号(静默)(跳过应答)
			else wait0 = WAIT + 200U;			//设置指令应答延时时间
		}
	}
	if((wait0 < 200U)&&(wait0 > 20U))				//在100ms-5ms之间做出应答(超时若还未响应 则放弃应答)
	{
		Off_line0 = OVEERTIME;					//确认在线
		R_Config_UARTA0_Send(MODBUS_CMD0,returned0);	//发送应答指令
		wait0 = 0;								//应答后下个周期开始搜索新指令
	}
	
	if(0x00 == wait1)			//空闲状态搜索指令
	{
		if(FindCMD1(MODBUS_CMD1) == 0x00)		//从列队中查找有效指令
		{
  			returned1 = MODBUS_RTU(MODBUS_CMD1);//执行MODBUS指令
			if(MODBUS_CMD1[1]==SILENC)wait1 = 0;//[非标协议]广播号(静默)(跳过应答)
			else wait1 = WAIT + 200U;			//设置指令应答延时时间
		}
	}
	if((wait1 < 200U)&&(wait1 > 20U))				//在100ms-5ms之间做出应答(超时若还未响应 则放弃应答)
	{
		Off_line1 = OVEERTIME;					//确认在线
		R_Config_UARTA1_Send(MODBUS_CMD1,returned1);	//发送应答指令
		wait1 = 0;								//应答后下个周期开始搜索新指令
	}	
	return 0;				//空闲等待中
}

//通过设备地址判断指令帧帧头(栈尾)并移动帧头到与设备地址相同的位置
//注意：执行该函数先判断栈是否未空
static unsigned char MoveFrameHead0(void)
{
	while(1)			
	{
		if(MODBUSQueueHead0 == MODBUSQueueTail0) return 1;				//直至栈为已空状态都未发现帧头 帧头已被移动到栈头位置且尚未对齐
		if(MODBUSQueue0[MODBUSQueueTail0] == DEVICE)return 0;			//遇见设备码
		if(MODBUSQueue0[MODBUSQueueTail0] == DEBUG)return 0;			//遇见调试码
		if(MODBUSQueue0[MODBUSQueueTail0] == SILENC)return 0;			//遇见广播码
		MODBUSQueue0[MODBUSQueueTail0] = 0x01;
		MODBUSQueueTail0 = (MODBUSQueueTail0 +1) % MODBUS_QUEUE_SIZE;	//继续移动
	}
}
static unsigned char MoveFrameHead1(void)
{
	while(1)			
	{
		if(MODBUSQueueHead1 == MODBUSQueueTail1) return 1;				//直至栈为已空状态都未发现帧头 帧头已被移动到栈头位置且尚未对齐
		if(MODBUSQueue1[MODBUSQueueTail1] == DEVICE)return 0;			//遇见设备码
		if(MODBUSQueue1[MODBUSQueueTail1] == DEBUG)return 0;			//遇见调试码
		if(MODBUSQueue1[MODBUSQueueTail1] == SILENC)return 0;			//遇见广播码
		MODBUSQueue1[MODBUSQueueTail1] = 0x01;
		MODBUSQueueTail1 = (MODBUSQueueTail1 +1) % MODBUS_QUEUE_SIZE;	//继续移动
	}
}

//*Queue为指令仓 从BUFF截取的有效指令放置在这个空间
//截取指令帧
//10H 帧头对齐失败
//11H 栈为空状态直接返回
//12H - 18H 指令接收中
//30H 效验失败的指令
//50H 地址不合法的指令
//00H 效验通过的指令 指令仓内数据有效
unsigned char FindCMD0(unsigned char *Queue)
{
	unsigned int pos;				//影子栈尾指针
	unsigned int crc; 				// CRC 码
	unsigned char i,x;

	if(MoveFrameHead0()) return 0x10;			//0:通过设备码对齐
	Queue[0] = MODBUSQueue0[MODBUSQueueTail0];	//1:截取设备码
	
	pos = MODBUSQueueTail0;						//获得栈尾的映射
	pos = (pos +1) % MODBUS_QUEUE_SIZE;			//影子栈尾指针指向下一个字节
	if(MODBUSQueueHead0 != pos)					//检查下一个字节是否被接收 
		Queue[1] = MODBUSQueue0[pos];			//2:截取操作码放入指令仓
	else return 0x12;							//若栈为已空状态表示指令尚在接收中且第2个数据未到达
		
	switch(Queue[1])	//通过操作码确定指令的长度
	{
		//指令长度为8字节
		//case 0x01:				//读取单个或多个离散输出:	20 01 00 00 00 10 3B 77	(RW)
		//case 0x02:				//读取单个或多个离散输入:	20 02 00 00 00 10 7F 77	(R)
		case 0x03:				//读取单个或多个保持寄存器:	20 03 00 03 00 01 72 BB	(R)
		case 0x04:				//读取单个或多个输入寄存器:	20 04 00 03 00 01 C7 7B	(RW)
		//case 0x05:			//写入单个离散输出:		20 05 00 00 FF 00 8A 8B	(00地址置1)
		case 0x06:				//写入单个保持寄存器:		20 06 00 03 00 FF 3F 3B	(03地址00FF)
			for(i=2;i<8;i++)	//截取指令后半部分
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead0 != pos)
					Queue[i]=MODBUSQueue0[pos];		//3:截取指令内容放入指令仓
				else return 0x11 + i;				//接收中
			}
			
			MODBUSQueueTail0 = (MODBUSQueueTail0 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			crc = CRCA001(Queue, 6);		//校验这条指令
			if((Queue[7] == (unsigned char)(crc)) && (Queue[6] == (unsigned char)(crc >> 8)))
				return 0x00;				//A:已经获得正确的指令()
			else 
				return 0x30;				//效验不成功
		
		//指令长度字节数可变
		case 0x10:				//写多个保持寄存器:20 10 00 03 00 03 06 FF AA FF 55 FF F0 0E C1
			for(i=2;i<7;i++)	//截取指令地址、数量、字节数部分(固定长度)
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead0 != pos)
					Queue[i]=MODBUSQueue0[pos];	//3:截取指令内容放入指令仓
				else return 0x11 + i;
			}
			x = 9 + Queue[6];
			for(i=7;i<x;i++)	//截取指令数据、CRC部分(可变长度)
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead0 != pos)
					Queue[i]=MODBUSQueue0[pos];	//4:截取指令内容放入指令仓
				else return 0x11 + i;
			}
		
			MODBUSQueueTail0 = (MODBUSQueueTail0 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			crc = CRCA001(Queue, x-2);				//校验这条指令
			if((Queue[x-1] == (unsigned char)(crc)) && (Queue[x-2] == (unsigned char)(crc >> 8)))
				return 0x00;						//A:已经获得正确的指令()
			else 
				return 0x30;					//效验不成功
		
		//未知的指令
		default:							//非正常操作数(未知指令)
			MODBUSQueueTail0 = (MODBUSQueueTail0 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			return 0x40;					//C:指令不合法(允许再次调用直到栈空)
	}
}
unsigned char FindCMD1(unsigned char *Queue)
{
	unsigned int pos;				//影子栈尾指针
	unsigned int crc; 				// CRC 码
	unsigned char i,x;

	if(MoveFrameHead1()) return 0x10;			//0:通过设备码对齐
	Queue[0] = MODBUSQueue1[MODBUSQueueTail1];	//1:截取设备码
	
	pos = MODBUSQueueTail1;						//获得栈尾的映射
	pos = (pos +1) % MODBUS_QUEUE_SIZE;			//影子栈尾指针指向下一个字节
	if(MODBUSQueueHead1 != pos)					//检查下一个字节是否被接收 
		Queue[1] = MODBUSQueue1[pos];			//2:截取操作码放入指令仓
	else return 0x12;							//若栈为已空状态表示指令尚在接收中且第2个数据未到达
		
	switch(Queue[1])	//通过操作码确定指令的长度
	{
		//指令长度为8字节
		//case 0x01:				//读取单个或多个离散输出:	20 01 00 00 00 10 3B 77	(RW)
		//case 0x02:				//读取单个或多个离散输入:	20 02 00 00 00 10 7F 77	(R)
		case 0x03:				//读取单个或多个保持寄存器:	20 03 00 03 00 01 72 BB	(R)
		case 0x04:				//读取单个或多个输入寄存器:	20 04 00 03 00 01 C7 7B	(RW)
		//case 0x05:			//写入单个离散输出:		20 05 00 00 FF 00 8A 8B	(00地址置1)
		case 0x06:				//写入单个保持寄存器:		20 06 00 03 00 FF 3F 3B	(03地址00FF)
			for(i=2;i<8;i++)	//截取指令后半部分
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead1 != pos)
					Queue[i]=MODBUSQueue1[pos];		//3:截取指令内容放入指令仓
				else return 0x11 + i;				//接收中
			}
			
			MODBUSQueueTail1 = (MODBUSQueueTail1 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			crc = CRCA001(Queue, 6);		//校验这条指令
			if((Queue[7] == (unsigned char)(crc)) && (Queue[6] == (unsigned char)(crc >> 8)))
				return 0x00;				//A:已经获得正确的指令()
			else 
				return 0x30;				//效验不成功
		
		//指令长度字节数可变
		case 0x10:				//写多个保持寄存器:20 10 00 03 00 03 06 FF AA FF 55 FF F0 0E C1
			for(i=2;i<7;i++)	//截取指令地址、数量、字节数部分(固定长度)
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead1 != pos)
					Queue[i]=MODBUSQueue1[pos];	//3:截取指令内容放入指令仓
				else return 0x11 + i;
			}
			x = 9 + Queue[6];
			for(i=7;i<x;i++)	//截取指令数据、CRC部分(可变长度)
			{
				pos = (pos +1) % MODBUS_QUEUE_SIZE;
				if(MODBUSQueueHead1 != pos)
					Queue[i]=MODBUSQueue1[pos];	//4:截取指令内容放入指令仓
				else return 0x11 + i;
			}
		
			MODBUSQueueTail1 = (MODBUSQueueTail1 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			crc = CRCA001(Queue, x-2);				//校验这条指令
			if((Queue[x-1] == (unsigned char)(crc)) && (Queue[x-2] == (unsigned char)(crc >> 8)))
				return 0x00;						//A:已经获得正确的指令()
			else 
				return 0x30;					//效验不成功
		
		//未知的指令
		default:							//非正常操作数(未知指令)
			MODBUSQueueTail1 = (MODBUSQueueTail1 +1) % MODBUS_QUEUE_SIZE;	//栈尾前移
			return 0x40;					//C:指令不合法(允许再次调用直到栈空)
	}
}

//通过03功能码读寄存器数据
static unsigned char modbus_RTU_03(unsigned char *modbus_comm)
{
	unsigned int crc; 			// CRC 码
	unsigned int address;
	unsigned int amount;		//获取寄存器的数量
	unsigned char i;
	
	address = modbus_comm[2];
	address = address <<8;
	address = address | modbus_comm[3]; //解析地址号

	if(address < ADDRANGE)
	{	
		amount = modbus_comm[4];
		amount = amount <<8;
		amount = amount | modbus_comm[5]; 	//解析要读取寄存器数量
	
		if((amount + address) > ADDRANGE) amount = ADDRANGE - address;	//把数量控制在最大地址内
		else if(0 == amount) amount = 1;	//零长度为读取当前寄存器数据

		i = 3;	//从第4个字节起返回数据
		
		while((amount > 0) && (i < (CMDRANGE -2)))	//若指令栈枯竭则不再读取下一个从站寄存器
		{
			modbus_comm[i++] = (unsigned char)(Register[address] >> 8);//获取寄存器的数据
			modbus_comm[i++] = (unsigned char)(Register[address]);
			amount--;	//amount = amount-1;
			address++;	//address = address+1;
		}
		modbus_comm[2] = i-3;
		crc = CRCA001(modbus_comm, i);	//校验的数据量为 4+ 读取寄存器数量的2倍
		modbus_comm[i++] = (unsigned char)(crc >> 8);	//修改回显校验值
		modbus_comm[i++] = (unsigned char)(crc);
		return i;		//正确执行03指令 返回生产的指令长度(大于等于8字节)
	}
	
	modbus_comm[0] = 0x00;
	modbus_comm[1] = 0x00;
	modbus_comm[2] = 0x03;
	modbus_comm[3] = 0x01;
	return 0x04;			//地址错误	
}

//通过06功能码写寄存器数据
static unsigned char modbus_RTU_06(unsigned char *modbus_comm)
{
	unsigned int address;
	
	address = modbus_comm[2];
	address = address <<8;
	address = address | modbus_comm[3]; //解析地址号
	
	if(address < ADDRANGE)
	{
		if(1 == Read_only[address])
		{
			modbus_comm[0] = 0x00;
			modbus_comm[1] = 0x00;
			modbus_comm[2] = 0x06;
			modbus_comm[3] = 0x02;
			return 0x04;		//写保护
		}
		else 
		{
			Register[address] = modbus_comm[4];			//将数据保存到指定寄存器
			Register[address] = Register[address] << 8;
			Register[address] = Register[address] | modbus_comm[5];
		}
		return 0x08;		//正确执行06指令 返回生产的指令长度(固定8字节)
	}
	
	modbus_comm[0] = 0x00;
	modbus_comm[1] = 0x00;
	modbus_comm[2] = 0x06;
	modbus_comm[3] = 0x01;
	return 0x04;			//地址错误
}

//通过10功能码连续写寄存器数据
static unsigned char modbus_RTU_10(unsigned char *modbus_comm)
{
	unsigned int crc; 				// CRC 码
	unsigned int address;
	unsigned char amount;			//需要写入的寄存器数量
	unsigned char CommPointer = 7;	//数据在指令中的起始位置
	unsigned char DataCount = 0;	//数据计数
	
	address = modbus_comm[2];
	address = address <<8;
	address = address | modbus_comm[3]; //解析地址号
	
	if(address < ADDRANGE)
	{
		amount = modbus_comm[6] >>1; 	//解析要写入的寄存器数量(双字节)
		if((amount + address) > ADDRANGE) amount = ADDRANGE - address;	//把数量控制在最大地址内
		else if(0 == amount) amount = 1;	//零长度为读取当前寄存器数据
		
		while(DataCount < amount)	//数据写入下一个从站寄存器
		{
			if(1 == Read_only[address])
			{
				break;	//遇到写保护寄存器立即终止该指令执行
			}
			else 
			{
				Register[address] = modbus_comm[CommPointer++];
				Register[address] = Register[address] << 8;
				Register[address] = Register[address] | modbus_comm[CommPointer++];
			}
			address++;		//address = address+1;
			DataCount++;	//写入成功计数
		}
		
		modbus_comm[4] = 0;				//记录成功写入的寄存器数量
		modbus_comm[5] = DataCount;
		crc = CRCA001(modbus_comm, 6);	//校验的数据量为6字节
		modbus_comm[6] = (unsigned char)(crc >> 8);	//修改回显校验值
		modbus_comm[7] = (unsigned char)(crc);
		return 0x08;		//正确执行16指令 返回生产的指令长度(固定8字节)
	}
	
	modbus_comm[0] = 0x00;
	modbus_comm[1] = 0x00;
	modbus_comm[2] = 0x06;
	modbus_comm[3] = 0x01;
	return 0x04;			//地址错误
}

//执行收到的MODBUS指令
//返回00H 非本机指令或未知指令 未执行
//返回04H 地址错误或寄存器写保护 未执行(有4字节故障码)
//返回08H及以上,执行指令后所创建新指令的长度 应答指令取代原接收指令放入指令仓
unsigned int MODBUS_RTU(unsigned char *modbus_comm)
{
	unsigned char returned = 0;

	if((modbus_comm[0]==DEVICE)||(modbus_comm[0]==DEBUG)||(modbus_comm[0]==SILENC))	//确认本机设备地址
	{
		switch(modbus_comm[1])		//指令的第2个字节为操作码
		{
		//case 0x01:
		//case 0x02:
			//returned = modbus_RTU_01(modbus_comm);
			//break;	
			
		case 0x03:
		case 0x04:
			returned = modbus_RTU_03(modbus_comm);
			break;

		case 0x06:
			returned = modbus_RTU_06(modbus_comm);
			break;
			
		case 0x10:
			returned = modbus_RTU_10(modbus_comm);
			break;
			
		default:
			returned = 0x00; //未知指令
			break;
		}
	}
	else returned = 0x00; //非本机指令

	return returned;
}

///////////////////////////////////////////////////////
