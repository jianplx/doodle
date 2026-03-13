
#ifndef __MODBUS_SLAVE_H__
#define __MODBUS_SLAVE_H__

#define DEBUG		255			//定义MODBUS调试码
#define SILENC		254			//定义MODBUS广播码
#define ADDRANGE 	64			//定义MODBUS从站寄存器数量
#define CMDRANGE 	145			//定义MODBUS指令栈大小			指令栈空间=最大服务数量*2+6
#define MODBUS_QUEUE_SIZE 310	//定义接收缓冲器大小

#define WAIT 		0			//定义通信等待时间 WAIT * 0.4mS=1.2mS
#define OVEERTIME	5000		//无指令超时后标记为离线 5000*0.4mS=2S

extern unsigned int Register[ADDRANGE];
#define DEVICE	(Register[ 0])		//定义设备地址码的存放寄存器位置
#define BAUD	(Register[ 1])		//波特率
#define STACON	(Register[ 2])		//启动权控制
//B0,B1,B2,B3:	0,自由 		1,开关		2,远程		3,本地
#define COMCON	(Register[ 3])		//通信权控制	CONFIG		//#define CONFIG 	(Register[ 2])
//B4,B5,B6,B7:	0,自由 		1,远程 		2,本地
#define WARNING	(Register[ 4])
//B0---7:报警信息
//B8--12:通道运行指示位
#define SCRFLG	(Register[ 5])		//屏幕启动标志
//B0---4:		0,屏控停止 	1,屏控启动(未使能的通道不启动)
#define IOFLG	(Register[ 6])		//IO口启动标志
//B0---4:		0,IO口释放 	1,IO口闭合
#define CH1CURR	(Register[ 7])
#define CH1VOLT	(Register[ 8])
#define CH2CURR	(Register[ 9])
#define CH2VOLT	(Register[10])
#define CH3CURR	(Register[11])
#define CH3VOLT	(Register[12])
#define CH4CURR	(Register[13])
#define CH4VOLT	(Register[14])
#define CH5CURR	(Register[15])
#define CH5VOLT	(Register[16])

#define CH1PAR3	(Register[17])	//高阶参数
#define CH2PAR3	(Register[18])
#define CH3PAR3	(Register[19])
#define CH4PAR3	(Register[20])
#define CH5PAR3	(Register[21])

#define ENABLE	(Register[22])	//启用/禁用
//B0---4:工作使能标志位
#define CONTROL	(Register[23])	//启动/停止(上电回零)
//B0---4:远程控制标志位
#define CH1MODE	(Register[24])	//波形模式：0,直流 1,正弦 2,方波 3,三角
#define CH1FREQ	(Register[25])	//频率
#define CH1PPAK	(Register[26])	//正峰值
#define CH1NPAK	(Register[27])	//负峰值
#define CH1PAR1	(Register[28])	//正脉宽/上升
#define CH1PAR2	(Register[29])	//负脉宽/下降

#define CH2MODE	(Register[30])	//波形模式：0,直流 1,正弦 2,方波 3,三角
#define CH2FREQ	(Register[31])	//频率
#define CH2PPAK	(Register[32])	//正峰值
#define CH2NPAK	(Register[33])	//负峰值
#define CH2PAR1	(Register[34])	//正脉宽/上升
#define CH2PAR2	(Register[35])	//负脉宽/下降

#define CH3MODE	(Register[36])	//波形模式：0,直流 1,正弦 2,方波 3,三角
#define CH3FREQ	(Register[37])	//频率
#define CH3PPAK	(Register[38])	//正峰值
#define CH3NPAK	(Register[39])	//负峰值
#define CH3PAR1	(Register[40])	//正脉宽/上升
#define CH3PAR2	(Register[41])	//负脉宽/下降

#define CH4MODE	(Register[42])	//波形模式：0,直流 1,正弦 2,方波 3,三角
#define CH4FREQ	(Register[43])	//频率
#define CH4PPAK	(Register[44])	//正峰值
#define CH4NPAK	(Register[45])	//负峰值
#define CH4PAR1	(Register[46])	//正脉宽/上升
#define CH4PAR2	(Register[47])	//负脉宽/下降

#define CH5MODE	(Register[48])	//波形模式：0,直流 1,正弦 2,方波 3,三角
#define CH5FREQ	(Register[49])	//频率
#define CH5PPAK	(Register[50])	//正峰值
#define CH5NPAK	(Register[51])	//负峰值
#define CH5PAR1	(Register[52])	//正脉宽/上升
#define CH5PAR2	(Register[53])	//负脉宽/下降

#define CH1GRO	(Register[54])	//组号
#define CH1ROW	(Register[55])	//队号
#define CH2GRO	(Register[56])	//组号
#define CH2ROW	(Register[57])	//队号
#define CH3GRO	(Register[58])	//组号
#define CH3ROW	(Register[59])	//队号
#define CH4GRO	(Register[60])	//组号
#define CH4ROW	(Register[61])	//队号
#define CH5GRO	(Register[62])	//组号
#define CH5ROW	(Register[63])	//队号

//IIC存储地址规划
#define	MODBUS_ADD	 	 0				//MODBUS地址 保存低地址
#define SBAUD			 2				//波特率设置
#define SSTACON			 4				//启动权设置
#define SCOMCON			 6				//通信权设置
#define SENABLE		 	 8				//启用/禁用

#define SCH1MODE		10				//波形模式：0,直流 1,正弦 2,方波 3,三角
#define SCH1FREQ		12				//频率
#define SCH1PPAK		14				//正峰值
#define SCH1NPAK		16				//负峰值
#define SCH1PAR1		18				//直流:无效 正弦:无效 方波:正脉宽 三角:上升
#define SCH1PAR2		20				//直流:无效 正弦:无效 方波:负脉宽 三角:下降

#define SCH2MODE		22				//波形模式：0,直流 1,正弦 2,方波 3,三角
#define SCH2FREQ		24				//频率
#define SCH2PPAK		26				//正峰值
#define SCH2NPAK		28				//负峰值
#define SCH2PAR1		30				//直流:无效 正弦:无效 方波:正脉宽 三角:上升
#define SCH2PAR2		32				//直流:无效 正弦:无效 方波:负脉宽 三角:下降

#define SCH3MODE		34				//波形模式：0,直流 1,正弦 2,方波 3,三角
#define SCH3FREQ		36				//频率
#define SCH3PPAK		38				//正峰值
#define SCH3NPAK		40				//负峰值
#define SCH3PAR1		42				//直流:无效 正弦:无效 方波:正脉宽 三角:上升
#define SCH3PAR2		44				//直流:无效 正弦:无效 方波:负脉宽 三角:下降

#define SCH4MODE		46				//波形模式：0,直流 1,正弦 2,方波 3,三角
#define SCH4FREQ		48				//频率
#define SCH4PPAK		50				//正峰值
#define SCH4NPAK		52				//负峰值
#define SCH4PAR1		54				//直流:无效 正弦:无效 方波:正脉宽 三角:上升
#define SCH4PAR2		56				//直流:无效 正弦:无效 方波:负脉宽 三角:下降

#define SCH5MODE		58				//波形模式：0,直流 1,正弦 2,方波 3,三角
#define SCH5FREQ		60				//频率
#define SCH5PPAK		62				//正峰值
#define SCH5NPAK		64				//负峰值
#define SCH5PAR1		66				//直流:无效 正弦:无效 方波:正脉宽 三角:上升
#define SCH5PAR2		68				//直流:无效 正弦:无效 方波:负脉宽 三角:下降

#define SCH1GRO			70				//组号
#define SCH1ROW			72				//队号
#define SCH2GRO			74				//组号
#define SCH2ROW			76				//队号
#define SCH3GRO			78				//组号
#define SCH3ROW			80				//队号
#define SCH4GRO			82				//组号
#define SCH4ROW			84				//队号
#define SCH5GRO			86				//组号
#define SCH5ROW			88				//队号

void QueueReset(void);
void MODBUSTiming(void);
void MODBUSQueuePush0(unsigned char data);
void MODBUSQueuePush1(unsigned char data);
unsigned char MODBUS_COMMUN(void);

#endif