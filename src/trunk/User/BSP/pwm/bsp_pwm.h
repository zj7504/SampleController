#ifndef __PWM_H
#define	__PWM_H

#include "stm32f10x.h"

void TIM2_PWM_Init(u16 arr, u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void Set_PWM_Value(TIM_TypeDef* TIMx,uint8_t Channel, uint16_t val);


#endif /* __PWM_H */
