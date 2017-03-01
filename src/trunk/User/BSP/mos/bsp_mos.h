#ifndef __MOS_H
#define	__MOS_H

#include "stm32f10x.h"



//MOS控制

#define MOS
/* 直接操作寄存器的方法控制IO */
#define	IO_Hi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define IO_Lo(p,i)			{p->BRR=i;}				//输出低电平
#define IO_Toggle(p,i)		{p->ODR ^=i;}			//输出反转状态

#define MOS_Port	GPIOC
#define MOS_A_L		GPIO_Pin_6
#define MOS_A_H		GPIO_Pin_7
#define MOS_B_L		GPIO_Pin_8
#define MOS_B_H		GPIO_Pin_9

#ifdef IR2101S
//for IR2101S
#define MOS_A_L_ON		IO_Hi(MOS_Port,MOS_A_L)
#define MOS_A_L_OFF		IO_Lo(MOS_Port,MOS_A_L)
#define MOS_A_H_ON		IO_Hi(MOS_Port,MOS_A_H)
#define MOS_A_H_OFF		IO_Lo(MOS_Port,MOS_A_H)
#define MOS_B_L_ON		IO_Hi(MOS_Port,MOS_B_L)
#define MOS_B_L_OFF		IO_Lo(MOS_Port,MOS_B_L)
#define MOS_B_H_ON		IO_Hi(MOS_Port,MOS_B_H)
#define MOS_B_H_OFF		IO_Lo(MOS_Port,MOS_B_H)

#define MOS_ALL_OFF	{MOS_A_H_OFF; MOS_A_L_OFF; MOS_B_H_OFF; MOS_B_L_OFF;}
#define POSITIVE_SPIN	{MOS_A_H_ON; MOS_B_L_ON;}
#define NEGATIVE_SPIN	{MOS_B_H_ON; MOS_A_L_ON;}
#define STOP_SPIN	{MOS_ALL_OFF}

#define MOTOR_A_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_A_H_ON;}
#define MOTOR_A_OFF	{MOS_A_H_OFF}
#define MOTOR_B_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_B_H_ON;}
#define MOTOR_B_OFF {MOS_B_H_OFF}
#endif

#ifdef IR2103S
//for IR2103S
#define MOS_A_L_ON		IO_Lo(MOS_Port,MOS_A_L)
#define MOS_A_L_OFF		IO_Hi(MOS_Port,MOS_A_L)
#define MOS_A_H_ON		IO_Hi(MOS_Port,MOS_A_H)
#define MOS_A_H_OFF		IO_Lo(MOS_Port,MOS_A_H)
#define MOS_B_L_ON		IO_Lo(MOS_Port,MOS_B_L)
#define MOS_B_L_OFF		IO_Hi(MOS_Port,MOS_B_L)
#define MOS_B_H_ON		IO_Hi(MOS_Port,MOS_B_H)
#define MOS_B_H_OFF		IO_Lo(MOS_Port,MOS_B_H)

#define MOS_ALL_OFF	{MOS_A_H_OFF; MOS_A_L_OFF; MOS_B_H_OFF; MOS_B_L_OFF;}
#define POSITIVE_SPIN	{MOS_A_H_ON; MOS_B_L_ON;}
#define NEGATIVE_SPIN	{MOS_B_H_ON; MOS_A_L_ON;}
#define STOP_SPIN	{MOS_ALL_OFF}

#define MOTOR_A_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_A_H_ON;}
#define MOTOR_A_OFF	{MOS_A_H_OFF}
#define MOTOR_B_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_B_H_ON;}
#define MOTOR_B_OFF {MOS_B_H_OFF}
#endif

//direct mos
#ifdef MOS
#define MOS_A_L_ON		IO_Lo(MOS_Port,MOS_A_L)
#define MOS_A_L_OFF		IO_Hi(MOS_Port,MOS_A_L)
#define MOS_A_H_ON		IO_Hi(MOS_Port,MOS_A_H)
#define MOS_A_H_OFF		IO_Lo(MOS_Port,MOS_A_H)
#define MOS_B_L_ON		IO_Lo(MOS_Port,MOS_B_L)
#define MOS_B_L_OFF		IO_Hi(MOS_Port,MOS_B_L)
#define MOS_B_H_ON		IO_Hi(MOS_Port,MOS_B_H)
#define MOS_B_H_OFF		IO_Lo(MOS_Port,MOS_B_H)

#define MOS_ALL_OFF	{MOS_A_H_OFF; MOS_A_L_OFF; MOS_B_H_OFF; MOS_B_L_OFF;}
#define POSITIVE_SPIN	{MOS_A_H_ON; MOS_B_L_ON;}
#define NEGATIVE_SPIN	{MOS_B_H_ON; MOS_A_L_ON;}
#define STOP_SPIN	{MOS_ALL_OFF}

#define MOTOR_A_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_A_H_ON;}
#define MOTOR_A_OFF	{MOS_A_H_OFF}
#define MOTOR_B_ON	{MOS_A_L_OFF; MOS_B_L_OFF; MOS_B_H_ON;}
#define MOTOR_B_OFF {MOS_B_H_OFF}
#endif

void MOS_GPIO_Config(void);


#endif /* __MOS_H */
