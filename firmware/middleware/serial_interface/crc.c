/**
 * @file crc.c
 * @author Stanislav Karpikov
 * @brief Calculate CRC16
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "crc.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define CRC16_INITIAL_VALUE (0x1D0F) /**< Initial CRC16 value, 0x1D0F for CRC16-CCITT */
#define CRC16_POLYNOMIAL_VALUE (0x1021) /**< Polynomial CRC16 value, 0x1021 = 0001 0000 0010 0001  (0, 5, 12) for CRC16-CCITT */

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
 * @brief Calculate CRC16 (CRC16-CCITT)
 * 
 * @datablock The pointer to the data block
 * @len The size of the data block
 */
uint16_t crc16(uint8_t *datablock, uint32_t len)
{
    uint16_t crc = CRC16_INITIAL_VALUE;
    uint16_t polynomial = CRC16_POLYNOMIAL_VALUE;
    uint8_t b, i;
    for (b = 0; b < len; b++)
    {
        for (i = 0; i < 8; i++)
        {
            uint16_t bit = ((datablock[b] >> (7 - i) & 1) == 1);
            uint16_t c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}
