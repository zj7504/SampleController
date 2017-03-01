/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.h
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file contains all the functions prototypes for the
*                      EEPROM emulation firmware library.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "platform_config.h"

/* Exported constants --------------------------------------------------------*/

/* Define the STM32F10Xxx Flash page size depending on the used STM32 device */

#define PAGE_SIZE_EEPROM  (uint16_t)0x400  /* Page size = 1KByte */


/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS    ((u32)0x08018000) /* EEPROM emulation start address:
                                                  after 96KByte of used Flash memory */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((u32)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((u32)(EEPROM_START_ADDRESS + (PAGE_SIZE_EEPROM - 1)))

#define PAGE1_BASE_ADDRESS      ((u32)(EEPROM_START_ADDRESS + PAGE_SIZE_EEPROM))
#define PAGE1_END_ADDRESS       ((u32)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE_EEPROM - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((u16)0x0000)
#define PAGE1                   ((u16)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((u16)0x00AB)

/* Page status definitions */
#define ERASED                  ((u16)0xFFFF)     /* PAGE is empty */
#define RECEIVE_DATA            ((u16)0xEEEE)     /* PAGE is marked to receive data */
#define VALID_PAGE              ((u16)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((u8)0x00)
#define WRITE_IN_VALID_PAGE     ((u8)0x01)

/* Page full define */
#define PAGE_FULL               ((u8)0x80)

/* Variables' number */
#define NumbOfVar               ((u8)0x06)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
u16 EE_Init(void);
u16 EE_ReadVariable(u16 VirtAddress, u16* Data);
u16 EE_WriteVariable(u16 VirtAddress, u16 Data);

/* Virtual address defined by the user: 0xFFFF value is prohibited */
extern u16 VirtAddVarTab[NumbOfVar];

enum variable_add
{
    Max_PWM_Add = 0,
    Min_PWM_Add = 1,
    Rope_Len_Add = 2,
    Hole_Num_Add = 3,
    Perimeter_Add = 4,
    App_update_Add = 5,
};

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
