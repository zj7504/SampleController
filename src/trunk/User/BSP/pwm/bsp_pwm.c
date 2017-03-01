/**
  ******************************************************************************
  * @file    bsp_pwm.c
  * @author  zj
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   pwm?????
  ******************************************************************************
  */

#include "bsp_pwm.h"


/**
  * @brief  Config Timer NVIC
  */
	
static void NVIC_Config_TIM2(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
    /* TIM2_IRQ */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void NVIC_Config_TIM3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* TIM3_IRQ */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
*		Config TIM2
*/
void TIM2_PWM_Init(u16 arr, u16 psc)
{
		GPIO_InitTypeDef GPIO_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);  //TIM2-FullRemap(CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)
		GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
		//Config GPIO
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		TIM_TimeBaseStructure.TIM_Period = arr; //计数值(example:定时器周期，就是20us记一次，计数50000次，就是1s) 
		TIM_TimeBaseStructure.TIM_Prescaler = psc; //预分频数，example:72M/(Prescaler+1) = 50K,也就是20us  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //这个TIM_ClockDivision是设置时钟分割，这里不分割还是1MHZ的计数频率
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //counter up
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
    TIM_OCInitStructure.TIM_Pulse = 0; //设置跳变值，当计数器计数到这个值时，电平发生跳变
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //当定时器计数值小于CCR值时为高电平
		
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);	
  	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);	
  	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);	
  	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
		TIM_OC4Init(TIM2, &TIM_OCInitStructure);	
  	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
		
		TIM_ARRPreloadConfig(TIM2, ENABLE); //使能TIM3重载寄存器ARR
		TIM_Cmd(TIM2, ENABLE);  //??TIM2  
}

/**
 * @brief  Config TIM3
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT ?????
 *                    TIMx_CCR(???????仯)
 * ?????(TIMx_ARR +1 ) * ????
 * ????TIMx_CCR/(TIMx_ARR +1)
 */

//PWM????????  
//arr????????  
//psc?????????  
  
void TIM3_PWM_Init(u16 arr,u16 psc)  
{    
    GPIO_InitTypeDef GPIO_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);// ????????TIM3  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);  //??GPIOA??????  
																																								
		//breath LED GPIO
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
      
    TIM_TimeBaseStructure.TIM_Period = arr; //?????????????????????????????????    50HZ  
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //???????TIMx??????????????  ?????  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //????????????????(???ò???)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //counter up
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //????IM_TimeBaseInitStruct?????????????TIMx????????λ  
     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //??????  
    TIM_OCInitStructure.TIM_Pulse = 0; //?????????????????????  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //??????:TIM?????????? 

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);	
  	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE); //??TIMx?ARR??????????  
    TIM_CtrlPWMOutputs(TIM3,ENABLE);    //MOE ??????  
    TIM_Cmd(TIM3, ENABLE);  //??TIM3  
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    NVIC_Config_TIM3();
}  

void Set_PWM_Value(TIM_TypeDef* TIMx,uint8_t Channel, uint16_t val)
{
    if(Channel > 4)
    {
        Channel = 4;
    }
    switch(Channel)
    {
        case 1:
            TIMx->CCR1 = val;
            break;
        case 2:
            TIMx->CCR2 = val;
            break;
        case 3:
            TIMx->CCR3 = val;
            break;
        case 4:
            TIMx->CCR4 = val;
            break;
        default:
            break;
    }
}

/* 呼吸灯中断服务函数 */
void TIM3_IRQHandler(void)
{
    static uint8_t pwm_index = 0;			//用于PWM查表
    static uint8_t period_cnt = 0;			//用于计算周期数
    /* LED亮度等级 PWM表 */
    uint16_t indexWave[] = {9922, 9922, 9922, 9883, 9844, 9805, 9766, 9727, 9648, 9570, 9453, 9336, 9180, 8945, 8711, 8398, 8008, 7500, 6875, 6133, 5195,
                           3984, 2539, 1953, 703, 1, 1, 1,1, 1, 1, 1, 703, 1953, 2539, 3984, 5195, 6133, 6875, 7500, 8008, 8398, 8711, 8945, 9180, 9336,
                           9453, 9570, 9648, 9727, 9766, 9805, 9844, 9883, 9922, 9922, 9922
                          };
    /*   uint8_t indexWave[] = {250, 249, 247, 245, 242, 239, 235, 229, 223, 215, 205, 192, 176, 157, 133,
                           102, 65, 18, 1, 1,1,1,1,1, 1, 50, 91, 133, 107, 170, 187, 201, 212, 220, 227, 233, 237, 241,
                           244, 246, 248, 250
                          };*/
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//TIM_IT_Update
    {
        period_cnt++;
        if(period_cnt >= 2)							//若输出的周期数大于10，输出下一种脉冲宽的PWM波
        {

            TIM3->CCR3 = indexWave[pwm_index];			//根据PWM表修改定时器的比较寄存器值
            pwm_index++;								//标志PWM表的下一个元素

            if( pwm_index >=  57)						//若PWM脉冲表已经输出完成一遍，重置PWM查表标志
            {
                pwm_index = 0;
            }

            period_cnt = 0;								//重置周期计数标志
        }
        TIM_ClearITPendingBit (TIM3, TIM_IT_Update);	//必须要清除中断标志位
    }
}

/*********************************************END OF FILE**********************/
