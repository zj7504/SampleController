/**********************************************************************
*                                                                    *
*        USART 2 decode       *
*                                                                    *
**********************************************************************
*/

#include "includes.h"
//#include <protocol.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************/
Recv_Filter filter;
uint16_t Update_Flag;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

//CRC 16
uint16_t crc_tab16[] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780,
    0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1,
    0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801,
    0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40,
    0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680,
    0xd641, 0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0,
    0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501,
    0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1, 0xe981,
    0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1,
    0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200,
    0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141,
    0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480,
    0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0,
    0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01,
    0x7bc0, 0x7a80, 0xba41, 0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381,
    0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0,
    0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01,
    0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40,
    0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81,
    0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41, 0x4400, 0x84c1,
    0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341, 0x4100,
    0x81c1, 0x8081, 0x4040,
};

uint32_t crc_tab32[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535,
    0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd,
    0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d,
    0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac,
    0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
    0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb,
    0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
    0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce,
    0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409,
    0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739,
    0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268,
    0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
    0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8,
    0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
    0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
    0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae,
    0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6,
    0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d,
    0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5,
    0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};
    
    
const unsigned short CRC_INIT = 0x3AA3;    
    
uint16_t crc16_update(uint16_t crc, uint8_t ch)
{
    uint16_t tmp;
    uint16_t msg;

    msg = 0x00ff & (uint16_t)ch;
    tmp = crc ^ msg;
    crc = (crc >> 8) ^ crc_tab16[tmp & 0xff];

    return crc;
}

uint32_t crc32_update(uint32_t crc, uint8_t ch)
{
    uint32_t tmp;
    uint32_t msg;

    msg = 0x000000ffL & (uint32_t)ch;
    tmp = crc ^ msg;
    crc = (crc >> 8) ^ crc_tab32[tmp & 0xff];
    return crc;
}
    
uint16_t sdk_stream_crc16_calc(const uint8_t *pMsg, size_t nLen)
{
    size_t i;
    uint16_t wCRC = CRC_INIT;

    for (i = 0; i < nLen; i++)
    {
        wCRC = crc16_update(wCRC, pMsg[i]);
    }

    return wCRC;
}

uint32_t sdk_stream_crc32_calc(const uint8_t *pMsg, size_t nLen)
{
    size_t i;
    uint32_t wCRC = CRC_INIT;

    for (i = 0; i < nLen; i++)
    {
        wCRC = crc32_update(wCRC, pMsg[i]);
    }

    return wCRC;
}

void calculateCRC(void *p_data)
{
    Header *p_head = (Header *)p_data;
    unsigned char *p_byte = (unsigned char *)p_data;
    unsigned int index_of_crc2;

    if (p_head->sof != _SDK_SOF)
        return;

    p_head->crc16 = sdk_stream_crc16_calc(p_byte, _SDK_HEAD_DATA_LEN);

    if (p_head->length >= _SDK_FULL_DATA_SIZE_MIN)
    {
        index_of_crc2 = p_head->length - _SDK_CRC_DATA_SIZE;
        _SDK_U32_SET(p_byte + index_of_crc2, sdk_stream_crc32_calc(p_byte, index_of_crc2));
    }
}

// get cmd set
uint8_t GetCmdSet(Header *header)
{
    uint8_t *ptemp = ((unsigned char *)header) + sizeof(Header);
    return *ptemp;
}

//get cmd id
uint8_t getCmdID(Header *header)
{
    unsigned char *ptemp = ((unsigned char *)header) + sizeof(Header);
    ptemp++;
    return *ptemp;
}

void passData(void *data, unsigned char *buf,uint8_t datalen)
{
    memcpy((unsigned char *)data, (unsigned char *)buf, datalen);
}

//shift left
void stream_shift_data_lambda(Recv_Filter *p_filter)
{
    if (p_filter->recvIndex)
    {
        p_filter->recvIndex--;
        if (p_filter->recvIndex)
        {
            memmove(p_filter->recvBuf, p_filter->recvBuf + 1, p_filter->recvIndex);
        }
    }
}

//note: push data to filter buffer
void storeData(Recv_Filter *p_filter, uint8_t in_data)
{
    if (p_filter->recvIndex < MAX_RECV_SIZE)
    {
        p_filter->recvBuf[p_filter->recvIndex] = in_data;
        p_filter->recvIndex++;
    }
    else
    {
        memset(p_filter->recvBuf, 0, p_filter->recvIndex);
        p_filter->recvIndex = 0;
    }
}

//**********************************************
//水泵控制命令解码
//**********************************************
void Pump_Decode(uint8_t value)
{
	uint8_t status1,status2,status3,status4;
	status1 = value & 0x01;
	status2 = value & 0x02;
	status3 = value & 0x04;
	status4 = value & 0x08;
	if(status1)
		Pump_Sample_Flag[0] = 1;
	else
		Pump_Sample_Flag[0] = 0;
		
	if(status2)
		Pump_Sample_Flag[1] = 1;
	else
		Pump_Sample_Flag[1] = 0;
		
	if(status3)
		Pump_Sample_Flag[2] = 1;
	else
		Pump_Sample_Flag[2] = 0;
		
	if(status4)
		Pump_Sample_Flag[3] = 1;
	else
		Pump_Sample_Flag[3] = 0;
	
}

/*handle command*/
void Rx_decode_action(Cmd_Command *p_cmd_command)
{
    switch(p_cmd_command->set)
    {
        case CMD_SET_SYSTEM:
            switch(p_cmd_command->id)
            {
                case CID_SYSTEM_HEALTH:
                    if(p_cmd_command->value[0] == 0xaa)
                    {
                        Health_Status_Flag = true;
                    }
                    break;
                case CID_SYSTEM_RESET:
                    switch(p_cmd_command->value[0])
                    {
                        case 0x01:
                            //Generates a system reset.
                            __disable_fault_irq();   
                            NVIC_SystemReset();
                            break;
                        case 0x02:
                            //write in flash
                            Update_Flag = 0x01; 
                            FLASH_Unlock();
                            EE_WriteVariable(VirtAddVarTab[App_update_Add],Update_Flag);
                            FLASH_Lock();
                            //Generates a system reset.
                            __disable_fault_irq();   
                            NVIC_SystemReset();
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case CMD_SET_BOTTLE_SAMPLE:
            switch(p_cmd_command->id)
            {
                case CID_WINCH_CONTROL:
                    switch(p_cmd_command->value[0])
                    {
                        case CVALUE_WINCH_STOP:
                            winch_status = Winch_Stop;
                            break;
                        case CVALUE_WINCH_DOWN:
                            winch_status = Winch_Down;
                            break;
                        case CVALUE_WINCH_UP:
                            winch_status = Winch_Up;
                            break;
                        default:
                            winch_status = Winch_Stop;
                            break;
                    }
                    break;
                case CID_FIXTURE_CONTROL:
                    switch(p_cmd_command->id)
                    {
                        case 0x00:
                            Fixture_Release();
                            break;
                        case 0x01:
                            Fixture_clamp();
                            break;
                    }
             
                default:
                    break;
            }
            break;
        case CMD_SET_PUMP_SAMPLE:
            switch(p_cmd_command->id)
            {
                case CID_PUMP_CONTROL:
                    Pump_Decode(p_cmd_command->value[0]);
                    break;
                default:
                    break;
            }
            break;
        case CMD_SET_COMMON:
            switch(p_cmd_command->id)
            {
                case CID_THROW_CONTROL:
                    switch(p_cmd_command->value[0])
                    {
                        case CVALUE_NO_THROW:
                            Buoy_Flag[0] = 0;
                            Buoy_Flag[1] = 0;
                            break;
                        case CVALUE_THROW_1:
                            Buoy_Flag[0] = 1;
                            Buoy_Flag[1] = 0;
                            break;
                        case CVALUE_THROW_2:
                            Buoy_Flag[0] = 0;
                            Buoy_Flag[1] = 1;
                            break;
                        default:
                            Buoy_Flag[0] = 0;
                            Buoy_Flag[1] = 0;
                            break;
                    }
                    break;
            }
            break;
        case CMD_SET_PARAMETER:
            switch(p_cmd_command->id)
            {
                case CID_FIXED_MAX_PWM:
                    memcpy(&Fixed_MAX_PWM, (uint16_t *)p_cmd_command->value, 2);
                    if(Fixed_MAX_PWM > 1250)
                        Fixed_MAX_PWM = 1250;   
                    if(Fixed_MAX_PWM < 500)
                        Fixed_MAX_PWM = 500;  
                    //write in flash
                    FLASH_Unlock();
                    EE_WriteVariable(VirtAddVarTab[Max_PWM_Add],Fixed_MAX_PWM);
                    FLASH_Lock();
                    break;
                case CID_FIXED_MIN_PWM:
                    memcpy(&Fixed_MIN_PWM, (uint16_t *)p_cmd_command->value, 2);
                    if(Fixed_MIN_PWM > 500)
                        Fixed_MIN_PWM = 500;   
                    if(Fixed_MIN_PWM < 200)
                        Fixed_MIN_PWM = 200;  
                    //write in flash
                    FLASH_Unlock();
                    EE_WriteVariable(VirtAddVarTab[Min_PWM_Add],Fixed_MIN_PWM);
                    FLASH_Lock();
                    break;
                case CID_ROPE_LENGTH:
                    memcpy(&Rope_Length, (uint16_t *)p_cmd_command->value, 2);
                    if(Rope_Length > 300)
                        Rope_Length = 300;   
                    if(Rope_Length < 50)
                        Rope_Length = 50;  
                    //write in flash
                    FLASH_Unlock();
                    EE_WriteVariable(VirtAddVarTab[Rope_Len_Add],Rope_Length);
                    FLASH_Lock();
                    break;
                case CID_HOLE_NUM:
                    memcpy(&Hole_num, (uint8_t *)p_cmd_command->value, 1);
                    if(Hole_num > 10)
                        Hole_num = 10;   
                    if(Hole_num < 1)
                        Hole_num = 1;  
                    //write in flash
                    FLASH_Unlock();
                    EE_WriteVariable(VirtAddVarTab[Hole_Num_Add],Hole_num);
                    FLASH_Lock();
                    break;
                case CID_PERIMETER:
                    memcpy(&Perimeter, (uint8_t *)p_cmd_command->value, 1);
                    if(Perimeter > 20)
                        Perimeter = 20;   
                    if(Perimeter < 5)
                        Perimeter = 5;  
                    //write in flash
                    FLASH_Unlock();
                    EE_WriteVariable(VirtAddVarTab[Perimeter_Add],Perimeter);
                    FLASH_Lock();
                    break;
                default:
                    break;
            }
            break;                
        default:
            break;
    }
}

//headle rx data
void headle_data(Recv_Filter *p_filter)
{
	Header *p_head = (Header *)(p_filter->recvBuf);
    Cmd_Command cmd_command_recv;
	if(p_head->isAck == 0)
	{
		cmd_command_recv.set = GetCmdSet(p_head);
		cmd_command_recv.id = getCmdID(p_head);
		cmd_command_recv.value_length = p_head->length - sizeof(Header) - _SDK_CRC_DATA_SIZE - 2;
		memcpy(cmd_command_recv.value, (unsigned char *)p_head + sizeof(Header) + 2, cmd_command_recv.value_length);
        Rx_decode_action(&cmd_command_recv);
	}
	
}
void verifyHead(Recv_Filter *p_filter)
{
    Header *p_head = (Header *)(p_filter->recvBuf);
    
    if ((p_head->sof == _SDK_SOF) && (_SDK_CALC_CRC_HEAD(p_head, sizeof(Header)) == 0))
    {
        // no need handle
    }
    else
    {
        stream_shift_data_lambda(p_filter);
    }
}

void verifyData(Recv_Filter *p_filter)
{
    Header *p_head = (Header *)(p_filter->recvBuf);
    
    if (_SDK_CALC_CRC_TAIL(p_head, p_head->length) == 0)
    {
        headle_data(p_filter);
		//clear data
        memset(p_filter->recvBuf, 0, p_filter->recvIndex);
        p_filter->recvIndex = 0;
    }
    else
    {
        //clear data
        memset(p_filter->recvBuf, 0, p_filter->recvIndex);
        p_filter->recvIndex = 0;
    }
}
void checkStream(Recv_Filter *p_filter)
{
    Header *p_head = (Header *)(p_filter->recvBuf);

    if (p_filter->recvIndex < sizeof(Header))
    {
        // Continue receive data, nothing to do
        return;
    }
    else if (p_filter->recvIndex == sizeof(Header))
    {
        // recv a full-head
        verifyHead(p_filter);
    }
    else if (p_filter->recvIndex == p_head->length)
    {
        verifyData(p_filter);
    }
}

void streamHandler(Recv_Filter *p_filter, uint8_t in_data)
{
    storeData(p_filter, in_data);
    checkStream(p_filter);
}

void byteHandler(const uint8_t in_data)
{
	//filter.reuseCount = 0;
    //filter.reuseIndex = MAX_RECV_SIZE;
	streamHandler(&filter, in_data);
}



//**********************************************
//解析串口接收数据中的信息
//**********************************************
// USART1 interrupt
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
        for(i = 0; i < usart1_p; i++)
        {
            byteHandler(Usart1_Rx_buff[i]);
        }
        //debug
        //USART1_DMA_Send((uint32_t)filter.recvBuf, filter.recvIndex);
        //Usart1_Received = 1;
        
		//设置传输数据长度  
        DMA_SetCurrDataCounter(DMA1_Channel5,USART_BUFF_SIZE);  
        //打开DMA  
        DMA_Cmd(DMA1_Channel5,ENABLE);   
    }    
    OSIntExit(); 
}

//TASK USART Decode
void Task_USART_Decode(void *p_arg)
{
	OS_ERR err;
	(void)p_arg; 

	while(1)
	{
		OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);	//系统延时100ms	
	}

}

