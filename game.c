//main programs for Paper circuit controller
#include "stc15fxxxx.h"
#include "intrins.h"
#include "iic.h"
#include "t_table.h"
#include "game.h"


//#define 	MAIN_Fosc		5529600L	//定义主时钟

//#define PIN

sbit INT3= P3^7;
sbit POWERVCC = P4^0;

sbit LED0= P1^0;
sbit LED1= P1^1;
sbit LED2= P1^2;
sbit LED3= P1^3;
sbit LED4= P1^4;
sbit LED5= P1^5;
sbit LED6= P1^6;
sbit LED7= P1^7;   //template

//define functions
// T1CKO P3.4 driver buzzer for 3.1KHz


void InitialMPR(void);
u16 ReadMPR(void);

u8 Switch_prog(void);//read intialization pin set


u16 stage_number,score_player;
u16 tries_player;
bit flag;     //success or fail
bit restart_flag;
u16 last_time = 0;    //game_lasting,minutes

long game_duration = 0;


void InitialT1CKO(void);//initial the driver the Buzzer
void IntialLowPowerWakeup(void);//initial INT3 wakeup MCU
void GoSleep(void);//Sleep mode
void BuzzerONOFF(bit on);//
void InitialTime0_1ms(void);//1ms Timer
void PowerVcc(bit on);// open Vcc for phy
void delayms(u16 ms);//
void WakeupInitial(void); //
void InitialSpeaker(void);

void AdjustTimer2(u8 keyvalue);//key sound
void SpeakerONOFF(bit on);
void TouchkeytoSound(u16 Touchkey);
void UtilityGPIO();



//define var for all
bit Timer0_1ms = 0;		//1ms标志

bit	soundon=0;		//sound on flag
bit in_game;
u8  xdata T_sin[40];
u16  keyvalue;
u8	PWM_Index;	//SPWM查表索引
u8	utility = 0;//P0.4-P0.7


//define CONST
#define NOPTOSLEEP 60000

#define SOUNDON 0x10
#define LEDON 0x20
#define	LEDKEYON	0x40
#define	LED43PAI	0x80

#define CHECKKEYTIME 30 		//30ms 
#define LEDTIME1 30					//CHECKKEYTIME*LEDTIME1

u16 reflect_time_true;
long game_time;    					//time passing
u16 delay_time;
u16 cnt_trigger;

void game_start(){
	stage_number = 0;
  delay_time = reflect_time / multiplier * delay_table[stage_number];
	delayms(delay_time);
	score_player = 0;
	tries_player = 3;
	LED_all_on();
	reflect_time_true = delay_time;
	flag = 0;
	in_game = 1;
	last_time = 2;  //2 min
	game_duration = last_time * 60000;
	game_time = 0;
}

void game_over(){
  //provided two kinds of endgames
	if (flag == 0) {//fail
		 LED0 = 1;
	   LED1 = 1;
	   LED2 = 1;
	   LED3 = 1;
	   LED4 = 1;
		 LED5 = 1;
 	   LED6 = 1;
	   LED7 = 1;
		 delayms(delay_time_led);
		 LED0 = 0;
		 delayms(delay_time_led);
	   LED1 = 0;
	   delayms(delay_time_led);
		 LED2 = 0;
	   delayms(delay_time_led);
		 LED3 = 0;
	   delayms(delay_time_led);
		 LED4 = 0;
	   delayms(delay_time_led);
		 LED5 = 0;
	   delayms(delay_time_led);
		 LED6 = 0;
	   delayms(delay_time_led);
		 LED7 = 0;}
	else{ //success
		 LED0 = 0;
	   LED1 = 0;
	   LED2 = 0;
	   LED3 = 0;
	   LED4 = 0;
		 LED5 = 0;
 	   LED6 = 0;
	   LED7 = 0;
		 delayms(delay_time_led);
		 LED0 = 1;
		 delayms(delay_time_led);
	   LED1 = 1;
	   delayms(delay_time_led);
		 LED2 = 1;
	   delayms(delay_time_led);
		 LED3 = 1;
	   delayms(delay_time_led);
		 LED4 = 1;
	   delayms(delay_time_led);
		 LED5 = 1;
	   delayms(delay_time_led);
		 LED6 = 1;
	   delayms(delay_time_led);
		 LED7 = 1;}
		
}

void clean_stage(){
  game_time = 0;
	delay_time = reflect_time / multiplier * delay_table[stage_number];
	reflect_time_true = delay_time;
	cnt_trigger = reflect_time;
}

main()
{
	
	bit buzzer = 0;
	u8 cnt_16ms = CHECKKEYTIME;
	u16 cnt_sleep = NOPTOSLEEP;
	u16 Touchkey = 0,oldtkey=0xf; //only for low 12 bit
	u16 checkkey = 0 ;
	cnt_trigger = reflect_time_true;
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	//P2n_open_drain(1<<4);
	//P2n_open_drain(1<<5);
	P3n_pure_input(1<<0); 

	InitialTime0_1ms();
	IntialLowPowerWakeup();

	InitialT1CKO();// initial the driver the Buzzer

	//InitialMPR();//no power now
	InitialSpeaker();
	SpeakerONOFF(0);
	
	EA=1;

	delayms(500);

	PowerVcc(1);			 	
	BuzzerONOFF(1);
	delayms(1000);
	BuzzerONOFF(0);

	InitialMPR();

  game_start();
	while(1){
		
		if(Timer0_1ms){
	   		Timer0_1ms=0;
			cnt_sleep--;
			cnt_16ms--;
			cnt_trigger--;
			game_time ++;
		}
		if(cnt_sleep ==0){	  // time to go to sleep
			cnt_sleep=NOPTOSLEEP;
			PowerVcc(0);	
			GoSleep();		
			cnt_16ms=30;	 // wakeup by int3,continue to run
			if (in_game = 0){
				game_start();}
			WakeupInitial();
		}
		if(cnt_trigger ==0){
		  cnt_trigger=reflect_time_true;
		}
		if(cnt_16ms ==0){
			cnt_16ms=CHECKKEYTIME;
			Touchkey = ReadMPR();
			keyvalue = Touchkey;
			UtilityGPIO();
	//		P00 = ~P00;
	//		P24 = ~P24;
		}
		
			if(Touchkey>0) {	//detect key touched
				//BuzzerONOFF(1);
				cnt_sleep=NOPTOSLEEP;	
				if( oldtkey != Touchkey){
					checkkey = Touchkey & oldtkey;
					Touchkey = Touchkey & (~checkkey);//效果就是同时按住的key来回激荡，3bit形成1bit:2bit激荡即1key:default激荡
					put_LED_on(Touchkey);
					TouchkeytoSound(Touchkey);
					oldtkey = Touchkey;
					//processing score
					if (in_game){
						if (reflect_time_true >= cnt_trigger){
					    score_player += multiplier * (reflect_time_true-cnt_trigger)* 2 * (stage_number + 1) + score_init;
						  cnt_trigger = reflect_time_true;}
						else{
							tries_player -= 1;
							score_player -= score_init;
							TouchkeytoSound(128);  //med 5
							cnt_trigger = reflect_time_true;}
					  if (tries_player == 0){
					   	flag = 0;
				  		game_over();
				  	  in_game = 0;}
   					}
					  if (game_time == game_duration){
							if (score_player > opt_stage[stage_number]){
								if (stage_number >= stage_NUM){
									flag = 1;
									game_over();
									in_game = 0;}
							else{
								TouchkeytoSound(1);
								delayms(50);
								TouchkeytoSound(128);
								delayms(50);
								TouchkeytoSound(32);
								delayms(50);
								TouchkeytoSound(128);
								stage_number ++;
								clean_stage();
						}
					}
				}
				}
//			AdjustTimer2(Touchkey+7);

			//SpeakerONOFF(1);
			BuzzerONOFF(soundon);
				
		}else{
			
			//SpeakerONOFF(0);
			BuzzerONOFF(0);
			InitialT1CKO();
		}

	
	}
  
}


void InitialTime0_1ms()		//1毫秒@5.5296MHz
{
	
	CLK_DIV =0xC0;	//P5.4 sysclk/4 

	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xEA;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;		//allow interrupt
}


void InitialT1CKO() //initial driver the Buzzer
{
	
	//151微秒6.6KHz @22.1184MHz
	TR1 = 0;		//定时器
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xBD;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
		
	//INT_CLKO |= 0x02; //b1 - T1CLKO   : 允许 T1 溢出脉冲在P3.4脚输出，Fck1 = 1/2 T1 溢出率

}

void IntialLowPowerWakeup()//initial INT3 wakeup MCU
{
	
	//P4n_push_pull(1<<0);
	//P4n_standard(1<<0);

	/*INT_CLKO
    7   6    5    4   3     2        1       0         Reset Value
    -  EX4  EX3  EX2  -   T2CLKO   T1CLKO  T0CLKO      0000,0000B8*/

	INT_CLKO |= 0x20; //EX3=1 	
	
}

void PowerVcc(bit on)// open Vcc for phy
{
	
	POWERVCC = ~on;	
}

void delayms(u16 ms)
{
	while(ms){
		if(Timer0_1ms){
			Timer0_1ms=0;
			ms--;
		}
		
	}
}

void GoSleep(void)//Sleep mode
{
	P0 =  0xf0;
	P2 =  0x0f;
	PCON = 0x02;// MCU goto Sleep
	_nop_();	// 掉电模式被唤醒后，首先执行此语句在进入中断服务程序。
	_nop_();
}
void WakeupInitial() //
{
	PowerVcc(1);
	delayms(50);		 	
	InitialMPR();
	delayms(100);
	BuzzerONOFF(1);
	delayms(1000);
	BuzzerONOFF(0);	
}

#define		PWM_DeadZone	1	/* 死区时钟数, 6 ~ 24之间  */


/*************	功能说明	**************

演示使用2路PWM产生互补或同相的SPWM.

主时钟选择24MHZ, PWM时钟选择1T, PWM周期2400, 死区12个时钟(0.5us).正弦波表用200点. 

输出正弦波频率 = 24000000 / 2400 / 200 = 50 HZ.

本程序仅仅是一个SPWM的演示程序, 用户可以通过上面的计算方法修改PWM周期和正弦波的点数和幅度.

本程序输出频率固定, 如果需要变频, 请用户自己设计变频方案.

本程序从P2.1(PWM3)输出正相脉冲, 从P2.2(PWM4)输出反相脉冲(互补).

如果需要P2.2输出同相的, 请在初始化配置中"PWMCFG"项选择设置1(设置PWM输出端口的初始电平, 0或1).


******************************************/



//void changTsin(void)
//{
//	u8 i;
//	for(i=0;i<40;i++){
//		T_sin[i] = (T_SinTable[i*5]-40)*40/2400;
//	}
//}

#define PwmClk_Timer2  0x10

void	PWM_config(void)
{
	u8	xdata	*px;

	EAXSFR();			// 访问XFR

	px = PWM3T1H;		// 指针指向PWM3
	*px = 0;			// 第一个翻转计数高字节
	px++;
	*px = 1;			// 第一个翻转计数低字节
	px++;
	*px = 0;	// 第二个翻转计数高字节
	px++;
	*px = 20;	// 第二个翻转计数低字节
	px++;
	*px = 0;				// PWM3输出选择P2.1, 无中断
	PWMCR  |=  0x02;	// 相应PWM通道的端口为PWM输出口，受PWM波形发生器控制
	PWMCFG &= ~0x02;	// 设置PWM输出端口的初始电平为0
//	PWMCFG |=  0x02;	// 设置PWM输出端口的初始电平为1
	P21 = 1;
	P2n_push_pull(1<<1);	//IO初始化, 上电时为高阻

	px = PWM4T1H;		// 指针指向PWM4
	*px = 0;			// 第一个翻转计数高字节
	px++;
	*px = 0;			// 第一个翻转计数低字节
	px++;
	*px = 0;	// 第二个翻转计数高字节
	px++;
	*px = 19;	// 第二个翻转计数低字节
	px++;
	*px = 0;			// PWM4输出选择P2.2, 无中断
	PWMCR  |=  0x04;	// 相应PWM通道的端口为PWM输出口，受PWM波形发生器控制
//	PWMCFG &= ~0x04;	// 设置PWM输出端口的初始电平为0
	PWMCFG |=  0x04;	// 设置PWM输出端口的初始电平为1
	P22 = 1;
	P2n_push_pull(1<<2);	//IO初始化, 上电时为高阻

// modify by lq

	px  = PWMCH;		// PWM计数器的高字节
//	*px = 2400 / 256;
	*px = 0;
	px++;
	//*px = 2400 % 256;	// PWM计数器的低字节
	*px = 40;	// PWM计数器的低字节
	px++;				// PWMCKS, PWM时钟选择
	*px = PwmClk_Timer2;	// 时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_Timer2

	EAXRAM();			// 恢复访问XRAM
	
	PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
//	PWMCR &= ~ECBI;		// 禁止PWM计数器归零中断
	PWMCR |=  ECBI;		// 允许PWM计数器归零中断

//	PWMFDCR = ENFD | FLTFLIO | FDIO;	//PWM失效中断控制,  ENFD | FLTFLIO | EFDI | FDCMP | FDIO
}

/********************* PWM中断函数************************/

void PWM_int (void) interrupt PWM_VECTOR
{
	u8	xdata	*px;
	u8	j;
//	u8 sinL,sinH;
	u8	SW2_tmp;

	if(PWMIF & CBIF)	//PWM计数器归零中断标志
	{
		PWMIF &= ~CBIF;	//清除中断标志

		SW2_tmp = P_SW2;	//保存SW2设置
		EAXSFR();		//访问XFR
		px = PWM3T2H;	// 指向PWM3
		
		//modify by lq
		j = T_sin[PWM_Index];
		//*px = (u8)(j >> 8);	//第二个翻转计数高字节
		*px = 0;
		px++;
		*px = j;			//第二个翻转计数低字节

		j += PWM_DeadZone;		//死区

		px = PWM4T2H;			// 指向PWM4
		*px = 0;//(u8)(j >> 8);	//第二个翻转计数高字节
		px++;
		*px = j;			//第二个翻转计数低字节
		P_SW2 = SW2_tmp;	//恢复SW2设置

	//	if(++PWM_Index >= 200)	PWM_Index = 0;
	//modify by lq
		PWM_Index++;
		if( PWM_Index>= 40)	PWM_Index = 0;

	}
}

/**********************************************/

// Initialize timer2 for PWM 
  void Timer2Initial()		//6微秒@22.1184MHz =166122Hz
{
	AUXR |= 0x04;		//定时器时钟1T模式
//	T2L = 0x09;		//设置定时初值	//6微秒@22.1184MHz =166122Hz
//	T2H = 0xfe;		//设置定时初值

	T2L = 0x13;		//设置定时初值	//45微秒@22.1184MHz = 22000Hz
	T2H = 0xFC;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时

}

//void AdjustTimer2(u8 keyvalue)

//{
//	AUXR &= ~0x10;
////	T2L = T_Freq_L[keyvalue];
////	T2H = T_Freq_H[keyvalue];		//设置定时初值
//	T2L = T_Freq[keyvalue]%256;
//	T2H = T_Freq[keyvalue]/256;		//设置定时初值

//	AUXR |= 0x10;	
//	
//}

void AdjustTimer1(u8 keyvalue)

{
	TR1 = 0;
	TL1 = T1_Freq[keyvalue]%256;
	TH1 = T1_Freq[keyvalue]/256;		//设置定时初值
	TR1 = 1;	
	
}

void InitialSpeaker(void)//	 for SPWM @P2.1 P2.2
{
	
	Timer2Initial();
	PWM_config();	//初始化PWM
//	changTsin();//new sin table
	
		
}

void SpeakerONOFF(bit on)
{
	if(on){
		PWMCR |= ENPWM;
	}else{
		PWMCR &= ~ENPWM;
	}
}
//////////////////////////////////////////////////

void BuzzerONOFF(bit on)
{
	if(on){
		/*INT_CLKO
    7   6    5    4   3     2        1       0         Reset Value
    -  EX4  EX3  EX2  -   T2CLKO   T1CLKO  T0CLKO      0000,0000B8*/

		INT_CLKO |= 0x02; //b1 - T1CLKO   : 允许 T1 溢出脉冲在P3.4脚输出，Fck1 = 1/2 T1 溢出率	
	}else{

		INT_CLKO &= ~0x02; //b1 - T1CLKO   : 允许 T1 溢出脉冲在P3.4脚输出，Fck1 = 1/2 T1 溢出率	
	}

}

// interrupt service program
void timer0 (void) interrupt TIMER0_VECTOR
{
	//DisplayScan();	//1ms扫描显示一位
	Timer0_1ms = 1;		//1ms标志
}

void exint3() interrupt 11
{
	INT_CLKO &= ~0x20; //clear flag
	INT_CLKO |= 0x20; //EX3=1
}

// mpr121 reg set value table
/*
T/R Threshold: 0x41 ~0x5a tt0,tr0,tt1,tr1,.... set value 0x4-0x10 = 0x8
Debounce reg: 0x5b set value DT[0-2],DR[4-6] . set value 11
F/G CDC config reg:0x5c use default value 0x10
F/G CDT config reg:0x5d use default value 0x24
ECR is maincontrol reg :0x5e .set valuse CL[6-7]=10.EROX_EN[4-5]=00.ELE_EN[0-3]=0011 (use 0-2 Electrode)
autoconfig reg: 0x7b. FFI[6-7]=00 same as 0x5c.RETRY[4-5]=10 retry 4times.BVA[2-3]=10 sameas CL.ARE[1]=1 enable reconfig.ACE[0]=1 enabel auto-config
autoreconfig reg:0x7c.SCTS[7]=0 not skip CTSearch.OOR-ARF-ACF-IE =0 for no interrupt
USL reg:0x7d.	(VDD-0.7)/VDD*256 = 201@3.3V
LSL reg:0x7e.	USL*0.65=131@3.3V
TSL reg:0x7f.	USL*0.9=181@3.3V
Softreset reg:0x80 write 0x63


read reg:
OOR:0x03.ACFF[7] ARFF[6]=1 if autoconfig autoreconfig failured
T status reg:0x00 ELE[7-0] 1 for touched .0 for released;0x01 OVCF[7] 1for over current,0 for normal


*/

// initial MPR121
void InitialMPR()
{
	u8 i;
	P1n_open_drain(1<<6);
	P1n_open_drain(1<<7);
	P3n_open_drain(1<<6);

	Single_WriteI2C(0x80,0x63);
	for(i=0;i<250;i--)_nop_();
	Single_WriteI2C(0x80,0x63);
	for(i=0;i<250;i--)_nop_();

//Single_WriteI2C(u8 REG_Address,u8 REG_data)
	for(i=0;i<24;i++)
		Single_WriteI2C(0x41+i,0x4);
	Single_WriteI2C(0x5b,0x33);
	Single_WriteI2C(0x7b,0x2b);
	Single_WriteI2C(0x7c,0x07);
	Single_WriteI2C(0x7d,201);
	Single_WriteI2C(0x7e,131);
	Single_WriteI2C(0x7f,181);
	Single_WriteI2C(0x5e,0x8C);
}

u16 ReadMPR()
{
  u16 ral=0;
  u8 tmp1,tmp;
  tmp1  = Single_ReadI2C(0x01);
  tmp = Single_ReadI2C(0x00);
  ral = (tmp1&0x0f)*256+tmp;

  return ral;
}

void TouchkeytoSound(u16 Touchkey)
{
	LED_disblack();
	switch(Touchkey){
		case 1:
			LED0 = 1;
			break;
		case 2:
			AdjustTimer1(1);//Low 6
			break;
		case 4:
			AdjustTimer1(2);//Low 7
			break;
		case 8:
			AdjustTimer1(3);//med 1
			break;
		case 16:
			AdjustTimer1(4);//med 2
			break;
		case 32:
			AdjustTimer1(5);//med 3
			break;
		case 64:
			AdjustTimer1(6);//med 4
			break;
		case 128:
			AdjustTimer1(7);//med 5
			break;
		case 256:
			AdjustTimer1(8);//med 6
			break;
		case 512:
			AdjustTimer1(9);//med 7
			break;
		case 1024:
			AdjustTimer1(10);//high 1
			break;
		case 2048:
			AdjustTimer1(11);//high 2
			break;
		//case 0:
			//soundon = 0;
			//break;
		default:
			InitialT1CKO();//AdjustTimer1(2);//Low 3
			break;
			

	}
		
}

u8 ledrunmode(u8 ledflash,u8 ledmode)
{
	switch(ledmode){
		case 0:
			ledflash = ledflash << 1;
			if(ledflash ==0)ledflash=0x1;
			break;
		case 1:
			ledflash = ledflash >> 1;
			if(ledflash == 0)ledflash=0x80;
			break;
		case 2://42拍
			if(ledflash > 0x0f){
				ledflash = 0x0f;
			}else{
				ledflash = 0xf0;
			}
			break;
		case 3://43拍
			if(ledflash > 0x0f){
				ledflash = 0x0c;
			}else{
				if(ledflash == 0x0c){
					ledflash = 0x03;
				}else{
					ledflash = 0x30;
				}
			}
			break;
			
		default:
			ledflash=0;
			break;
	}
	return ledflash;
}


void UtilityGPIO()
{
	static u8 ledtimecnt1=LEDTIME1;//900ms
	//static u8 P0_03= 1,P2_47=1;
	static u8 ledset=0;
	static bit p43pai=0,oldsoundon;
	static u8 ledmode=0;
	u8 chgledset;
	
	utility =  P0 & 0xf0;
	
	if(utility & LED43PAI){
		p43pai =1;
	}else{
		p43pai =0;
	}
	
	if(utility & SOUNDON ){
		soundon =1;
	}else{
		soundon =0;
		if(oldsoundon){
			ledset = 0x1;
			ledmode= 0;
		}
	}
	oldsoundon = soundon; 
	
	if(utility & LEDON){
		if(ledtimecnt1-- == 0){
			ledtimecnt1 = LEDTIME1;
			if(soundon){
				ledmode=p43pai?3:2;
			}else{
				//if(ledset==0)ledmode = (ledmode>0)?0:1;
				ledmode = p43pai;
			}
			ledset = ledrunmode(ledset,ledmode);
		}		
	}
	
	if(utility & LEDKEYON){
		if(keyvalue < 0xff){
			ledset = keyvalue &0xff;
		}else{
			ledset = keyvalue >> 8;
			ledset+= 0x80;
		}
	}
	if((0==(utility & LEDON))&&(0==(utility & LEDKEYON))){
		ledset=0;
	}
	
	//P0 = ((~ledset) | 0xf0);
	//P2 = (~ledset) | 0x0f;
	chgledset =(ledset>>4);
	chgledset += (ledset&0x0f)<<4;
	//chgledset += (ledset>>4);
	P0 = (~chgledset) | 0xf0;
	P2 = (~chgledset) | 0x0f;
	
}

void put_LED_on(u16 label){
	LED_disblack();
  switch(label){
		case 1:
			LED0 = 1;//Low 5
			break;
		case 2:
			LED1 = 1;//Low 6
			break;
		case 4:
			LED2 = 1;//Low 7
			break;
		case 8:
			LED3 = 1;//med 1
			break;
		case 16:
			LED4 = 1;//med 2
			break;
		case 32:
			LED5 = 1;//med 3
			break;
		case 64:
			LED6 = 1;//med 4
			break;
		case 128:
			LED7 = 1;//med 5
			break;
		default:
			break;
	}
}

void LED_disblack(){
  LED0 = 0;
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;
	LED5 = 0;
	LED6 = 0;
	LED7 = 0;
}

void LED_all_on(){
	LED0 = 1;
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
	LED5 = 1;
	LED6 = 1;
	LED7 = 1;
}