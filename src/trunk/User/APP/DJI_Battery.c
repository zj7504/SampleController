/**********************************************************************
*                                                                    *
*        DJI Battery decode       *
*                                                                    *
**********************************************************************
*/
#include "includes.h"
//#include "Sample_app.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************/

#pragma  pack(1) //按照1字节方式对齐

typedef struct _DJI_Battery_Inf
{
	uint8_t voltage_lo; //电压LSB
	uint8_t voltage_hi; //电压MSB
	uint8_t capacity_lo;  //电池容量LSB  
	uint8_t capacity_hi;    //电池容量MSB
	uint8_t current_mAH_lo;     //剩余电量LSB
	uint8_t current_mAH_hi;     //剩余电量MSB
	uint8_t temperature_lo;     //温度LSB
	uint8_t temperature_hi;     //温度MSB
	uint8_t percent_mAH;        //电量百分比
	
} DJI_Battery_Inf;

typedef struct _DJI_Header
{
	uint8_t head_1;
	uint8_t head_2;
	uint8_t head_3;
	uint8_t head_4;
	uint8_t head_5;
} DJI_Header;

typedef	struct _Send_Battery_Inf	
{
	DJI_Header header;
	DJI_Battery_Inf dji_battery_inf;
	uint8_t crc_lo;
	uint8_t crc_hi;
} Send_Battery_Inf;

#pragma apop() //取消1字节对齐方式

Send_Battery_Inf send_battery_inf;

/*********************************************************************
*
*       Static code
*
**********************************************************************/

//checksum
static void Checksum(const uint8_t *buf, uint8_t len)
{
	uint16_t sum;
	uint16_t i;
	for(i = 0; i < len; i++)
	{
		sum += (uint16_t)*buf++;
	}
	send_battery_inf.crc_lo = (uint8_t)sum;
	send_battery_inf.crc_hi = (uint8_t)(sum/256);
}

//**********************************************
//解析串口数据
//**********************************************
static void RxComdAction(void) 
{
	uint8_t i;
	if(Usart2_Received == 1)
	{
		Usart2_Received = 0;
		for(i = 0; i <= usart2_p; i++)
		{
			if((Usart2_DJI_buff[i] == 0x55)&&(Usart2_DJI_buff[i+1] == 0x2B)&&(Usart2_DJI_buff[i+2] == 0x04)&&(Usart2_DJI_buff[i+3] == 0x58)&&(Usart2_DJI_buff[i+4] == 0x05))
			{	
				send_battery_inf.dji_battery_inf.voltage_lo = Usart2_DJI_buff[i+12];
				send_battery_inf.dji_battery_inf.voltage_hi = Usart2_DJI_buff[i+13];
				
				send_battery_inf.dji_battery_inf.capacity_lo = Usart2_DJI_buff[i+20];
				send_battery_inf.dji_battery_inf.capacity_hi = Usart2_DJI_buff[i+21];
				
				send_battery_inf.dji_battery_inf.current_mAH_lo = Usart2_DJI_buff[i+24];
				send_battery_inf.dji_battery_inf.current_mAH_hi = Usart2_DJI_buff[i+25];

				send_battery_inf.dji_battery_inf.temperature_lo = Usart2_DJI_buff[i+28];
				send_battery_inf.dji_battery_inf.temperature_hi = Usart2_DJI_buff[i+29];	

				send_battery_inf.dji_battery_inf.percent_mAH = Usart2_DJI_buff[i+31];

				
                USART1_DMA_Send((uint32_t)&send_battery_inf, 14);
			}
		}
		memset(&Usart2_DJI_buff, 0, USART_BUFF_SIZE);
	}
}


//TASK DJI Battery
void Task_DJI_Battery(void *p_arg)
{
	OS_ERR err;
	(void)p_arg; 

	send_battery_inf.header.head_1 = 0x55;
	send_battery_inf.header.head_2 = 0x2b;
	send_battery_inf.header.head_3 = 0x04;
	send_battery_inf.header.head_4 = 0x58;
	send_battery_inf.header.head_5 = 0x05;
	
	while(1)
	{
		RxComdAction();
        OSTimeDlyHMSM(0, 0,0,1,OS_OPT_TIME_HMSM_STRICT,&err);	//delay 10ms	
	}

}

