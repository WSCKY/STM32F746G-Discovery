/**
  ******************************************************************************
  * @file    ./Src/usbd_storage.c
  * @author  kyChu
  * @version V1.0.0
  * @date    26-November-2017
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage.h"
#include "FAT_TAB.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STORAGE_LUN_NBR                  1  
#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t writestatus, readstatus = 0;

/* USB Mass storage Standard Inquiry Data */
int8_t STORAGE_Inquirydata[] = { /* 36 */
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'k', 'y', '.', 'C', 'h', 'u', ' ', ' ', /* Manufacturer: 8 bytes  */
  'V', 'e', 'h', 'i', 'c', 'l', 'e', ' ', /* Product     : 16 Bytes */
  'U', 'p', 'g', 'r', 'a', 'd', 'e', 'r',
  '0', '.', '8','8',                      /* Version     : 4 Bytes  */
};

/* Private function prototypes -----------------------------------------------*/
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata, 
};
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the storage unit (medium)       
  * @param  lun: Logical unit number
  * @retval Status (0 : OK / -1 : Error)
  */
int8_t STORAGE_Init(uint8_t lun)
{
  return 0;
}

/**
  * @brief  Returns the medium capacity.      
  * @param  lun: Logical unit number
  * @param  block_num: Number of total block number
  * @param  block_size: Block size
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
	*block_num = TOTAL_SECTORS;
	*block_size = SECTOR_SIZE;
	return 0;
}

/**
  * @brief  Checks whether the medium is ready.  
  * @param  lun: Logical unit number
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_IsReady(uint8_t lun)
{
  int8_t ret = -1;

//  if()
//  {
      ret = 0;
//  }

  return ret;
}

/**
  * @brief  Checks whether the medium is write protected.
  * @param  lun: Logical unit number
  * @retval Status (0: write enabled / -1: otherwise)
  */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
  return 0;
}

/**
  * @brief  Reads data from the medium.
  * @param  lun: Logical unit number
  * @param  blk_addr: Logical block address
  * @param  blk_len: Blocks number
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	int8_t ret = -1;
	uint16_t Index = 0;
	uint16_t Address = 0;
	uint8_t *pBuf = (uint8_t *)buf;
	uint32_t Memory_Offset = blk_addr * 512;
	uint32_t Transfer_Length = blk_len * 512;
	if(lun == 0)
	{
		if(Memory_Offset < FATFS_USED_SIZE)
		{
			Address = Memory_Offset;
			for(Index = 0; Index < Transfer_Length; Index ++)
			{
				if(Address < BOOT_TABLE_USED_SIZE + BOOT_TABLE_OFFSET)
					pBuf[Index] = BOOT_TABLE[Address];
				else if(Address < BOOT_TABLE_SIZE + BOOT_TABLE_OFFSET - 2)
					pBuf[Index] = 0;
				else if(Address == BOOT_TABLE_SIZE + BOOT_TABLE_OFFSET - 2)
					pBuf[Index] = 0x55;
				else if(Address == BOOT_TABLE_SIZE + BOOT_TABLE_OFFSET - 1)
					pBuf[Index] = 0xAA;
				else if(Address < FAT_TABLE_SIZE + FAT1_TABLE_OFFSET)
					pBuf[Index] = FATn_TABLE[Address - FAT1_TABLE_OFFSET];
				else if(Address < FAT_TABLE_SIZE + FAT2_TABLE_OFFSET)
					pBuf[Index] = FATn_TABLE[Address - FAT2_TABLE_OFFSET];
				else if(Address < ROOT_TABLE_SIZE + ROOT_TABLE_OFFSET)
					pBuf[Index] = ROOT_TABLE[Address - ROOT_TABLE_OFFSET];
				else break;

				Address ++;
			}
			return 0;
		}
		else
		{
			return 0;
//			Memory_Offset -= FAT_USED_SIZE;
//			for(Index = 0; Index < Transfer_Length; Index += 4)
//			{
//				Readbuff[Index>>2] = *((uint32_t *)(FAT_TABLE_ADDR + Memory_Offset + Index));
//			}
		}
	}
	return ret;
}

/**
  * @brief  Writes data into the medium.
  * @param  lun: Logical unit number
  * @param  blk_addr: Logical block address
  * @param  blk_len: Blocks number
  * @retval Status (0 : OK / -1 : Error)
  */
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  int8_t ret = -1;  
//  
//  if(BSP_SD_IsDetected() != SD_NOT_PRESENT)
//  {
//    BSP_SD_WriteBlocks_DMA((uint32_t *)buf, blk_addr, blk_len);
//    WriteTimes ++;
//    /* Wait for Tx Transfer completion */
//    while (writestatus == 0)
//    {
//    }
//    writestatus = 0;
//    
//    /* Wait until SD card is ready to use for new operation */
//    while (BSP_SD_GetCardState() != SD_TRANSFER_OK)
//    {
//    }
//    
    ret = 0;
//  }
  return ret;
}

/**
  * @brief  Returns the Max Supported LUNs.   
  * @param  None
  * @retval Lun(s) number
  */
int8_t STORAGE_GetMaxLun(void)
{
  return(STORAGE_LUN_NBR - 1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
