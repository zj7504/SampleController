/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  zj
  * @version V1.0
  * @date    2017-xx-xx
  *****************************************************************************
  *
  *can总线通信配置
  *
  ******************************************************************************
  */
#include "bsp_can.h"

/*
* 函数名：CAN_GPIO_Config
* 描述 ：CAN 的 GPIO 配置,PB8 上拉输入，PB9 推挽输出
* 输入 ：无
* 输出 : 无
* 调用 ：内部调用
*/
static void CAN_GPIO_Config(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  /*外设时钟设置*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,
  ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  /*IO 设置*/
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
  /* Configure CAN pin: RX */ // PB8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure CAN pin: TX */ // PB9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
* 函数名：CAN_NVIC_Config
* 描述 ：CAN 的 NVIC 配置,第 1 优先级组，0，0 优先级
* 输入 ：无
* 输出 : 无
* 调用 ：内部调用
*/
static void CAN_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  /*中断设置*/
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  //CAN1 RX0 中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //子优先级为 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
02 * 函数名：CAN_Mode_Config
03 * 描述 ：CAN 的模式 配置
04 * 输入 ：无
05 * 输出 : 无
06 * 调用 ：内部调用
07 */
static void CAN_Mode_Config(void) {
  CAN_InitTypeDef CAN_InitStructure;
  /************************CAN 通信参数设置*************************/
  /*CAN 寄存器初始化*/
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /*CAN 单元初始化*/
  CAN_InitStructure.CAN_TTCM=DISABLE; //MCR-TTCM关闭时间触发通信模式使能
  CAN_InitStructure.CAN_ABOM=ENABLE; //MCR-ABOM自动离线管理
  CAN_InitStructure.CAN_AWUM=ENABLE; //MCR-AWUM使用自动唤醒模式
  CAN_InitStructure.CAN_NART=DISABLE; //MCR-NART禁止报文自动重传 DISABLE-自动重传
  CAN_InitStructure.CAN_RFLM=DISABLE; //MCR-RFLM 接收 FIFO锁定模式 DISABLE-溢出时新报文会覆盖原有报文
  CAN_InitStructure.CAN_TXFP=DISABLE; //MCR-TXFP发送 FIFO 优先级 DISABLE-优先级取决于报文标示符
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  //正常工作模式
  CAN_InitStructure.CAN_SJW=CAN_SJW_2tq; //BTR-SJW重新同步跳跃宽度2 个时间单元
  CAN_InitStructure.CAN_BS1=CAN_BS1_6tq; //BTR-TS1 时间段 1占用了 6 个时间单元
  CAN_InitStructure.CAN_BS2=CAN_BS2_3tq; //BTR-TS1 时间段 2占用了 3 个时间单元
  CAN_InitStructure.CAN_Prescaler = 4; ////BTR-BRP波特率分频器定义了时间单元的时间长 36/(1+6+3)/4=0.9Mbps
  CAN_Init(CAN1, &CAN_InitStructure);
}

/*
02 * 函数名：CAN_Filter_Config
03 * 描述 ：CAN 的过滤器 配置
04 * 输入 ：无
05 * 输出 : 无
06 * 调用 ：内部调用
07 */
static void CAN_Filter_Config(void)
{
  CAN_FilterInitTypeDef CAN_FilterInitStructure;

  /*CAN 过滤器初始化*/
  CAN_FilterInitStructure.CAN_FilterNumber=0; //过滤器组 0
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  //工作在标识符屏蔽位模式
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  //过滤器位宽为单个 32 位。
  /*使能报文标示符过滤器按照标示符的内容进行比对过滤，扩展 ID 不是如下��
   �就抛弃掉，是的话，会存入 FIFO0。 */
  CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)0x1314<<3)&0xFFFF0000)>>16;
  //要过滤的 ID 高位
  CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
  //要过滤的 ID 低位
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF; //过滤器高 16位每位必须匹配
  CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0xFFFF; //过滤器低 16位每位必须匹配
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;
  //过滤器被关联到 FIFO0
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //使能过滤器
  CAN_FilterInit(&CAN_FilterInitStructure);
  /*CAN 通信中断使能*/
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void CAN_Config(void) {
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();
}
