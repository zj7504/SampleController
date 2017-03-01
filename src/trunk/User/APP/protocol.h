#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROTOCOL_VALUE_SIZE 128
#define BUFFER_SIZE 256
#define MAX_RECV_SIZE (BUFFER_SIZE)
#define _SDK_SOF ((uint8_t)(0xAA))
#define _SDK_HEAD_DATA_LEN (sizeof(Header) - 2)
#define _SDK_CRC_HEAD_SIZE (2) // CRC16
#define _SDK_CRC_DATA_SIZE (4) // CRC32
#define _SDK_FULL_DATA_SIZE_MIN (sizeof(Header) + _SDK_CRC_DATA_SIZE)
#define _SDK_U32_SET(_addr, _val) (*((unsigned int *)(_addr)) = (_val))
//CRC 16
#define _SDK_CALC_CRC_HEAD(_msg, _len)     \
    sdk_stream_crc16_calc((const unsigned char *)(_msg), _len)
        
#define _SDK_CALC_CRC_TAIL(_msg, _len)     \
    sdk_stream_crc32_calc((const unsigned char *)(_msg), _len)
        

#pragma pack(1)
//Filter
typedef struct _Recv_Filter
{
    unsigned short reuseIndex;
    unsigned short reuseCount;
    unsigned short recvIndex;
    unsigned char recvBuf[BUFFER_SIZE];
} Recv_Filter;
//header
typedef struct _Header
{
    uint8_t sof;
    uint8_t length;
    uint8_t isAck : 1;
    uint8_t version : 7;
    uint8_t reserve;
    uint16_t crc16;
} Header;

//ask command- MCU send to FMU
typedef struct _Ask_Command
{
    Header header;
    uint8_t set;
    uint8_t id;
    uint8_t value;
    uint32_t crc32;
} Ask_Command;
/*
typedef struct _Ask_Command
{
    Header header;
    uint8_t set;
    uint8_t id;
    uint8_t value[1];
    uint32_t crc32;
} Ask_Command;*/

//cmd command - FMU send to MCU
typedef struct _Cmd_Command
{
    uint8_t set;
    uint8_t id;
	uint8_t value_length;
    uint8_t value[PROTOCOL_VALUE_SIZE];	//temporary
} Cmd_Command;
#pragma pack()

    
//CMD SET
enum CMD_SET
{
    CMD_SET_SYSTEM = 0x00,
    CMD_SET_BOTTLE_SAMPLE = 0x01,
    CMD_SET_PUMP_SAMPLE = 0x02,
    CMD_SET_COMMON = 0x03,
    CMD_SET_PARAMETER = 0xA0,
};

//ASK SET
enum ASK_SET
{
    ASK_SET_SYSTEM = 0x00,
    ASK_SET_BOTTLE_SAMPLE = 0x01,
    ASK_SET_PUMP_SAMPLE = 0x02,
    ASK_SET_COMMON = 0x03,
};

//CMD ID
enum CMD_SYSTEM_ID
{
    CID_SYSTEM_HEALTH = 0x00,
    CID_SYSTEM_RESET = 0xCC,
};
enum CMD_BOTTLE_ID
{
    CID_WINCH_CONTROL = 0x00,
    CID_FIXTURE_CONTROL = 0x01,
};
enum CMD_PUMP_ID
{
    CID_PUMP_CONTROL = 0x00,
};
enum CMD_COMMON_ID
{
    CID_THROW_CONTROL = 0x00,
};
enum CMD_PARAMETER_ID
{
    CID_FIXED_MAX_PWM = 0x01,
    CID_FIXED_MIN_PWM = 0x02,
    CID_ROPE_LENGTH = 0x03,
    CID_HOLE_NUM = 0x04,
    CID_PERIMETER = 0x05,
};

//ASK ID
enum ASK_SYSTEM_ID
{
    AID_SYSTEM_HEALTH = 0x00,
};
enum ASK_BOTTLE_ID
{
    AID_SAMPLE_STATUS = 0x00,
};
enum ASK_PUMP_ID
{
    AID_WATER_FULL = 0x00,
    AID_WATER_TOUCH = 0x01,
    AID_PUMP_STATUS = 0x02,
};
enum ASK_COMMON_ID
{
    AID_THROW_STATUS = 0x00,
};

//Value
enum CMD_WINCH_CONTROL_VALUE
{
    CVALUE_WINCH_STOP = 0x00,
    CVALUE_WINCH_DOWN = 0x01,
    CVALUE_WINCH_UP = 0x02,
};

enum CMD_THROW_CONTROL_VALUE
{
    CVALUE_NO_THROW = 0x00,
    CVALUE_THROW_1 = 0x01,
    CVALUE_THROW_2 = 0X02,
};
enum ASK_BOTTLE_SAMPLE_VALUE
{
    AVALUE_SAMPLE_NO_COMPLETE = 0x00,
    AVALUE_SAMPLE_COMPLETE = 0x01,
};
enum ASK_WATER_FULL_VALUE
{
    AVALUE_WATER_EMPTY = 0x00,
    AVALUE_WATER_FULL_1 = 0x01,
    AVALUE_WATER_FULL_2 = 0x02,
    AVALUE_WATER_FULL_1_2 = 0x03,
};
enum ASK_WATER_TOUCH_VALUE
{
    AVALUE_WATER_NO_TOUCH = 0x00,
    AVALUE_WATER_TOUCH = 0x01,
};
enum ASK_PUMP_STATUS_VALUE
{
    AVALUE_PUMP_ALL_OFF = 0x00,
    AVALUE_PUMP_1_ON = 0x01,
    AVALUE_PUMP_2_ON = 0x02,
};
enum ASK_THROW_STATUS_VALUE
{
    AVALUE_NO_THROW = 0x00,
    AVALUE_THROW_1 = 0x01,
    AVALUE_THROW_2 = 0x02,
};


extern	Ask_Command  Pump_Touch_Sensor;	
extern	Ask_Command  Pump_Full_Sensor;
extern	Ask_Command  Pump_Status;
extern	Ask_Command  Bottle_Sample;
extern	Ask_Command  Buoy_Release;

uint32_t sdk_stream_crc32_calc(const uint8_t *pMsg, size_t nLen);
void calculateCRC(void *p_data);    
    
    
#endif

