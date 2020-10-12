/**
 * @file bsp.h
 * @author Stanislav Karpikov
 * @brief Board support package: general tools (header)
 */

#ifndef _BSP_H
#define _BSP_H

/** @addtogroup hdw_bsp_bsp
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"

#undef BOARD_STM32F767_MAIN /**< Define to use the main production board */
#define BOARD_STM32F723_DISCO /**< Define to use the STM32F723E-DISCO development board */

#if defined(BOARD_STM32F767_MAIN)
#include "board/board_stm32f767_main_v0_2.h"
#elif defined(BOARD_STM32F723_DISCO)
#include "board/board_stm32f723_disco.h"
#else
#error "Please define the board"
#endif

/** @} */
#endif /* _BSP_H */
