/**
 * @file uart.h
 * @author Stanislav Karpikov
 * @brief Board support package: UART (header)
 */

#ifndef _UART_H
#define _UART_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t uart_interface_transmit(uint8_t* data, uint32_t length);
status_t uart_debug_transmit(uint8_t* data, uint32_t length);
void uart_init(void);
status_t uart_interface_rx_init(void);
status_t uart_interface_get_byte(uint8_t* byte);

#endif /* _UART_H */
