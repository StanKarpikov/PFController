/**
 * @file dma.c
 * @author Stanislav Karpikov
 * @brief Board support package: DMA
 */

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/dma.h"

#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    /* DMA2_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    /* DMA2_Stream3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    /* DMA2_Stream4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);
    /* DMA2_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
    /* DMA2_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
    /* DMA2_Stream7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}
