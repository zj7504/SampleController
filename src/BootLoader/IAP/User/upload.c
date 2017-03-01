/*******************************************************************************
** 文件名: 		upload.c
** 版本：  		1.0
** 工作环境: 	Keil uVision 5.11
** 作者: 		zj
** 生成日期: 	2016-12
** 功能:		上传文件的相关函数
** 相关文件:	common.h
** 修改日志：	2016-12  创建文档
*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "common.h"

/*******************************************************************************
  * @函数名称	SerialUpload
  * @函数说明   通过串口上传一个文件
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void SerialUpload(void)
{
    uint32_t status = 0;

    SerialPutString("\n\n\rSelect Receive File ... (press any key to abort)\n\r");

    if (GetKey() == CRC16)
    {
        //通过ymodem协议上传程序
        status = Ymodem_Transmit((uint8_t*)ApplicationAddress, (const uint8_t*)"UploadedFlashImage.bin", FLASH_IMAGE_SIZE);

        if (status != 0)
        {
            SerialPutString("\n\rError Occured while Transmitting File\n\r");
        }
        else
        {
            SerialPutString("\n\rFile Trasmitted Successfully \n\r");
        }
    }
    else
    {
        SerialPutString("\r\n\nAborted by user.\n\r");
    }

}

/*******************************文件结束***************************************/
