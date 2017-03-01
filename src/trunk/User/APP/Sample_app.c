/*********************************************************************/

#include "includes.h"
//#include "protocol.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

uint8_t Bero_Check_Flag;

#if	PUMP_SAMPLE == 1
Ask_Command Pump_Touch_Sensor;
Ask_Command Pump_Full_Sensor;
Ask_Command Pump_Status;

#elif	BOTTLE_SAMPLE == 1
Ask_Command	Bottle_Sample;

#elif	BUOY_RELEASE == 1
Ask_Command	Buoy_Release;
#endif
Ask_Command Health_Status;

bool Touch_Flag = false;
bool Full_Flag[2] = {false};
bool Health_Status_Flag = false;
volatile uint8_t	Pump_Sample_Flag[4] = {0};
volatile uint8_t	Buoy_Flag[2] = {0};

/*Parameter store in eeprom*/
uint16_t   Fixed_MAX_PWM = 1250 ;    //固定装置舵机PWM值( 0度：250，180度：1250)
uint16_t   Fixed_MIN_PWM = 250;
bool Fixed_Enable = false;
//卷扬机构绳索长度计算
uint16_t   Rope_Length = 150;
uint8_t    Hole_num = 1; //number of hole for whell
uint8_t    Perimeter = 11 ; //perimeter of whell

/***************************/
bool Winch_Down_Check = false; //接近开关检查标志
bool Winch_Up_Check = false;
uint8_t bero_timer = 0; //延时开启接近开关检查计数
uint8_t Down_First_Enter;
uint8_t Up_First_Enter;
uint8_t Bottle_Sample_Finish = 0;
uint32_t CRC_Transfer = 0;

enum Winch_Status winch_status = Winch_Stop;





/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

void Sensor_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /*配置传感器io口*/
    GPIO_InitStructure.GPIO_Pin = Full_Pin | Touch_Pin;
    //	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    //上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/**
端口状态扫描
  */
uint8_t Port_Scan(GPIO_TypeDef *GPIOx, u16 GPIO_Pin, uint8_t Down_state)
{
    OS_ERR err;
    /**/
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == Down_state )
    {
        /*延时*/
        OSTimeDlyHMSM(0, 0, 0, 5, OS_OPT_TIME_HMSM_STRICT, &err);
        if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == Down_state )
        {
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}



/*初始化ask数据头*/
void Init_ASK_Header(Ask_Command *command)
{
    command->header.sof = 0xAA;
    command->header.length = 0x0D;
    command->header.isAck = 1;
    command->header.version = 0;
    command->header.reserve = 0;
    command->header.crc16 = 0; //crc16 = 0xc2b3
}

/*传感器状态检查*/
void Sensor_Check(void)
{
    if( Port_Scan(Sensor_Port, Full_Pin, 0))
    {
        Full_Flag[0] = true;
    }
    else
    {
        Full_Flag[0] = false;
    }

    if( Port_Scan(Sensor_Port, Touch_Pin, 0))
    {
        Touch_Flag = true;
    }
    else
    {
        Touch_Flag = false;
    }
}

/*Key contol*/
void Key_Control(void)
{
		if( (Port_Scan(GPIOC, Full_Pin, 0))&&(Port_Scan(GPIOC, Touch_Pin, 1)))
    {
				NEGATIVE_SPIN; //卷扬机下降
    }
    else if((Port_Scan(GPIOC, Touch_Pin, 0))&&(Port_Scan(GPIOC, Full_Pin, 1)))
    {
        POSITIVE_SPIN; //卷扬机构旋转
    }
		else
		{
				MOS_ALL_OFF;
		}
}

/*Servo control*/
void Sample_Servo_Stop(void)
{
    Set_PWM_Value(TIM2, TIM2_CH1, 750); //middle point 1500us
}

void Sample_Positive_Spin(void)
{
    //Set_PWM_Value(TIM3,TIM3_CH1, 250);
    Set_PWM_Value(TIM2, TIM2_CH1, 500);
}

void Sample_Negative_Spin(void)
{
    //Set_PWM_Value(TIM3,TIM3_CH1, 1250);
    Set_PWM_Value(TIM2, TIM2_CH1, 1000);
}

//采样瓶固定机构
void Fixture_Release(void)
{
    Set_PWM_Value(TIM2, TIM2_CH1, Fixed_MAX_PWM);
}

void Fixture_clamp(void)
{
    Set_PWM_Value(TIM2, TIM2_CH1, Fixed_MIN_PWM);
}


/*****长度转换为卷扬机旋转圈数*******/
/*** length---释放长度***************/
/*** perimeter---卷线轮周长  cm *****/
/*** para---卷线轮红外检测孔数量*****/
uint16_t Length_To_Checknum(uint16_t length, uint8_t perimeter, uint8_t para)
{
    uint16_t turns = (uint16_t)length / Perimeter / para;
    return turns;
}

#ifdef USE_SERVO
void Bottle_Sample_Control(void)
{
    OS_ERR err;
    uint32_t tick;
    switch(winch_status)
    {
    case Winch_Stop:
        BERO1_Count = 0;
        Winch_Down_Check = false;
        Winch_Up_Check = false;
        Up_First_Enter = 1;
        Down_First_Enter = 1;
        Sample_Servo_Stop();
        break;
    case Winch_Down:
        while(Down_First_Enter)
        {
            Sample_Servo_Stop();
            Down_First_Enter = 0;
            Up_First_Enter = 1;
            tick = OSTimeGet(&err);
            Winch_Up_Check = false;
            Infrared_Count = 0;
            Fixture_Release(); //release bottle clamp
        }
        Sample_Negative_Spin(); //卷扬机下降
        if(OSTimeGet(&err) - tick  > 500)
        {
            Winch_Down_Check = true;
        }

        if(Infrared_Count > Length_To_Checknum(Rope_Length, Perimeter, Hole_num))
        {
            winch_status = Winch_Stop;
            Sample_Servo_Stop();
            Winch_Down_Check = false;
            Bottle_Sample_Finish = 1;
        }

        if(Winch_Down_Check)
        {
            if(BERO1_Count > 0)
            {
                winch_status = Winch_Stop;
                Sample_Servo_Stop();
                Winch_Down_Check = false;
                Bottle_Sample_Finish = 1;
            }
        }
        else
        {
            BERO1_Count = 0;
            Bottle_Sample_Finish = 0;
        }
        break;
    case Winch_Up:
        while(Up_First_Enter)
        {
            Sample_Servo_Stop();
            Up_First_Enter = 0;
            Down_First_Enter = 1;
            tick = OSTimeGet(&err);
            Winch_Down_Check = false;
            Infrared_Count = 0;
        }
        Sample_Positive_Spin(); //卷扬机构旋转
        if(OSTimeGet(&err) - tick > 500)
        {
            Winch_Up_Check = true;
        }

        if(Infrared_Count > Length_To_Checknum(Rope_Length, Perimeter, Hole_num))
        {
            winch_status = Winch_Stop;
            Sample_Servo_Stop();
            Winch_Up_Check = false;
            Bottle_Sample_Finish = 2;
            Fixture_clamp();//fix bottle
        }

        if(Winch_Up_Check)
        {
            if(BERO1_Count > 0)
            {
                winch_status = Winch_Stop;
                Sample_Servo_Stop();
                Winch_Up_Check = false;
                Bottle_Sample_Finish = 2;
                Fixture_clamp();//fix bottle
            }
        }
        else
        {
            BERO1_Count = 0;
            Bottle_Sample_Finish = 0;
        }
        break;
    }
}

//投放装置控制代码在此添加，舵机控制方式
#endif


#ifdef USE_MOS
void Bottle_Sample_Control(void)
{
    //OS_ERR err;
    //uint32_t tick;
    switch(winch_status)
    {
    case Winch_Stop:
        BERO1_Count = 0;
        bero_timer = 0;
        Winch_Down_Check = false;
        Winch_Up_Check = false;
        Up_First_Enter = 1;
        Down_First_Enter = 1;
        MOS_ALL_OFF;
        break;
    case Winch_Down:
        while(Down_First_Enter)
        {
            MOS_ALL_OFF;
            Down_First_Enter = 0;
            Up_First_Enter = 1;
            Winch_Down_Check = false;
            bero_timer = 0;
					  Infrared_Count = 0;
            Fixture_Release(); //release bottle clamp
        }
        NEGATIVE_SPIN; //卷扬机下降

        if(Infrared_Count > Length_To_Checknum(Rope_Length, Perimeter, Hole_num))
        {
            winch_status = Winch_Stop;
            Winch_Down_Check = false;
            Bottle_Sample_Finish = 1;
        }
        //else if(Length_To_Checknum(Rope_Length, Perimeter, Hole_num) >= Infrared_Count > (Length_To_Checknum(Rope_Length, Perimeter, Hole_num) - 5))
        {
            if(Winch_Down_Check)
            {
                if(BERO1_Count > 0)
                {
                    winch_status = Winch_Stop;
                    MOS_ALL_OFF;
                    Winch_Down_Check = false;
                    Bottle_Sample_Finish = 1;
                }
            }
            else
            {
                BERO1_Count = 0;
                Bottle_Sample_Finish = 0;
            }
        }
        break;
    case Winch_Up:
        while(Up_First_Enter)
        {
            MOS_ALL_OFF;
            Up_First_Enter = 0;
            Down_First_Enter = 1;
            Winch_Up_Check = false;
            bero_timer = 0;
					  Infrared_Count = 0;
        }
        POSITIVE_SPIN; //卷扬机构旋转

        if(Infrared_Count > Length_To_Checknum(Rope_Length, Perimeter, Hole_num))
        {
            winch_status = Winch_Stop;
            Winch_Up_Check = false;
            Bottle_Sample_Finish = 2;
            Fixture_clamp();//fix bottle
        }
        //else if(Length_To_Checknum(Rope_Length, Perimeter, Hole_num) >= Infrared_Count > (Length_To_Checknum(Rope_Length, Perimeter, Hole_num) - 5))
        {
            if(Winch_Up_Check)
            {
                if(BERO1_Count > 0)
                {
                    winch_status = Winch_Stop;
                    MOS_ALL_OFF;
                    Winch_Up_Check = false;
                    Bottle_Sample_Finish = 2;
                    Fixture_clamp();//fix bottle
                }
            }
            else
            {
                BERO1_Count = 0;
                Bottle_Sample_Finish = 0;
            }
        }
        break;
    }

}

void Buoy_Control(void)
{
    if(Buoy_Flag[0] == 1)
    {
        MOTOR_A_ON;
    }
    else
    {
        MOTOR_A_OFF;
    }

    if(Buoy_Flag[1] == 1)
    {
        MOTOR_B_ON;
    }
    else
    {
        MOTOR_B_OFF;
    }

}
#endif

//状态处理
void Status_Handle(void)
{
#if	PUMP_SAMPLE == 1
    //更新水满状态数据

    Init_ASK_Header(&Pump_Full_Sensor);
    Pump_Full_Sensor.set = ASK_SET_PUMP_SAMPLE;
    Pump_Full_Sensor.id = AID_WATER_FULL;
    Pump_Full_Sensor.value = (Full_Flag[1] << 1) | Full_Flag[0];
    calculateCRC(&Pump_Full_Sensor);

    //更新水面接触状态数据
    Init_ASK_Header(&Pump_Touch_Sensor);
    Pump_Touch_Sensor.set = ASK_SET_PUMP_SAMPLE;
    Pump_Touch_Sensor.id = AID_WATER_TOUCH;
    Pump_Touch_Sensor.value = Touch_Flag;
    calculateCRC(&Pump_Touch_Sensor);

    //更新水泵运行状态数据
    Init_ASK_Header(&Pump_Status);
    Pump_Status.set = ASK_SET_PUMP_SAMPLE;
    Pump_Status.id = AID_PUMP_STATUS;
    Pump_Status.value = (Pump_Sample_Flag[3] << 3) | (Pump_Sample_Flag[2] << 2) | (Pump_Sample_Flag[1] << 1) | Pump_Sample_Flag[0];
    calculateCRC(&Pump_Status);

    //根据条件相应控制水泵状态
    //Pump #1
    if((Full_Flag[0] == 0) && (Pump_Sample_Flag[0] == 1))
    {
        //Open the Pump #1
        MOTOR_A_ON;
    }
    else
    {
        MOTOR_A_OFF;
        if(Full_Flag[0] == 1)
            Pump_Sample_Flag[0] = 0;
    }
    //Pump #2
    if((Full_Flag[1] == 0) && (Pump_Sample_Flag[1] == 1))
    {
        //Open the Pump #2
        MOTOR_B_ON;
    }
    else
    {
        MOTOR_B_OFF;
        if(Full_Flag[1] == 1)
            Pump_Sample_Flag[1] = 0;
    }

#elif	BOTTLE_SAMPLE == 1
    //采样控制
    Bottle_Sample_Control();

    //控制采样瓶固定装置，由飞控发送控制命令
    //if(Fixed_Enable)
    //{
    //    Fixed_Enable = false;
    //Set_PWM_Value(TIM3,TIM3_CH2, Fixed_PWM_Value ); //控制采样瓶固定装置
    //}

    //更新采样状态数据
    Init_ASK_Header(&Bottle_Sample);
    Bottle_Sample.set = ASK_SET_BOTTLE_SAMPLE;
    Bottle_Sample.id = AID_SAMPLE_STATUS;
    Bottle_Sample.value = Bottle_Sample_Finish;
    calculateCRC(&Bottle_Sample);

#else
    //浮标投放控制
    Buoy_Control();
    //浮标投放状态
    Init_ASK_Header(&Buoy_Release);
    Buoy_Release.set = ASK_SET_COMMON;
    Buoy_Release.id = CID_THROW_CONTROL;
    Buoy_Release.value = (Buoy_Flag[1] << 1) | Buoy_Flag[0];
    calculateCRC(&Buoy_Release);

#endif

}

void Parameter_Read_eeprom(void)
{
    EE_ReadVariable(VirtAddVarTab[Max_PWM_Add], &Fixed_MAX_PWM);
    EE_ReadVariable(VirtAddVarTab[Min_PWM_Add], &Fixed_MIN_PWM);
    EE_ReadVariable(VirtAddVarTab[Rope_Len_Add], &Rope_Length);
    EE_ReadVariable(VirtAddVarTab[Hole_Num_Add], (uint16_t *)&Hole_num);
    EE_ReadVariable(VirtAddVarTab[Perimeter_Add], (uint16_t *)&Perimeter);
}

void Sample_Inti(void)
{
    Sensor_GPIO_Config();
}

//TASK Samples
void Task_Sample(void *p_arg)
{
    OS_ERR err;
    (void)p_arg;
    /*read parameter from flash*/
    Parameter_Read_eeprom();
    Sample_Inti();
		Fixture_clamp();
    while(1)
    {
#if	PUMP_SAMPLE	== 1
        Sensor_Check();
#elif BOTTLE_SAMPLE == 1
				Key_Control();
#endif
        Status_Handle();

        if(Health_Status_Flag)
        {
            Health_Status_Flag = false;
            Init_ASK_Header(&Health_Status);
            Health_Status.set = ASK_SET_SYSTEM;
            Health_Status.id = AID_SYSTEM_HEALTH;
            Health_Status.value = 0xaa;
            calculateCRC(&Health_Status);
            USART1_DMA_Send((uint32_t)&Health_Status, Health_Status.header.length);
        }

        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);	//延时
    }

}

