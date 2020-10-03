/**
 * @file bsp.h
 * @author Stanislav Karpikov
 * @brief Board support package: general tools (header)
 */

#ifndef _BSP_H
#define _BSP_H

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/debug.h"

#define BOARD_STM32F7

#ifdef BOARD_STM32F7
	#include "board/board_stm32f7.h"
#else
	#error "Please define the board"
#endif

/*------------------  PUBLIC FUNCTIONS  --------------------------------------*/


#endif /* _BSP_H */
