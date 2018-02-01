#ifndef __DMACOPY_H
#define __DMACOPY_H

#include "stm32f7xx.h"

#define DMA_INSTANCE               DMA2_Stream0
#define DMA_CHANNEL                DMA_CHANNEL_0
#define DMA_INSTANCE_IRQ           DMA2_Stream0_IRQn
#define DMA_INSTANCE_IRQHANDLER    DMA2_Stream0_IRQHandler

HAL_StatusTypeDef DMA_Copy_Init(void);
HAL_StatusTypeDef MemoryCopyDMA(uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);

#endif /* __DMACOPY_H */
