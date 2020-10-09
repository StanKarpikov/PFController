/**
 * @file crc.h
 * @author Stanislav Karpikov
 * @brief Calculate CRC16
 */

#ifndef __CRC_H__
#define __CRC_H__

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include <stdint.h>

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Calculate CRC16 (CRC16-CCITT)
 * 
 * @datablock The pointer to the data block
 * @len The size of the data block
 */
uint16_t crc16(uint8_t *datablock, uint32_t len);

/** @} */
#endif /* __CRC_H__ */

