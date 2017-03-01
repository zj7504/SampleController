/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*											//定义任务控制块
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;


/*
*********************************************************************************************************
*                                                STACKS
*											//定义任务堆栈
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);

/******************* 定时器服务函数 ***************************/
/******************** 串口1定时发送 ***************************/
/*************************************************************/
static void _cbOfTmr1(OS_TMR *p_tmr, void *p_arg)
{
    //OS_ERR err;	
	(void)p_arg;
    
	//printf("\r\n hello world! \r\n");
    
    /*Send the information of Status for DJI M600*/
    //Uart1_PutString(&Usart1_DJI_buff , 14);
    
    /*test*/
    //Uart1_PutChar(Infrared_Count);
	//Uart1_PutString((uint8_t* )&Fixed_MAX_PWM , 2);
    //Uart1_PutChar(BERO1_Count);
    //printf("%d\r\n", Fixed_MAX_PWM);
    //printf("%d\r\n", Fixed_MIN_PWM);
    //printf("%d\r\n", Rope_Length);
    //printf("%d\r\n", Hole_num);
    //printf("%d\r\n", Perimeter);
  
    /*prtocol v2*/
    USART1_DMA_Send((uint32_t)&Bottle_Sample, Bottle_Sample.header.length);

}


/******************* 定时器服务函数 ***************************/
/**********************************************/
/*************************************************************/
static void _cbOfTmr2(OS_TMR *p_tmr, void *p_arg)
{
    //OS_ERR err;	
	(void)p_arg;
	//printf("\r\n hello world! \r\n");
    /***延时检测计时*/
    switch(winch_status)
    {
        case Winch_Down:
            bero_timer++;
            if(bero_timer > 4)
                Winch_Down_Check = true;
            break;
        case Winch_Up:
            bero_timer++;
            if(bero_timer > 4)
                Winch_Up_Check = true;
            break;
        default:
            bero_timer = 0;
            break;
    }

}

/******************* 定时器服务函数 ***************************/
/********************  ***************************/
/*************************************************************/
static void _cbOfTmr3(OS_TMR *p_tmr, void *p_arg)
{
    //OS_ERR err;	
	(void)p_arg;
	//printf("\r\n hello world! \r\n");
   /****向底层MOS板发送控制命令*/

}

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    //设置中断向量表的位置在 0x3000
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
    //需要修改stm32f10x.h文件中的FLASH_BASE地址，修改为APP程序的开始地址为0x08003000;
    
    OS_ERR  err;

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR      err;
    OS_TMR 		MyTmr,MyTmr2,MyTmr3;
   (void)p_arg;
    
    
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();

//    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

//    CPU_IntDisMeasMaxCurReset();
    
    //APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create Application Tasks                             */
    
    //APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppObjCreate();                                             /* Create Application Objects                           */
    
	//创建定时器
  	OSTmrCreate((OS_TMR              *)&MyTmr,
               (CPU_CHAR            *)"MyTimer",          
               (OS_TICK              )1,                //定时器的频率是10Hz，值取1正好是100ms//开始循环模式前等待第一次启动的超时时间
               (OS_TICK              )2,                //取值2，估算时间为0.2s
               (OS_OPT               )OS_OPT_TMR_PERIODIC,//模式设置为重复模式
               (OS_TMR_CALLBACK_PTR  )_cbOfTmr1,          //回调函数
               (void                *)0,                  //参数设置为0
               (OS_ERR              *)err);
  
	//启动定时器
	OSTmrStart((OS_TMR *)&MyTmr,(OS_ERR *)err);
	
	//创建定时器
  	OSTmrCreate((OS_TMR              *)&MyTmr2,
               (CPU_CHAR            *)"MyTimer2",          
               (OS_TICK              )1,                //定时器的频率是10Hz，值取1正好是100ms
               (OS_TICK              )5,                //定时器的频率是100Hz，值取1正好10ms
               (OS_OPT               )OS_OPT_TMR_PERIODIC,//模式设置为重复模式
               (OS_TMR_CALLBACK_PTR  )_cbOfTmr2,          //回调函数
               (void                *)0,                  //参数设置为0
               (OS_ERR              *)err);
  
	//启动定时器
	OSTmrStart((OS_TMR *)&MyTmr2,(OS_ERR *)err);	
    
    /*
    //创建定时器
  	OSTmrCreate((OS_TMR              *)&MyTmr3,
               (CPU_CHAR            *)"MyTimer3",          
               (OS_TICK              )1,                //定时器的频率是10Hz，值取1正好是100ms
               (OS_TICK              )1,                //定时器的频率是100Hz，值取1正好10ms
               (OS_OPT               )OS_OPT_TMR_PERIODIC,//模式设置为重复模式
               (OS_TMR_CALLBACK_PTR  )_cbOfTmr3,          //回调函数
               (void                *)0,                  //参数设置为0
               (OS_ERR              *)err);
  
	//启动定时器
	OSTmrStart((OS_TMR *)&MyTmr3,(OS_ERR *)err);               
    */
               
	/*Delete task*/
    OSTaskDel(&AppTaskStartTCB, &err);
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	MainAPP();		
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
}


/*********************************************END OF FILE**********************/
