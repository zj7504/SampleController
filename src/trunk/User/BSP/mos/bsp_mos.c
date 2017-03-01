/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-MINI STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_mos.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void MOS_GPIO_Config(void)
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED的外设时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = MOS_A_L|MOS_A_H|MOS_B_L|MOS_B_H;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        
    //初始化GPIO之前设置好BBR寄存器值，防止上电过程输出高电平
    GPIO_ResetBits(MOS_Port, MOS_A_H|MOS_B_H);
		GPIO_SetBits(MOS_Port, MOS_A_L|MOS_B_L);
    
		/*调用库函数，初始化GPIO*/
		GPIO_Init(MOS_Port, &GPIO_InitStructure);			  

		/* 关闭所有MOS管	*/
		MOS_ALL_OFF;
}



/*********************************************END OF FILE**********************/
