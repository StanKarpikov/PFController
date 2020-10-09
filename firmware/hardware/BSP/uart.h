/**
 * @file uart.h
 * @author Stanislav Karpikov
 * @brief Board support package: UART (header)
 */

#ifndef _UART_H
#define _UART_H

/** @addtogroup hdw_bsp_uart
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Transmit data to the interface UART
 *
 * @param data The data block pointer
 * @param length The size of the data block
 *
 * @return The status of the operation
 */
status_t uart_interface_transmit(uint8_t* data, uint32_t length);

/**
 * @brief Transmit data to the interface UART
 *
 * @param data The data block pointer
 * @param length The size of the data block
 *
 * @return The status of the operation
 */
status_t uart_debug_transmit(uint8_t* data, uint32_t length);

/**
 * @brief USART interface Initialization Function
 *
 * @return The status of the operation
 */
status_t uart_init(void);

/**
 * @brief Init receiving process
 *
 * @param byte A byte read from the interface
 *
 * @return The status of the operation
 */
status_t uart_interface_rx_init(void);

/**
 * @brief Get received byte (the oldest one)
 *
 * @param byte A byte read from the interface
 *
 * @return The status of the operation
 */
status_t uart_interface_get_byte(uint8_t* byte);

/** @} */
#endif /* _UART_H */
