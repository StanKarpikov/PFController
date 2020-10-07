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

#define CRC16_INITIAL_VALUE (0x1D0F)

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

uint16_t crc16(unsigned char *pcBlock, unsigned char len)
{
    uint16_t crc = CRC16_INITIAL_VALUE;  // initial value
    uint16_t polynomial = 0x1021;        // 0001 0000 0010 0001  (0, 5, 12)
    unsigned char b, i;
    for (b = 0; b < len; b++)
    {
        for (i = 0; i < 8; i++)
        {
            uint16_t bit = ((pcBlock[b] >> (7 - i) & 1) == 1);
            uint16_t c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}
