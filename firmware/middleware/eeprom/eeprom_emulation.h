/**
 * @file eeprom_emulation.c
 * @author Stanislav Karpikov
 * @brief Emulate EEPROM in flash memory
 */

/** NOTE: See http://www.st.com/web/en/resource/technical/document/application_note/CD00165693.pdf */

#ifndef __EEPROM_H
#define __EEPROM_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "defines.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define MAGIC_WORD (0x55AA) /**< A word that is used to check the storage */

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/** EEPROM return codes */
typedef enum
{
    EEPROM_OK = ((uint16_t)0x0000),           /**< The operation succeded */
    EEPROM_OUT_SIZE = ((uint16_t)0x0081),     /**< No empty EEPROM slots */
    EEPROM_BAD_ADDRESS = ((uint16_t)0x0082),  /**< Wrong address of a variable or a page */
    EEPROM_BAD_FLASH = ((uint16_t)0x0083),    /**< An error at the flash memory level */
    EEPROM_NOT_INIT = ((uint16_t)0x0084),     /**< The EEPROM module is not initialized, or a page is not initialized */
    EEPROM_SAME_VALUE = ((uint16_t)0x0085),   /**< The value to write is the same with the currently stored value */
    EEPROM_NO_VALID_PAGE = ((uint16_t)0x00AB) /**< There is no page that can receive data */
} eeprom_status_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
  * @brief  Init the EEPROM module
  *
  * @return The status of the operation
  */
eeprom_status_t eeprom_init(void);

/**
  * @brief	Returns the last stored variable data, if found,
  *			which correspond to the passed virtual address
  *
  * @param  address The variable virtual address
  * @param  data A pointer to data variable
  *
  * @return The status of the operation
  * @retval EEPROM_OK if variable was found
  * @retval EEPROM_BAD_ADDRESS if the variable was not found
  * @retval EEPROM_NO_VALID_PAGE if no valid page was found.
  */
eeprom_status_t eeprom_read_variable(uint16_t address, uint16_t *data);

/**
  * @brief  Writes/upadtes the variable data in EEPROM.
            The value is written only if differs from the one already saved at the same address
  *
  * @param  address The variable virtual address
  * @param  data A 16 bit data to be written
  *
  * @return The status of the operation, Flash error code: on write Flash error
  *	@retval EEPROM_SAME_VALUE If new data matches existing EEPROM data
  *	@retval EEPROM_OK on success
  *	@retval EEPROM_BAD_ADDRESS if address = 0xFFFF
  *	@retval EEPROM_PAGE_FULL if valid page is full
  *	@retval EEPROM_NO_VALID_PAGE if no valid page was found
  *	@retval EEPROM_OUT_SIZE if no empty EEPROM variables
  */
eeprom_status_t eeprom_update_variable(uint16_t address, uint16_t data);

#endif /*__EEPROM_H*/
