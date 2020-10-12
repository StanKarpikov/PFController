/**
 * @file dma.c
 * @author Stanislav Karpikov
 * @brief Board support package: DMA
 */

/** @addtogroup hdw_bsp_dma
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/bsp.h"
#include "BSP/dma.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @breif Enable DMA controller clock
 * 
 * @return The status of the operation
 */
status_t dma_init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */    
    HAL_NVIC_SetPriority(ADC_DMA_IRQN, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_DMA_IRQN);
    
    HAL_NVIC_SetPriority(TIMER_PWM_CH1_DMA_IRQN, 1, 0);
    HAL_NVIC_EnableIRQ(TIMER_PWM_CH1_DMA_IRQN);
    
    HAL_NVIC_SetPriority(TIMER_PWM_CH2_DMA_IRQN, 1, 0);
    HAL_NVIC_EnableIRQ(TIMER_PWM_CH2_DMA_IRQN);
    
    HAL_NVIC_SetPriority(TIMER_EFMC_CH1_DMA_IRQN, 1, 0);
    HAL_NVIC_EnableIRQ(TIMER_EFMC_CH1_DMA_IRQN);
    
    HAL_NVIC_SetPriority(TIMER_EFMC_CH2_DMA_IRQN, 1, 0);
    HAL_NVIC_EnableIRQ(TIMER_EFMC_CH2_DMA_IRQN);
    
    HAL_NVIC_SetPriority(USART_INTERFACE_DMA_RX_IRQN, 0, 0);
    HAL_NVIC_EnableIRQ(USART_INTERFACE_DMA_RX_IRQN);
    
    HAL_NVIC_SetPriority(TIMER_PWM_CH3_DMA_IRQN, 1, 0);
    HAL_NVIC_EnableIRQ(TIMER_PWM_CH3_DMA_IRQN);
    
    HAL_NVIC_SetPriority(USART_INTERFACE_DMA_TX_IRQN, 0, 0);
    HAL_NVIC_EnableIRQ(USART_INTERFACE_DMA_TX_IRQN);

    return PFC_SUCCESS;
}
/** @} */
