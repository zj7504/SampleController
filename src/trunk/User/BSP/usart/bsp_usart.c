/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   重现c库printf函数到usart端口
  *****************************************************************************
  *
  *串口通信配置
  *
  ******************************************************************************
  */

#include "bsp_usart.h"

//数据类型定义
volatile uint8_t	usart1_p = 0;
volatile uint8_t	Usart1_Send_Num = 0;
volatile uint8_t	Usart1_Received = 0;
uint8_t Usart1_Tx_buff[USART_BUFF_SIZE];
uint8_t	Usart1_Rx_buff[USART_BUFF_SIZE];
//uint8_t Decode_buff[USART_BUFF_SIZE];

volatile uint32_t	usart2_p = 0;
volatile uint8_t	Usart2_Send_Num = 0;
volatile uint8_t	Usart2_Received = 0;
uint8_t		Usart2_DJI_buff[USART2_BUFF_SIZE];
uint8_t		Usart2_Rx_buff[USART2_BUFF_SIZE];

volatile uint8_t	usart3_p = 0;
volatile uint8_t	Usart3_Send_Num = 0;
volatile uint8_t	Usart3_Received = 0;
uint8_t Usart3_Tx_buff[USART_BUFF_SIZE];
uint8_t	Usart3_Rx_buff[USART_BUFF_SIZE];


/*****************************************************/
/*--------------------USART1-------------------------*/
/*****************************************************/

/**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;

		/* config USART1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

		/* USART1 GPIO config */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* USART1 mode config */
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1, &USART_InitStructure);


		//中断配置
    	USART_ITConfig(USART1,USART_IT_TC,DISABLE);
    	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
    	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);

    	 //采用DMA方式发送
	    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	    //采用DMA方式接收
	    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
		USART_Cmd(USART1, ENABLE);
}


/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART1_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;

		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//NVIC_Config();	   			//配置DMA中断


        //DMA1通道4配置
        DMA_DeInit(DMA1_Channel4);
		//设置DMA源：串口数据寄存器地址
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);

		//内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart1_Tx_buff;
		//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_DJI_buff;
		//方向：从内存到外设
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		//传输大小DMA_BufferSize=SENDBUFF_SIZE
		DMA_InitStructure.DMA_BufferSize = 0;

		//外设地址不增
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

		//内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

		//外设数据单位//
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		//内存数据单位 8bit
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

		//DMA模式：不断循环
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

		//优先级：中
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

		//禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		//配置DMA1的4通道
		DMA_Init(DMA1_Channel4, &DMA_InitStructure);

		//配置DMA发送完成后产生中断
		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);

		//使能DMA
		DMA_Cmd (DMA1_Channel4,ENABLE);


        //串口收DMA配置
	    //DMA1通道5配置
	    DMA_DeInit(DMA1_Channel5);
	    //外设地址
	    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
	    //内存地址
	    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart1_Rx_buff;
	    //dma传输方向单向
	    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	    //设置DMA在传输时缓冲区的长度
	    DMA_InitStructure.DMA_BufferSize = USART_BUFF_SIZE;
	    //设置DMA的外设递增模式，一个外设
	    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    //设置DMA的内存递增模式
	    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	    //外设数据字长
	    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	    //内存数据字长
	    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	    //设置DMA的传输模式
	    //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	    //设置DMA的优先级别
	    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	    //设置DMA的2个memory中的变量互相访问
	    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	    DMA_Init(DMA1_Channel5,&DMA_InitStructure);

        //DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);

	    //使能通道5
	    DMA_Cmd(DMA1_Channel5,ENABLE);
}

void USART1_NVIC_Config(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    //DMA 发送中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


	//Enable the USARTy Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART1_Init(void)
{
    USART1_Config();
    USART1_DMA_Config();
    USART1_NVIC_Config();
}

///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);

		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

		return (ch);
}

///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//发送一个字符
void Uart1_PutChar(uint8_t ch)
{
    /* 发送一个字节数据到USART2 */
    USART_SendData(USART1, (uint8_t) ch);
    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    //return (ch);
}


//发送一个字符串
void Uart1_PutString(uint8_t* buf , uint8_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        Uart1_PutChar(*buf++);
    }
}
//设置DMA发送的字节长度
void Set_Usart1_BuffSize_DMA_Tx(uint32_t size)
{
    DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA1_Channel4->CNDTR = (uint32_t)size; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel4, ENABLE);        //开始DMA发送
}
//设置DMA发送地址
void Set_Usart1_BuffAdd_DMA_Tx(uint32_t addess)
{
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA1_Channel4->CMAR = (uint32_t)addess;
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

void USART1_DMA_Send(uint32_t addess, uint32_t size)
{
	Set_Usart1_BuffAdd_DMA_Tx(addess);
	Set_Usart1_BuffSize_DMA_Tx(size);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/*
//^^^remove to USART_Decode_app.c
void USART1_IRQHandler(void)
{

    uint32_t temp = 0;
    uint16_t i = 0;

    OSIntEnter();   

    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        temp = USART1->SR;
        temp = USART1->DR; //清USART_IT_IDLE标志
        DMA_Cmd(DMA1_Channel5,DISABLE);
		usart1_p = USART_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
        //for(i = 0; i < usart1_p; i++)
        //{
            //Decode_buff[i] = Usart1_Rx_buff[i];
        //}
		Usart1_Received = 1;
        //USART1_DMA_Send((uint32_t)Usart1_Rx_buff, usart1_p);

		//设置传输数据长度
        DMA_SetCurrDataCounter(DMA1_Channel5,USART_BUFF_SIZE);
        //打开DMA
        DMA_Cmd(DMA1_Channel5,ENABLE);

    }

    OSIntExit();                                                                                

}
*/

//usart1 DMA TX
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)==SET)
	{
	 	DMA_ClearFlag(DMA1_FLAG_TC4);
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
	}

}

/**********************************************************/
/*------------------USART2--------------------------------*/
/**********************************************************/
/**
  * @brief  USART2 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART2_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;

		//config USART1 clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		//USART1 GPIO config
		//Configure USART2 Tx (PA.02) as alternate function push-pull
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//Configure USART2 Rx (PA.03) as input floating
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART2 mode config
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);

		//中断配置
    	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
    	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
    	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);

    	 //采用DMA方式发送
	    //USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	    //采用DMA方式接收
	    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
		USART_Cmd(USART2, ENABLE);
}
/**
  * @brief  USART2 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */

void USART2_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;

		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//NVIC_Config;	   			//配置DMA中断

        /*
        //DMA1通道7配置
        DMA_DeInit(DMA1_Channel7);
		//设置DMA源：串口数据寄存器地址
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);

		//内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Tx_buff;

		//方向：从内存到外设
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		//传输大小DMA_BufferSize=SENDBUFF_SIZE
		DMA_InitStructure.DMA_BufferSize = 0;

		//外设地址不增
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

		//内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

		//外设数据单位//
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		//内存数据单位 8bit
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

		//DMA模式：不断循环
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

		//优先级：中
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

		//禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		//配置DMA1的7通道
		DMA_Init(DMA1_Channel7, &DMA_InitStructure);

		//配置DMA发送完成后产生中断
		DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);

		//使能DMA
		DMA_Cmd (DMA1_Channel7,ENABLE);
        */
         //串口收DMA配置
	    //DMA1通道6配置
	    DMA_DeInit(DMA1_Channel6);
	    //外设地址
	    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	    //内存地址
	    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Rx_buff;
	    //dma传输方向单向
	    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	    //设置DMA在传输时缓冲区的长度
	    DMA_InitStructure.DMA_BufferSize = USART2_BUFF_SIZE;
	    //设置DMA的外设递增模式，一个外设
	    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    //设置DMA的内存递增模式
	    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	    //外设数据字长
	    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	    //内存数据字长
	    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	    //设置DMA的传输模式
	    //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	    //设置DMA的优先级别
	    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	    //设置DMA的2个memory中的变量互相访问
	    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	    DMA_Init(DMA1_Channel6,&DMA_InitStructure);

        //DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);

	    //使能通道6
	    DMA_Cmd(DMA1_Channel6,ENABLE);

}

void USART2_NVIC_Config(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /*
    //DMA 发送中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/

    //DMA接收中断设置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	//Enable the USARTy Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART2_Init()
{
    USART2_Config();
    USART2_DMA_Config();
    USART2_NVIC_Config();
}

void Set_Usart2_BuffSize_DMA_Tx(uint32_t size)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
	DMA1_Channel7->CNDTR = (uint32_t)size; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel7, ENABLE);        //开始DMA发送
}

void USART2_IRQHandler(void)
{
    uint32_t temp = 0;
    uint16_t i = 0;
    OSIntEnter(); 
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        temp = USART2->SR;
        temp = USART2->DR; //清USART_IT_IDLE标志
        DMA_Cmd(DMA1_Channel6,DISABLE);
        usart2_p = USART2_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		for (i = 0; i < usart2_p; i++)
        {
            Usart2_DJI_buff[i] = Usart2_Rx_buff[i];
        }
		Usart2_Received = 1;
        //Set_Usart1_BuffSize_DMA_Tx(usart2_p);
        //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
        //设置传输数据长度
        DMA_SetCurrDataCounter(DMA1_Channel6,USART2_BUFF_SIZE);
        //打开DMA
        DMA_Cmd(DMA1_Channel6,ENABLE);
    }

    OSIntExit();                                                                                

}


/*****************************************************/
/*-------------------USART3--------------------------*/
/*****************************************************/
/**
  * @brief  USART3 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART3_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;

		//config USART3 clock
		//开启端口C和复用功能时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);//使能端口重映射
        //GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);//使能端口重映射

		//USART1 GPIO config
		//Configure USART3 remap Tx (PC.10) as alternate function push-pull
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		//Configure USART3 remap Rx (PC.11) as input floating
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		//USART3 mode config
		USART_InitStructure.USART_BaudRate = 57600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);


		//中断配置
    	USART_ITConfig(USART3,USART_IT_TC,DISABLE);
    	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
    	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);

    	 //采用DMA方式发送
	    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
	    //采用DMA方式接收
	    //USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
		USART_Cmd(USART3, ENABLE);
}

void USART3_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;

		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//NVIC_Config();	   			//配置DMA中断

        //串口发DMA配置
        //DMA1通道2配置
        DMA_DeInit(DMA1_Channel2);
		//设置DMA源：串口数据寄存器地址
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);

		//内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Tx_buff;

		//方向：从内存到外设
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		//传输大小DMA_BufferSize=SENDBUFF_SIZE
		DMA_InitStructure.DMA_BufferSize = 0;

		//外设地址不增
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

		//内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

		//外设数据单位//
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		//内存数据单位 8bit
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

		//DMA模式：不断循环
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

		//优先级：中
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

		//禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		//配置DMA1的2通道
		DMA_Init(DMA1_Channel2, &DMA_InitStructure);

		//配置DMA发送完成后产生中断
		DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);

		//使能DMA
		DMA_Cmd (DMA1_Channel2,ENABLE);


        //串口收DMA配置
	    //DMA1通道3配置
	    DMA_DeInit(DMA1_Channel3);
	    //外设地址
	    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
	    //内存地址
	    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Rx_buff;
	    //dma传输方向单向
	    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	    //设置DMA在传输时缓冲区的长度
	    DMA_InitStructure.DMA_BufferSize = USART_BUFF_SIZE;
	    //设置DMA的外设递增模式，一个外设
	    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    //设置DMA的内存递增模式
	    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	    //外设数据字长
	    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	    //内存数据字长
	    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	    //设置DMA的传输模式
	    //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	    //设置DMA的优先级别
	    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	    //设置DMA的2个memory中的变量互相访问
	    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	    DMA_Init(DMA1_Channel3,&DMA_InitStructure);

        //DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);

	    //使能通道3
	    DMA_Cmd(DMA1_Channel3,ENABLE);
}

// NVIC config
void USART3_NVIC_Config(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    //DMA 发送中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //DMA接收中断设置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	//Enable the USARTy Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART3_Init()
{
    USART3_Config();
    USART3_DMA_Config();
    USART3_NVIC_Config();
}

void Set_Usart3_BuffSize_DMA_Tx(uint32_t size)
{
    DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA1_Channel2->CNDTR = (uint32_t)size; // 设置要发送的字节数目
    DMA_Cmd(DMA1_Channel2, ENABLE);        //开始DMA发送
}

void Set_Usart3_BuffAdd_DMA_Tx(uint32_t addess)
{
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA1_Channel2->CMAR = (uint32_t)addess;
	DMA_Cmd(DMA1_Channel2, ENABLE);
}

void USART3_DMA_Send(uint32_t addess, uint32_t size)
{
	Set_Usart3_BuffAdd_DMA_Tx(addess);
	Set_Usart3_BuffSize_DMA_Tx(size);
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
}

void USART3_IRQHandler(void)
{

    uint32_t temp = 0;
    uint16_t i = 0;
    OSIntEnter(); 
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        temp = USART3->SR;
        temp = USART3->DR; //清USART_IT_IDLE标志
        DMA_Cmd(DMA1_Channel3,DISABLE);
		usart3_p = USART_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		//for(i = 0; i < usart3_p; i++)
		//{
			//Usart3_to_Usart1[i] = Usart3_Rx_buff[i];
		//}
		Usart3_Received = 1;
		//设置传输数据长度
        DMA_SetCurrDataCounter(DMA1_Channel3,USART_BUFF_SIZE);
        //打开DMA
        DMA_Cmd(DMA1_Channel3,ENABLE);
    }
    OSIntExit(); 

}

//usart3 DMA TX
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC2)==SET)
	{
	 	DMA_ClearFlag(DMA1_FLAG_TC2);
		USART_DMACmd(USART3, USART_DMAReq_Tx, DISABLE);
	}
}

/*********************************************END OF FILE**********************/
