#ifndef __SAMPLE_APP_H__
#define	___SAMPLE_APP_H__

#include <stdio.h>
#include "stm32f10x.h"
/**********************************************************************
*
*       Defines
*
**********************************************************************
*/

typedef enum{false, true} bool;

#define PUMP_SAMPLE 0
#define BOTTLE_SAMPLE 1
#define BUOY_RELEASE 0
//#define USE_SERVO
#define USE_MOS

#if	PUMP_SAMPLE == 1
#define Send_Data_length	15
#elif	BOTTLE_SAMPLE == 1
#define Send_Data_length	5
#else
#define Send_Data_length	5
#endif


	
#define Sensor_On	1
#define Sensor_Off	0
	
	/*端口定义*/
#define Sensor_Port	GPIOC
#define Full_Pin	GPIO_Pin_0
#define	Touch_Pin	GPIO_Pin_1



/*********************************************************************
*
*       Data
*
**********************************************************************
*/


enum Winch_Status{
	Winch_Stop = 0,
	Winch_Down,
	Winch_Up,
	};

enum PWM_Channel{
	TIM2_CH1 = 1,
	TIM2_CH2,
	TIM2_CH3,
	TIM2_CH4,
};

extern	bool Touch_Flag;
extern	bool Full_Flag[2];
extern  volatile uint8_t Buoy_Flag[2];
extern	volatile uint8_t Pump_Sample_Flag[4];
extern bool Health_Status_Flag;

/*Parameter store in eeprom*/
extern uint16_t   Fixed_MAX_PWM;    //固定装置舵机PWM值
extern uint16_t   Fixed_MIN_PWM;
extern bool Fixed_Enable;
//卷扬机构绳索长度计算
extern uint16_t   Rope_Length;
extern uint8_t    Hole_num; //number of hole for whell
extern uint8_t    Perimeter; //perimeter of whell

extern enum Winch_Status winch_status;

extern bool Winch_Down_Check;
extern bool Winch_Up_Check;
extern uint8_t bero_timer; //延时开启接近开关检查计数

/***********************************************************************
*
*    Function
*
************************************************************************/
//fixture control
void Fixture_Release(void);
void Fixture_clamp(void);

#endif

