/**
 * @file iwdg.h
 * @author Stanislav Karpikov
 * @brief Board support package: Independent watchdog (header)
 */

#ifndef _IWDG_H
#define _IWDG_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
  * @brief IWDG Initialization Function
	*
  * @return The status of the operation
  */
status_t iwdg_init(void);

#endif /* _IWDG_H */
