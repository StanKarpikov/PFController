/**
 * @file system.h
 * @author Stanislav Karpikov
 * @brief Board support package: system tools (header)
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

/** @addtogroup hdw_bsp_system
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
 * @brief Reset of all peripherals, Initializes the Flash interface and the Systick
 *
 * @param The status of the operation
 */
status_t system_init(void);

/**
 * @brief Wait for a delay (hard waiting)
 *
 * @param delay_ticks Ticks to wait
 *
 * @return The status of the operation
 */
status_t system_delay_ticks(uint32_t delay_ticks);

/**
 * @brief Get the local time
 *
 * @return 64-bit Unix timestamp (with us)
 */
uint64_t system_get_time(void);

/**
 * @brief Increment the time tick
 */
void system_increment_time(void);

/*
 * @brief Set the local time
 *
 * @param time 64-bit Unix timestamp (with us)
 */
void system_set_time(uint64_t time);

/** @} */
#endif /* _SYSTEM_H */
