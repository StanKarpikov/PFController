/**
 * @file defines.h
 * @author Stanislav Karpikov
 * @brief Global settings and defines
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

/* Hardware settings (TODO: move to the panel configuration) */
#define STARTUP_STABILISATION_TIME (100)
#define SYNC_MINIMUM_PHASE (0.03f)
#define PRELOAD_DELAY (100)

#define EPS 0 /**< Used in ADC callback processing */

#define STARTUP_TIMEOUT (1000)
#define PWM_PERIOD (2000)

#define PFC_NCHAN   (3)    //three-phase network
#define BUF_NUM     (2)    //Double buffering
#define ADC_VAL_NUM (128)  //Count per period

#define SYNC_PHASE_ERROR    (0.03f)
#define SYNC_PHASE_DELTAMIN (5e-4)


#define MATH_PI (3.141592653589793)

/*--------------------------------------------------------------
											PUBLIC DEFINES::EEPROM
--------------------------------------------------------------*/

#define EEPROM_START_ADDRESS  ((uint32_t)(0x08180000))
#define EEPROM_PAGE0_SECTOR (FLASH_SECTOR_12)
#define EEPROM_PAGE1_SECTOR (FLASH_SECTOR_13)

/*--------------------------------------------------------------
											PUBLIC DEFINES::INTERFACE
--------------------------------------------------------------*/

#define USART_INTERFACE_BAUDRATE (115200)
#define USART_INTERFACE_IRQ_PRIORITY (3)

#endif
