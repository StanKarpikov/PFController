/**
 * @file uart.c
 * @author Stanislav Karpikov
 * @brief Board support package: UART
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/uart.h"
#include "defines.h"
#include "BSP/bsp.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"
#include "string.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define UART_INTERFACE_TIMEOUT (2000)
#define UART_DEBUG_TIMEOUT (2000)
#define USE_INTERFACE_AS_DEBUG (0)
#define UART_INTERFACE_BUFFER_DEFAULT_FILLER (0xFF)
#define RX_BUFFER_SIZE (0x3FF)
#define TX_BUFFER_SIZE (0x3FF)

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

typedef struct
{
    uint8_t tx_buffer[TX_BUFFER_SIZE];
    uint8_t tx_index;
    uint8_t tx_end;

    uint16_t rx_buffer[RX_BUFFER_SIZE];
    int rx_index;               /**< The next write position */
    int rx_readed;              /**< The next read position */
    uint8_t rx_overflow;  			/**< 0 - normal, 1 - rx buffer overflow */
} mcu_port_t;

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static UART_HandleTypeDef huart_interface={0};
static DMA_HandleTypeDef hdma_usart_interface_rx={0};
static DMA_HandleTypeDef hdma_usart_interface_tx={0};
static mcu_port_t mcu_port={0};

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t uart_interface_get_byte(uint8_t *byte)
{
    ARGUMENT_ASSERT(byte);
    /*
    if (mcu_port.rx_overflow) {
        mcu_port.rx_overflow = 0;
        mcu_port.rx_readed = mcu_port.rx_index;
        return PFC_WARNING;
    }
    */
    if (mcu_port.rx_buffer[mcu_port.rx_readed] != 0xFFFF)
    {
        *byte = mcu_port.rx_buffer[mcu_port.rx_readed] & 0xFF;
        mcu_port.rx_buffer[mcu_port.rx_readed] = 0xFFFF;
        mcu_port.rx_readed++;
        if (mcu_port.rx_readed == RX_BUFFER_SIZE)
        {
            mcu_port.rx_readed = 0;
        }
        return PFC_SUCCESS;
    }
    return PFC_NULL;
}

status_t uart_interface_rx_init(void)
{
	memset(mcu_port.rx_buffer, UART_INTERFACE_BUFFER_DEFAULT_FILLER, RX_BUFFER_SIZE);
	HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
	HAL_UART_Receive_DMA(&huart_interface, (uint8_t*)mcu_port.rx_buffer, RX_BUFFER_SIZE);
	
	mcu_port.rx_readed = 0;
	
	return PFC_SUCCESS;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(huart);
    HAL_UART_DeInit(huart);
    HAL_UART_MspDeInit(huart);
    uart_init();
    uart_interface_rx_init();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
}


status_t uart_interface_transmit(uint8_t* data, uint32_t length)
{
    ARGUMENT_ASSERT(data);

		HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_SET);
    if (HAL_UART_Transmit(&huart_interface, data, length, UART_INTERFACE_TIMEOUT) != HAL_OK)
    {
        return PFC_ERROR_HAL;
    };
		HAL_GPIO_WritePin(RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET);
		
    return PFC_SUCCESS;
}

status_t uart_debug_transmit(uint8_t* data, uint32_t length)
{
#if USE_INTERFACE_AS_DEBUG==1
    ARGUMENT_ASSERT(data);

    if (HAL_UART_Transmit(&huart_interface, data, length, UART_DEBUG_TIMEOUT) != HAL_OK)
    {
        return PFC_ERROR_HAL;
    };
#endif
    return PFC_SUCCESS;
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART_INTERFACE)
    {
        /* Peripheral clock enable */
        USART_INTERFACE_RCC_ENABLE();

        RS485_GPIO_PORT_ENABLE();
        GPIO_InitStruct.Pin = RS485_UART1_TX_Pin | RS485_UART1_RX_Pin | RE_485_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART_INTERFACE_AF;
        HAL_GPIO_Init(RS485_UART1_TX_GPIO_Port, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_RX Init */
        hdma_usart_interface_rx.Instance = USART_INTERFACE_DMA_RX_STREAM;
        hdma_usart_interface_rx.Init.Channel = USART_INTERFACE_DMA_RX_CHANNEL;
        hdma_usart_interface_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart_interface_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart_interface_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart_interface_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_usart_interface_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_usart_interface_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart_interface_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart_interface_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart_interface_rx) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(huart, hdmarx, hdma_usart_interface_rx);

        /* USART1_TX Init */
        hdma_usart_interface_tx.Instance = USART_INTERFACE_DMA_TX_STREAM;
        hdma_usart_interface_tx.Init.Channel = USART_INTERFACE_DMA_TX_CHANNEL;
        hdma_usart_interface_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart_interface_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart_interface_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart_interface_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart_interface_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart_interface_tx.Init.Mode = DMA_NORMAL;
        hdma_usart_interface_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart_interface_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart_interface_tx) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(huart, hdmatx, hdma_usart_interface_tx);

        HAL_NVIC_SetPriority(USART_INTERFACE_IRQN, USART_INTERFACE_IRQ_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(USART_INTERFACE_IRQN);
    }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART_INTERFACE)
    {
        USART_INTERFACE_RCC_DISABLE();

        HAL_GPIO_DeInit(RS485_UART1_TX_GPIO_Port, RS485_UART1_TX_Pin);
				HAL_GPIO_DeInit(RS485_UART1_RX_GPIO_Port, RS485_UART1_RX_Pin);
				HAL_GPIO_DeInit(RE_485_GPIO_Port, RE_485_Pin);
			
        HAL_DMA_DeInit(huart->hdmarx);
        HAL_DMA_DeInit(huart->hdmatx);

        HAL_NVIC_DisableIRQ(USART_INTERFACE_IRQN);
    }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void uart_init(void)
{
    huart_interface.Instance = USART_INTERFACE;
    huart_interface.Init.BaudRate = USART_INTERFACE_BAUDRATE;
    huart_interface.Init.WordLength = UART_WORDLENGTH_8B;
    huart_interface.Init.StopBits = UART_STOPBITS_1;
    huart_interface.Init.Parity = UART_PARITY_NONE;
    huart_interface.Init.Mode = UART_MODE_TX_RX;
    huart_interface.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_interface.Init.OverSampling = UART_OVERSAMPLING_16;
    huart_interface.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart_interface.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_RS485Ex_Init(&huart_interface, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
    {
        error_handler();
    }
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART_INTERFACE_IRQ(void)
{
  HAL_UART_IRQHandler(&huart_interface);
}

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  */
void USART_INTERFACE_DMA_RX_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_usart_interface_rx);
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void USART_INTERFACE_DMA_TX_IRQ(void)
{
  HAL_DMA_IRQHandler(&hdma_usart_interface_tx);
}
