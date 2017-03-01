/**
  ******************************************************************************
  * @file    bsp_bero.c
  * @author  zj
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   接近开关检测程序
  ******************************************************************************
  */

#include "bsp_bero.h"


uint8_t BERO1_Count = 0;
uint8_t BERO2_Count = 0;
uint8_t Infrared_Count = 0;

static void NVIC_Bero1_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /*如果用的 IO 是I05~IO9 中的一个的话，对应的中断向量只能是 EXTI9_5_IRQn*/
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void NVIC_Bero2_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /*如果用的 IO 口是 IO0~IO4，那么对应的中断向量是 EXTI0_IRQn ~ EXTI4_IRQn*/
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI_Bero1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* config the extiline clock and AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* config the NVIC */
    NVIC_Bero1_Config();

    /* EXTI line gpio config*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* EXTI line mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI_Bero2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* config the extiline clock and AFIO clock */
    //PB4---Remap(IO)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    /* config the NVIC */
    NVIC_Bero2_Config();

    /* EXTI line gpio config*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* EXTI line mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

//红外对管配置

void EXTI_Infrared_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    /* config the extiline clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* config the NVIC */
    //EXTI15_10_IRQn--pin10~15
    NVIC_Bero1_Config();

    /* EXTI line gpio config*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //下拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* EXTI line mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //rising trigger
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void BERO_Check_Config(void)
{
    EXTI_Bero1_Config();
    EXTI_Bero2_Config();
    EXTI_Infrared_Config();
}


//中断屏蔽设置
//value: 1-开启  0-关闭
void BERO1_Set_Int(uint8_t value)
{
    EXTI->PR = 1 << 12;
    if(value)
    {
        EXTI->IMR |= 1 << 12; //不屏蔽line12上的中断
    }
    else
    {
        EXTI->IMR &= ~(1 << 12); //屏蔽line12上的中断
    }

}

void BERO2_Set_Int(uint8_t value)
{
    EXTI->PR = 1 << 4;
    if(value)
    {
        EXTI->IMR |= 1 << 4; //不屏蔽line12上的中断
    }
    else
    {
        EXTI->IMR &= ~(1 << 4); //屏蔽line12上的中断
    }

}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)  //确保是否产生了EXTI_Line中断
    {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0)
				{
						BERO1_Count++;
				}
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1)
        {
            Infrared_Count++;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);     //清除中断标志位
    }
}

void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        BERO2_Count++;
        EXTI_ClearITPendingBit(EXTI_Line4);	//清除中断标志位
    }
}


/*********************************************END OF FILE**********************/
