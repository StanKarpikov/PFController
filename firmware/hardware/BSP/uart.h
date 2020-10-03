/**
 * @file uart.h
 * @author Stanislav Karpikov
 * @brief Board support package: UART (header)
 */

#ifndef _UART_H
#define _UART_H

/*---------------------  INCLUDES  -------------------------------------------*/

#include "stdint.h"
#include "BSP/debug.h"

/*------------------  PUBLIC FUNCTIONS  --------------------------------------*/

status_t uart_debug_transmit(uint8_t* data, uint32_t length);
void uart_init(void);

#endif /* _UART_H */
