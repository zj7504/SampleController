/*******************************************************************************
** 文件名: 		mian.c
** 版本：  		1.0
** 工作环境: 	Keil uVision 5.11
** 作者: 		zj
** 生成日期: 	2016-12
** 功能:		USART初始化和RCC设置，然后从common.c中执行主菜单
** 相关文件:	stm32f10x.h
** 修改日志：	2016-12 创建文档
*******************************************************************************/
/* 包含头文件 *****************************************************************/
#include "common.h"
#include "eeprom.h"
/* 类型声明 ------------------------------------------------------------------*/
/* 宏 ------------------------------------------------------------------------*/
#define LED_boot    GPIO_Pin_1

/* 变量 ----------------------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
uint16_t update_flag;

/* 函数声明 ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void LED_Configuration(void);
void MOS_Protect(void);
static void IAP_Init(void);
void KEY_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
/* 函数功能 ------------------------------------------------------------------*/

/*******************************************************************************
  * @函数名称	main
  * @函数说明   主函数
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
int main(void)
{
    //初始化MOS控制引脚为低电平，以防止上电高电平损坏MOS管
    MOS_Protect();
    /* Unlock the Flash Program Erase controller */
    FLASH_Unlock();
    /* EEPROM Init */
    EE_Init();
    FLASH_Lock();
    EE_ReadVariable(VirtAddVarTab[App_update_Add], &update_flag);
    LED_Configuration();
    //配置按键
    KEY_Configuration() ;
    IAP_Init();
    //按键是否按下
    if ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)  == 0x00)||(update_flag == 0x01))
    {
        
        //假如按键按下
        //执行IAP驱动程序更新Flash程序

        SerialPutString("\r\n======================================================================");
        SerialPutString("\r\n=              (C) COPYRIGHT 2016                                    =");
        SerialPutString("\r\n=                                                                    =");
        SerialPutString("\r\n=     In-Application Programming Application  (Version 1.0.0)        =");
        SerialPutString("\r\n=                                                                    =");
        SerialPutString("\r\n=                                   By zj                            =");
        SerialPutString("\r\n======================================================================");
        SerialPutString("\r\n\r\n");
        Main_Menu ();
        
        update_flag = 0x00;
        //write in flash
        FLASH_Unlock();
        EE_WriteVariable(VirtAddVarTab[App_update_Add],update_flag);
        FLASH_Lock();
    }
    //否则执行用户程序
    else
    {
        //判断用户是否已经下载程序，因为正常情况下此地址是栈地址。
        //若没有这一句的话，即使没有下载程序也会进入而导致跑飞。
        if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
        {
            SerialPutString("Execute user Program\r\n\n");
            GPIO_SetBits(GPIOB,LED_boot);  //熄灭LED_boot
            //跳转至用户代码
            JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
            Jump_To_Application = (pFunction) JumpAddress;

            //初始化用户程序的堆栈指针
            __set_MSP(*(__IO uint32_t*) ApplicationAddress);
            Jump_To_Application();
        }
        else
        {
            SerialPutString("no user Program\r\n\n");
        }
    }

    while (1)
    {
    }
}


/*******************************************************************************
  * @函数名称	LED_Configuration
  * @函数说明   配置使用LED
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void LED_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //使能LED所在GPIO的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    //初始化LED的GPIO
    GPIO_InitStructure.GPIO_Pin = LED_boot;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOB,LED_boot);  //熄灭LED_boot
    GPIO_ResetBits(GPIOB,LED_boot);  //点亮LED_boot
    
}

/*******************************************************************************
  * @函数名称	KEY_Configuration
  * @函数说明   按键初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void KEY_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //配置按键
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//初始化MOS控制引脚为低电平，以防止上电高电平损坏MOS管
void MOS_Protect(void)
{
        /*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED的外设时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        
		//初始化GPIO之前设置好BBR寄存器值，防止上电过程输出高电平
		GPIO_ResetBits(GPIOC, GPIO_Pin_7|GPIO_Pin_9);
		GPIO_SetBits(GPIOC, GPIO_Pin_6|GPIO_Pin_8);
    
		/*调用库函数，初始化GPIO*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);	
    //GPIO_ResetBits(GPIOC, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
}

/*******************************************************************************
  * @函数名称	GPIO_Configuration
  * @函数说明   配置使用USART1的相关IO管脚
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    // 配置 USART1 Tx (PA.09) 作为功能引脚并上拉输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //配置 USART1 Rx (PA.10) 作为功能引脚并是浮空输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
  * @函数名称	IAP_Init
  * @函数说明   配置使用IAP
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void IAP_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* USART1 配置 ------------------------------------------------------------
         USART1 配置如下:
          - 波特率      = 115200 baud
          - 字长        = 8 Bits
          - 一个停止位
          - 无校验
          - 无硬件流控制
          - 接受和发送使能
    --------------------------------------------------------------------------*/
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    GPIO_Configuration();
    USART_Init(USART1, &USART_InitStructure);
    // 使能 USART1
    USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
  * @函数名称	Delay
  * @函数说明   插入一段延时时间
  * @输入参数   nCount: 指定延时时间长度
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT

/*******************************************************************************
  * @函数名称	assert_failed
  * @函数说明   报告在检查参数发生错误时的源文件名和错误行数
  * @输入参数   file: 源文件名
  				line: 错误所在行数
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* 用户可以增加自己的代码用于报告错误的文件名和所在行数,
       例如：printf("错误参数值: 文件名 %s 在 %d行\r\n", file, line) */

    //死循环
    while (1)
    {
    }
}
#endif

/*******************************文件结束***************************************/
