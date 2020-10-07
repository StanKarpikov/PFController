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

#define MAGIC_2BYTE (0x55AA)

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

typedef enum
{
    EEPROM_OK = ((uint16_t)0x0000),
    EEPROM_OUT_SIZE = ((uint16_t)0x0081),
    EEPROM_BAD_ADDRESS = ((uint16_t)0x0082),
    EEPROM_BAD_FLASH = ((uint16_t)0x0083),
    EEPROM_NOT_INIT = ((uint16_t)0x0084),
    EEPROM_SAME_VALUE = ((uint16_t)0x0085),
    EEPROM_NO_VALID_PAGE = ((uint16_t)0x00AB)
} eeprom_status_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

eeprom_status_t eeprom_init(void);
eeprom_status_t eeprom_read_variable(uint16_t address, uint16_t *data);
eeprom_status_t eeprom_update_variable(uint16_t address, uint16_t data);

#endif /*__EEPROM_H*/
