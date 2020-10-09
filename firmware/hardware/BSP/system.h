/**
 * @file system.h
 * @author Stanislav Karpikov
 * @brief Board support package: system tools (header)
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

status_t system_init(void);
status_t system_delay_ticks(uint32_t delay_ticks);
uint64_t system_get_time(void);
void system_increment_time(void);
void system_set_time(uint64_t time);

#endif /* _SYSTEM_H */
