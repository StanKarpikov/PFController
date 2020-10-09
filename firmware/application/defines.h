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
#define STARTUP_STABILISATION_TIME (100U)  /**< The timeout before start the PFC operation */
#define SYNC_MINIMUM_PHASE         (0.03f) /**< The minimum phase difference that is considered as synchronisation */
#define PRELOAD_STABILISATION_TIME (100U)  /**< The timeout before preload is started */

#define EPS 0 /**< Epsilon, minimum float value to compare, used in ADC callback processing */

#define STARTUP_TIMEOUT (1000U) /**< The timeout after the divice starts */
#define PWM_PERIOD      (2000U) /**< PWM period in [us] */

#define PFC_NCHAN   (3U)   /**< Three-phase network */
#define BUF_NUM     (2U)   /**< Double buffering is used */
#define ADC_VAL_NUM (128U) /**< ADC measurements per period */
#define PFC_ACHAN   (0U)   /**< Channel A */
#define PFC_BCHAN   (1U)   /**< Channel B */
#define PFC_CCHAN   (2U)   /**< Channel C */

#define MATH_SQRT2 ((float)1.414213562373095) /**< The value of sqrt(2) */
#define MATH_PI    ((float)3.141592653589793) /**< The PI value */

/*--------------------------------------------------------------
											PUBLIC DEFINES::EEPROM
--------------------------------------------------------------*/

#define EEPROM_START_ADDRESS ((uint32_t)(0x08180000)) /**< The start address of the EEPROM block */
#define EEPROM_PAGE0_SECTOR  (FLASH_SECTOR_12)        /**< The ID of the first EEPROM sector */
#define EEPROM_PAGE1_SECTOR  (FLASH_SECTOR_13)        /**< The ID of the second EEPROM sector */

/*--------------------------------------------------------------
											PUBLIC DEFINES::INTERFACE
--------------------------------------------------------------*/

#define USART_INTERFACE_BAUDRATE     (115200UL) /**< Interface baudrate, bods (115200 is default) */
#define USART_INTERFACE_IRQ_PRIORITY (3U)       /**< The priority of the ADC IRQ. TODO: Check for overlapping with critical irqs */

#endif
