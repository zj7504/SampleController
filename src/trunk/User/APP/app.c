/**********************************************************************
*                                                                    *
*        Support							    					 *
*                                                                    *
**********************************************************************/

#include <includes.h>


/*********************************************************************
*
*       Defines
*
**********************************************************************/

/*******************设置任务优先级*******************/
#define	TASK_Breathing_PRIO			    5
#define	TASK_USART_Decode_PRIO			3
#define TASK_Sample_PRIO                4
#define Task_DJI_Battery_PRIO			6


/************设置栈大小（单位为 OS_STK ）************/
#define TASK_Breathing_STK_SIZE         80
#define TASK_USART_Decode_STK_SIZE   	80
#define TASK_Sample_STK_SIZE            80
#define TASK_DJI_Battery_STK_SIZE       80



/*********************************************************************
*
*       Static data
*
**********************************************************************/

extern void Task_Breathing(void *p_arg);
extern void Task_USART_Decode(void *p_arg);
extern void Task_Sample(void *p_arg);
extern void Task_DJI_Battery(void * p_arg);


/********************APP TCB（定义任务控制块）*************************/
static  OS_TCB	Breathing_TCB;
static  OS_TCB	USART_Decode_TCB;
static  OS_TCB  Sample_TCB;
static  OS_TCB  DJI_Battery_TCB;


/********************STACKS（定义任务堆栈）**************************/		   				
static	CPU_STK	Breathing_Stk[TASK_Breathing_STK_SIZE];	
static	CPU_STK	USART_Decode_Stk[TASK_Breathing_STK_SIZE];	
static  CPU_STK Sample_Stk[TASK_Sample_STK_SIZE];
static  CPU_STK DJI_Battery_Stk[TASK_DJI_Battery_STK_SIZE];



void MainAPP()
{
	OS_ERR err;
/*
	//创建任务呼吸灯	
	OSTaskCreate((OS_TCB     *)&Breathing_TCB,					            // 任务控制块指针          
               (CPU_CHAR   *)"Braething",						            // 任务名称
               (OS_TASK_PTR )Task_Breathing,					            // 任务代码指针
               (void       *)0,							                // 传递给任务的参数parg
               (OS_PRIO     )TASK_Breathing_PRIO,				        // 任务优先级
               (CPU_STK    *)&Breathing_Stk[0],				            // 任务堆栈基地址
               (CPU_STK_SIZE)TASK_Breathing_STK_SIZE/10,		        // 堆栈剩余警戒线
               (CPU_STK_SIZE)TASK_Breathing_STK_SIZE,			        // 堆栈大小
               (OS_MSG_QTY  )0,							                // 可接收的最大消息队列数
               (OS_TICK     )0,							                // 时间片轮转时间
               (void       *)0,							                // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        			// 任务选项
               (OS_ERR     *)&err);						                // 返回值
*/

   	//创建任务串口2	
	OSTaskCreate((OS_TCB     *)&USART_Decode_TCB,					            // 任务控制块指针          
               (CPU_CHAR   *)"USART_Decode",						            // 任务名称
               (OS_TASK_PTR )Task_USART_Decode,					            // 任务代码指针
               (void       *)0,							                // 传递给任务的参数parg
               (OS_PRIO     )TASK_USART_Decode_PRIO,				        // 任务优先级
               (CPU_STK    *)&USART_Decode_Stk[0],				            // 任务堆栈基地址
               (CPU_STK_SIZE)TASK_USART_Decode_STK_SIZE/10,		        // 堆栈剩余警戒线
               (CPU_STK_SIZE)TASK_USART_Decode_STK_SIZE,			        // 堆栈大小
               (OS_MSG_QTY  )0,							                // 可接收的最大消息队列数
               (OS_TICK     )0,							                // 时间片轮转时间
               (void       *)0,							                // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        			// 任务选项
               (OS_ERR     *)&err);						                // 返回值
     
	 //创建任务采样	
	OSTaskCreate((OS_TCB     *)&Sample_TCB,					            // 任务控制块指针          
               (CPU_CHAR   *)"Sample",						            // 任务名称
               (OS_TASK_PTR )Task_Sample,					            // 任务代码指针
               (void       *)0,							                // 传递给任务的参数parg
               (OS_PRIO     )TASK_Sample_PRIO,				        // 任务优先级
               (CPU_STK    *)&Sample_Stk[0],				            // 任务堆栈基地址
               (CPU_STK_SIZE)TASK_Sample_STK_SIZE/10,		        // 堆栈剩余警戒线
               (CPU_STK_SIZE)TASK_Sample_STK_SIZE,			        // 堆栈大小
               (OS_MSG_QTY  )0,							                // 可接收的最大消息队列数
               (OS_TICK     )0,							                // 时间片轮转时间
               (void       *)0,							                // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        			// 任务选项
               (OS_ERR     *)&err);						                // 返回值

 	//创建任务采样 
	OSTaskCreate((OS_TCB	 *)&DJI_Battery_TCB, 							// 任务控制块指针		   
		   (CPU_CHAR   *)"DJI_Battery",									// 任务名称
		   (OS_TASK_PTR )Task_DJI_Battery,								// 任务代码指针
		   (void	   *)0, 										// 传递给任务的参数parg
		   (OS_PRIO 	)Task_DJI_Battery_PRIO,						// 任务优先级
		   (CPU_STK    *)&DJI_Battery_Stk[0],							// 任务堆栈基地址
		   (CPU_STK_SIZE)TASK_DJI_Battery_STK_SIZE/10,				// 堆栈剩余警戒线
		   (CPU_STK_SIZE)TASK_DJI_Battery_STK_SIZE,					// 堆栈大小
		   (OS_MSG_QTY	)0, 										// 可接收的最大消息队列数
		   (OS_TICK 	)0, 										// 时间片轮转时间
		   (void	   *)0, 										// 任务控制块扩展信息
		   (OS_OPT		)(OS_OPT_TASK_STK_CHK | 
						  OS_OPT_TASK_STK_CLR), 					// 任务选项
		   (OS_ERR	   *)&err); 	

              
}

