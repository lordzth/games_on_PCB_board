
#include <intrins.h>
#include	"STC15Fxxxx.H"
#include "iic.h"

sbit    SCL=P1^7;			//IIC时钟引脚定义      Rev7.0硬件
sbit    SDA=P1^6;			//IIC数据引脚定义


//I^C时序中延时设置，具体参见各芯片的数据手册  6050推荐最小1.3us 但是会出问题，这里延时实际1.9us左右
void Delay2us(void)
{
	u8 i;
	i = 50;//11;
	while (--i);
}

//**************************************
//I2C起始信号
//**************************************
void I2C_Start(void)
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay2us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay2us();                 //延时
    SCL = 0;                    //拉低时钟线
}

//**************************************
//I2C停止信号
//**************************************
void I2C_Stop(void)
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay2us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay2us();                 //延时
}

//**************************************
//I2C接收应答信号
//**************************************
bit I2C_RecvACK(void)
{
    SDA = 1;					//释放SDA
	SCL = 1;                    //拉高时钟线
    Delay2us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay2us();                 //延时
    return CY;
}

//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(u8 dat)
{
    u8 i;
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
		_nop_();
		_nop_();
		_nop_();
		_nop_();
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay2us();             //延时
        SCL = 0;                //拉低时钟线
        Delay2us();             //延时
    }
    I2C_RecvACK();
}

//**************************************
//从I2C总线接收一个字节数据
//**************************************
u8 I2C_RecvByte(void)
{
    u8 i;
    u8 dat = 0;
    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
        SCL = 1;                //拉高时钟线
        Delay2us();             //延时
        dat |= SDA;             //读数据
        SCL = 0;                //拉低时钟线
        Delay2us();             //延时
    }
    return dat;
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(u8 REG_Address,u8 REG_data)	 
{
    I2C_Start();                  //起始信号
	Delay2us(); 
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，
    I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                   //发送停止信号
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************

u8 Single_ReadI2C(u8 REG_Address)
{
	u8 REG_data;
//	u8 i;
	I2C_Start();                   //起始信号
	Delay2us(); 
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始
	I2C_Start();                   //起始信号
	Delay2us(); 
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
//	for(i=0;i<0x7f;i++){	//连续读127
//		REG_data=I2C_RecvByte();       //读出寄存器数据
//		SDA = 0;                    //写应答信号
//		SCL = 1;                    //拉高时钟线
//		Delay2us();                 //延时
//		SCL = 0;                    //拉低时钟线
//		Delay2us();                 //延时
////		SDA = 1;                    //写应答信号
//
//	}
	
	REG_data=I2C_RecvByte();       //读出寄存器数据

	SDA = 1;                    //写应答信号
	SCL = 1;                    //拉高时钟线
	Delay2us();                 //延时
	SCL = 0;                    //拉低时钟线
	Delay2us();                 //延时
	
	I2C_Stop();                    //停止信号
	return REG_data;
}


